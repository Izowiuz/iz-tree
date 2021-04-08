#ifndef IZTREE_GLOBAL_H
#define IZTREE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(IZTREE_LIBRARY)
#define IZTREESHARED_EXPORT Q_DECL_EXPORT
#else
#define IZTREESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // IZTREE_GLOBAL_H
