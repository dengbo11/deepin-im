#ifndef FRONTENDADDON_H
#define FRONTENDADDON_H

#include "Addon.h"

namespace org {
namespace deepin {
namespace dim {

class FrontendAddon : public Addon
{
    Q_OBJECT

public:
    explicit FrontendAddon(Dim *dim, const QString &key);
};

} // namespace dim
} // namespace deepin
} // namespace org

#endif // !FRONTENDADDON_H
