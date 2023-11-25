// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FCITX5PROXY_H
#define FCITX5PROXY_H

#include "Fcitx5InputContextIface.h"

#include <dimcore/ProxyAddon.h>

#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusPendingReply>
#include <QProcess>

namespace org {
namespace deepin {
namespace dim {

class DBusProvider;

class Fcitx5Proxy : public ProxyAddon
{
public:
    explicit Fcitx5Proxy(Dim *dim);
    ~Fcitx5Proxy() override;

    void initInputMethods() override;
    const QList<InputMethodEntry> &getInputMethods() override;
    bool keyEvent(const InputMethodEntry &entry, InputContextKeyEvent &keyEvent) override;
    void cursorRectangleChangeEvent(InputContextCursorRectChangeEvent &event) override;
    void updateSurroundingText(InputContextEvent &event) override;
    void createFcitxInputContext(InputContext *ic) override;
    void focusIn(uint32_t id) override;
    void focusOut(uint32_t id) override;
    void destroyed(uint32_t id) override;
    void setCurrentIM(const std::string &im) override;
    void addCapability(uint32_t id);

private:
    inline bool isICDBusInterfaceValid(uint32_t id)
    {
        return !icMap_.isEmpty() && icMap_.contains(id) && icMap_[id]->isValid();
    }

    void updateInputMethods();

    bool shouldBeIgnored(const std::string &uniqueName) const;
    void initDBusConn();
    void launchDaemon();

private:
    DBusProvider *dbusProvider_ = nullptr;
    bool available_ = false;
    QMap<uint32_t, org::fcitx::Fcitx::InputContext1 *> icMap_;
    QList<InputMethodEntry> inputMethods_;
    QProcess *fcitx5Proc_;
};

} // namespace dim
} // namespace deepin
} // namespace org

#endif // !FCITX5PROXY_H