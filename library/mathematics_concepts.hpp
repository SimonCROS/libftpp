//
// Created by Simon Cros 15/12/2024.
//

#ifndef MATHEMATICS_CONCEPTS_HPP
#define MATHEMATICS_CONCEPTS_HPP

template <class T, class U>
concept addable_with = requires(T t, U u)
{
    t + u;
};

template <class T, class U>
concept subtractable_with = requires(T t, U u)
{
    t - u;
};

template <class T, class U>
concept multipliable_with = requires(T t, U u)
{
    t * u;
};

template <class T, class U>
concept dividable_with = requires(T t, U u)
{
    t / u;
};

template <class T>
concept negatable = requires(T t)
{
    { -t } -> std::same_as<T>;
};

template <class T, class U>
concept assign_addable_with = requires(T t, U u)
{
    t += u;
};

template <class T, class U>
concept assign_subtractable_with = requires(T t, U u)
{
    t -= u;
};

template <class T, class U>
concept assign_multipliable_with = requires(T t, U u)
{
    t *= u;
};

template <class T, class U>
concept assign_dividable_with = requires(T t, U u)
{
    t /= u;
};

#endif //MATHEMATICS_CONCEPTS_HPP
