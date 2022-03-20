#ifndef ULTRAGUI_GLOBAL_H
#define ULTRAGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ULTRAGUI_LIBRARY)
    #define ULTRAGUI_EXPORT Q_DECL_EXPORT
#else
    #if defined(ULTRAGUI_TESTINGAPP)
        #define ULTRAGUI_EXPORT
    #else
        #define ULTRAGUI_EXPORT Q_DECL_IMPORT
    #endif
#endif

#endif // ULTRAGUI_GLOBAL_H
