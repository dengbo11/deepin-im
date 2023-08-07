#ifndef WL_CLIENT_CONNECTIONFD_H
#define WL_CLIENT_CONNECTIONFD_H

#include "ConnectionBase.h"

#include <wayland-client-protocol.h>

#include <QSocketNotifier>

#include <memory>
#include <set>
#include <string>
#include <unordered_map>

namespace wl {
namespace client {

class ConnectionRaw : public ConnectionBase
{
public:
    ConnectionRaw(struct wl_display *display, QObject *parent = nullptr);
    ~ConnectionRaw();

    struct wl_display *display() const override { return display_; }

private:
    struct wl_display *display_;
};

} // namespace client
} // namespace wl

#endif // !WL_CLIENT_CONNECTIONFD_H
