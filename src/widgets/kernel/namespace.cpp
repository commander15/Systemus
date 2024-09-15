#include "namespace.h"

#include <QtWidgets/qapplication.h>

inline void _systemus_init_widgets_resources()
{ Q_INIT_RESOURCE(systemus_widgets_res); }

namespace Systemus {

void initWidgets(QApplication &app)
{
    Q_UNUSED(app);
    _systemus_init_widgets_resources();
}

}
