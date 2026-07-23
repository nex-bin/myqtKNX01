#ifndef QNEWTRAITS_H
#define QNEWTRAITS_H

#include <array>
#include <bitset>
#include <deque>
#include <vector>

// 仅仅用来提示系统 QT_BEGIN_NAMESPACE是类型名称。
#include <QtNetwork/qhostaddress.h>

QT_BEGIN_NAMESPACE

template <typename T, typename ... Ts> struct is_type
{
    enum { value = false };
};

template <typename T, typename T1, typename ... Ts> struct is_type<T, T1, Ts...>
{
    enum { value = std::is_same<T, T1>::value || is_type<T, Ts...>::value };
};

QT_END_NAMESPACE

#endif // QMYKNXTRAITS_H
