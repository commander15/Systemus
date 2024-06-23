#ifndef SYSTEMUS_WIDGETS_GLOBAL_H
#define SYSTEMUS_WIDGETS_GLOBAL_H

#include <SystemusCore/global.h>

#ifdef SYSTEMUS_SHARED
#   ifdef SYSTEMUS_WIDGETS_BUILD
#       define SYSTEMUS_WIDGETS_EXPORT Q_DECL_EXPORT
#   else
#       define SYSTEMUS_WIDGETS_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define SYSTEMUS_WIDGETS_EXPORT
#endif

#define SYSTEMUS_INIT() systemus_init_widgets_resources();

inline void systemus_init_widgets_resources()
{
    Q_INIT_RESOURCE(systemus_widgets_res);
}


#endif // SYSTEMUS_WIDGETS_GLOBAL_H
