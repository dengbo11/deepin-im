#ifndef DBUSFRONTEND_H
#define DBUSFRONTEND_H

#include <dimcore/FrontendAddon.h>

#include <QDBusObjectPath>

#include <memory>

class WaylandConnection;
struct wl_registry;
struct zwp_input_method_v2;
struct zwp_input_method_context_v2;

namespace org {
namespace deepin {
namespace dim {

class InputContext1;

class WLFrontend : public QObject
{
    Q_OBJECT
    friend class zwp_input_method_v1_listener;

public:
    explicit WLFrontend();
    ~WLFrontend();

    void registryGlobal(struct wl_registry *registry,
                        uint32_t name,
                        const char *interface,
                        uint32_t version);

private:
    WaylandConnection *wl_;

    std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::shared_ptr<void>>> globals_;
};

} // namespace dim
} // namespace deepin
} // namespace org

#endif // !DBUSFRONTEND_H
