#ifndef QMYKNX_H
#define QMYKNX_H


#ifndef QT_STATIC

typedef signed char qint8;         /* 8 bit signed */
typedef unsigned char quint8;      /* 8 bit unsigned */
typedef short qint16;              /* 16 bit signed */
typedef unsigned short quint16;    /* 16 bit unsigned */
typedef int qint32;                /* 32 bit signed */
typedef unsigned int quint32;      /* 32 bit unsigned */

typedef long long qint64;           /* 64 bit signed */
typedef unsigned long long quint64; /* 64 bit unsigned */


typedef qint64 qlonglong;
typedef quint64 qulonglong;

using quint48 = quint64;
#define Q_UINT48_MAX Q_UINT64_C(281474976710655)

#ifdef my_KNX_STATIC
#  define MYLIB_EXPORT
#else
#if defined(QT_BUILD_KNX_LIB)
#  define MYLIB_EXPORT Q_DECL_EXPORT
#else
#  define MYLIB_EXPORT Q_DECL_IMPORT
#endif



#endif // QMYKNX_H
#endif
#endif