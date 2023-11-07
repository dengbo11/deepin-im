// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef OUTPUT_H
#define OUTPUT_H

#include "Listener.h"

#include <wayland-server-core.h>

struct wlr_output;

class Server;

class Output
{
public:
    Output(Server *server, struct wlr_output *wlr_output, wl_list *list);
    ~Output();

private:
    void frameNotify(void *data);
    void destroyNotify(void *data);

private:
    wl_list link_;

    Server *server_;
    struct wlr_output *wlr_output_;

    Listener<&Output::frameNotify> frame_;
    Listener<&Output::destroyNotify> destroy_;
};

#endif // !OUTPUT_H