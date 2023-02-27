#ifndef DESKTOPPET_GLOBAL_H
#define DESKTOPPET_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DESKTOPPET_LIBRARY)
#  define DESKTOPPET_EXPORT Q_DECL_EXPORT
#else
#  define DESKTOPPET_EXPORT Q_DECL_IMPORT
#endif

#endif // DESKTOPPET_GLOBAL_H
