#include "Fcitx5Proxy.h"

#include "DBusProvider.h"
#include "common/common.h"
#include "dimcore/Events.h"
#include "dimcore/InputContext.h"

#include <QGuiApplication>

using namespace org::deepin::dim;

DIM_ADDON_FACTORY(Fcitx5Proxy);

const QDBusArgument &operator>>(const QDBusArgument &argument, BatchEvent &event)
{
    argument.beginStructure();
    argument >> event.type >> event.data;
    argument.endStructure();
    return argument;
}

Fcitx5Proxy::Fcitx5Proxy(Dim *dim)
    : ProxyAddon(dim, "fcitx5proxy")
    , dbusProvider_(new DBusProvider(this))
    , available_(dbusProvider_->available())
{
    connect(dbusProvider_, &DBusProvider::availabilityChanged, this, [this](bool available) {
        if (available_ != available) {
            available_ = available;

            updateInputMethods();
        }
    });

    updateInputMethods();
}

Fcitx5Proxy::~Fcitx5Proxy()
{
    icMap_.clear();
}

QList<InputMethodEntry> Fcitx5Proxy::getInputMethods()
{
    return inputMethods_;
}

void Fcitx5Proxy::createFcitxInputContext(InputContext *ic)
{
    if (!ic) {
        return;
    }

    if (!available_) {
        return;
    }

    FcitxQtStringKeyValueList list;
    FcitxQtStringKeyValue arg;

    arg.setKey("program");
    // TODO: it must be actual app name
    arg.setValue(QStringLiteral("dim"));
    list << arg;

    FcitxQtStringKeyValue arg2;
    arg2.setKey("display");
    arg2.setValue("x11:");
    list << arg2;

    auto result = dbusProvider_->imProxy()->CreateInputContext(list);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
    QObject::connect(
        watcher,
        &QDBusPendingCallWatcher::finished,
        this,
        [this, ic](QDBusPendingCallWatcher *watcher) {
            watcher->deleteLater();
            QDBusPendingReply<QDBusObjectPath, QByteArray> reply = *watcher;
            if (reply.isError()) {
                qDebug() << "create fcitx input context error:" << reply.error().message();
                return;
            }

            auto *icIface = new org::fcitx::Fcitx::InputContext1(QStringLiteral("org.fcitx.Fcitx5"),
                                                                 reply.value().path(),
                                                                 QDBusConnection::sessionBus(),
                                                                 this);
            if (icIface && icIface->isValid()) {
                icMap_[ic->id()] = icIface;
            }
        });
}

void Fcitx5Proxy::focusIn(uint32_t id)
{
    if (isICDBusInterfaceValid(id)) {
        icMap_[id]->asyncCall("FocusIn");
    }
}

void Fcitx5Proxy::focusOut(uint32_t id)
{
    if (isICDBusInterfaceValid(id)) {
        icMap_[id]->asyncCall("FocusOut");
    }
}

void Fcitx5Proxy::destroyed(uint32_t id)
{
    if (isICDBusInterfaceValid(id)) {
        icMap_[id]->asyncCall("DestroyIC");
    }
}

void Fcitx5Proxy::keyEvent(const InputMethodEntry &entry, InputContextKeyEvent &keyEvent)
{
    Q_UNUSED(entry);
    auto id = keyEvent.ic()->id();

    if (isICDBusInterfaceValid(id)) {
        // auto response = icMap_[id]->call("ProcessKeyEventBatch",
        //                                  keyEvent.keyValue(),
        //                                  keyEvent.keycode(),
        //                                  keyEvent.state(),
        //                                  keyEvent.isRelease(),
        //                                  keyEvent.time());
        auto response = icMap_[id]->ProcessKeyEventBatch(keyEvent.keyValue(),
                                                         keyEvent.keycode(),
                                                         keyEvent.state(),
                                                         keyEvent.isRelease(),
                                                         keyEvent.time());
        response.waitForFinished();
        QList<BatchEvent> events = response.value();

        auto ic = keyEvent.ic();
        // 从返回参数获取返回值

        for (const auto &event : events) {
            auto type = event.type;
            auto v = event.data;
            switch (type) {
            case BATCHED_COMMIT_STRING: {
                if (v.canConvert<QString>()) {
                    ic->updateCommitString(v.toString());
                }
                break;
            }
            case BATCHED_PREEDIT: {
                if (v.canConvert<PreeditKey>()) {
                    auto strs = v.value<PreeditKey>().info;
                    auto cursor = v.value<PreeditKey>().cursor;
                    for (auto &str : strs) {
                        ic->updatePreedit(str.text, cursor, cursor);
                    }
                }
                break;
            }
            case BATCHED_FORWARD_KEY: {
                if (v.canConvert<DBusForwardKey>()) {
                    ic->forwardKey(v.value<DBusForwardKey>().keysym,
                                   v.value<DBusForwardKey>().isRelease);
                }
                break;
            }
            default:
                qDebug() << "invalid event type " << type;
                return;
            }
        }
    }
}

void Fcitx5Proxy::updateInputMethods()
{
    if (!available_) {
        inputMethods_.clear();
        return;
    }

    auto call = dbusProvider_->controller()->AvailableInputMethods();
    auto watcher = new QDBusPendingCallWatcher(call, this);
    connect(
        watcher,
        &QDBusPendingCallWatcher::finished,
        this,
        [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<FcitxQtInputMethodEntryList> ims = *watcher;
            watcher->deleteLater();

            QList<InputMethodEntry> inputMethods;
            for (auto &im : ims.value()) {
                inputMethods.append(
                    { key(), im.uniqueName(), im.name(), im.nativeName(), im.label(), im.icon() });
            }

            inputMethods_.swap(inputMethods);
            Q_EMIT addonInitFinished(this);
        });
}
