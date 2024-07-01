#ifndef SYSTEMUS_CORE_GLOBAL_H
#define SYSTEMUS_CORE_GLOBAL_H

#include <SystemusCore/config.h>

#ifdef SYSTEMUS_SHARED
#   ifdef SYSTEMUS_CORE_BUILD
#       define SYSTEMUS_CORE_EXPORT Q_DECL_EXPORT
#   else
#       define SYSTEMUS_CORE_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define SYSTEMUS_CORE_EXPORT
#endif

#define S_DECLARE_PRIVATE(Class) friend class Class##Private;
#define S_DECLARE_PUBLIC(Class) friend class Class;

#define S_D(Class) Class##Private *d = static_cast<Class##Private *>(d_ptr.get());
#define S_Q(Class) Class *q = static_cast<Class *>(q_ptr);

void _s_register_internal_types();

#endif // SYSTEMUS_CORE_GLOBAL_H
