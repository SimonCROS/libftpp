//
// Created by Simon Cros on 02/01/2025.
//

#ifndef EXPECTED_HPP
#define EXPECTED_HPP

#include <version>

#if __cpp_lib_expected >= 202211L && !defined(LIBFTPP_EXPECTED)

#include <expected>

using Unexpect = std::unexpect_t;

inline constexpr Unexpect unexpect = std::unexpect;

template<class T, class E>
using Expected = std::expected<T, E>;

template<class E>
using Unexpected = std::unexpected<E>;

#else

#ifndef LIBFTPP_EXPECTED
#define LIBFTPP_EXPECTED
#endif

struct Unexpect
{
    explicit Unexpect() = default;
};

inline constexpr Unexpect unexpect{};

template <class E>
class Unexpected;

template <class T, class E>
class Expected
{
public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = Unexpected<E>;

    template <class U>
    using rebind = Expected<U, error_type>;

private:
    using variant_type = std::variant<T, E>;

    template <class U>
    constexpr bool is_unexpected = false;

    template <class U>
    constexpr bool is_unexpected<Unexpected<U>> = true;

    variant_type m_variant;

public:
    constexpr Expected()
        requires
        std::default_initializable<variant_type>
        : m_variant()
    {
    }

    Expected(const Expected& other) = default;

    constexpr Expected(const Expected& other)
        requires std::copy_constructible<variant_type> && (!std::is_trivially_copy_constructible_v<variant_type>)
        : m_variant(other.m_variant)
    {
    }

    constexpr Expected(Expected&& other)
        noexcept(std::is_nothrow_move_constructible_v<variant_type>)
        requires std::move_constructible<variant_type> && (!std::is_trivially_move_constructible_v<variant_type>)\
        : m_variant(std::move(other.m_variant))
    {
    }

    template <class U, class G>
    constexpr
    explicit(!std::is_convertible_v<const typename Expected<U, G>::variant_type&, variant_type>)
    Expected(const Expected<U, G>& other)
        requires std::constructible_from<variant_type, const typename Expected<U, G>::variant_type&>
        && std::constructible_from<T, Expected<U, G>&>
        && std::constructible_from<T, Expected<U, G>>
        && std::constructible_from<T, const Expected<U, G>&>
        && std::constructible_from<T, const Expected<U, G>>
        && std::convertible_to<Expected<U, G>&, T>
        && std::convertible_to<Expected<U, G>, T>
        && std::convertible_to<const Expected<U, G>&, T>
        && std::convertible_to<const Expected<U, G>, T>
        : m_variant(other.m_variant)
    {
    }

    template <class U, class G>
    constexpr
    explicit(!std::is_convertible_v<typename Expected<U, G>::variant_type, variant_type>)
    Expected(Expected<U, G>&& other)
        requires std::constructible_from<variant_type, typename Expected<U, G>::variant_type>
        && std::constructible_from<T, Expected<U, G>&>
        && std::constructible_from<T, Expected<U, G>>
        && std::constructible_from<T, const Expected<U, G>&>
        && std::constructible_from<T, const Expected<U, G>>
        && std::convertible_to<Expected<U, G>&, T>
        && std::convertible_to<Expected<U, G>, T>
        && std::convertible_to<const Expected<U, G>&, T>
        && std::convertible_to<const Expected<U, G>, T>
        : m_variant(std::move(other.m_variant))
    {
    }

    template <class U = T>
    constexpr
    explicit(!std::is_convertible_v<U, T>)
    Expected(U&& v)
        requires (!std::same_as<std::remove_cvref_t<U>, std::in_place_t>)
        && (!std::same_as<Expected<T, E>, std::remove_cvref_t<U>>)
        && (!is_unexpected<std::remove_cvref_t<U>>)
        && std::constructible_from<T, U>
        : m_variant(std::in_place_index<0>, std::forward<U>(v))
    {
    }

    template <class G>
    constexpr
    explicit(!std::is_convertible_v<const G&, E>)
    Expected(const Unexpected<G>& e)
        requires std::constructible_from<E, const G&>
        : m_variant(std::in_place_index<1>, std::forward<const G&>(e.error()))
    {
    }

    template <class G>
    constexpr explicit(!std::is_convertible_v<G, E>)
    Expected(Unexpected<G>&& e)
        requires std::constructible_from<E, G>
        : m_variant(std::in_place_index<1>, std::forward<G>(e.error()))
    {
    }

    template <class... Args>
    constexpr explicit Expected(std::in_place_t, Args&&... args)
        requires std::constructible_from<T, Args...>
        : m_variant(std::in_place_index<0>, std::forward<Args>(args)...)
    {
    }

    template <class U, class... Args>
    constexpr
    explicit
    Expected(std::in_place_t, std::initializer_list<U> il, Args&&... args)
        requires std::constructible_from<T, std::initializer_list<U>&, Args...>
        : m_variant(std::in_place_index<0>, il, std::forward<Args>(args)...)
    {
    }

    template <class... Args>
    constexpr
    explicit
    Expected(Unexpect, Args&&... args)
        requires std::constructible_from<E, Args...>
        : m_variant(std::in_place_index<1>, std::forward<Args>(args)...)
    {
    }

    template <class U, class... Args>
    constexpr
    explicit
    Expected(Unexpect, std::initializer_list<U> il, Args&&... args)
        requires std::constructible_from<E, std::initializer_list<U>&, Args...>
        : m_variant(std::in_place_index<1>, il, std::forward<Args>(args)...)
    {
    }
};

#endif

#endif //EXPECTED_HPP
