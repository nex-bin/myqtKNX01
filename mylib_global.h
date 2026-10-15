#ifndef MYLIB_GLOBAL_H
#define MYLIB_GLOBAL_H


#include <QtCore/qglobal.h>


// 静态编译模式：Q_KNX_EXPORT 展开为空
// #ifdef my_KNX_STATIC
// #  define MYLIB_EXPORT
// #else
// #if defined(QT_BUILD_KNX_LIB)
// #  define MYLIB_EXPORT Q_DECL_EXPORT
// #else
// #  define MYLIB_EXPORT Q_DECL_IMPORT
// #endif



// #if defined(MYLIB_LIBRARY)
// #  define MYLIB_EXPORT Q_DECL_EXPORT   // 编译库时：导出
// #else
// #  define MYLIB_EXPORT Q_DECL_IMPORT   // 编译调用方时：导入
// #endif

#endif // MYLIB_GLOBAL_H
