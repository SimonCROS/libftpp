//
// Created by scros on 12/31/24.
//

#ifndef OBSERVABLE_VALUE_HPP
#define OBSERVABLE_VALUE_HPP
#include <utility>
#include <concepts>
#include <functional>
#include <vector>

template <class TType>
class ObservableValue
{
public:
    using type = TType;

private:
    using listener_type = std::function<void (const TType&)>;
    using listeners_list_type = std::vector<listener_type>;

    TType m_value;
    listeners_list_type m_listeners;

public:
    constexpr ObservableValue()
        noexcept(
            std::is_nothrow_default_constructible_v<TType> &&
            std::is_nothrow_default_constructible_v<listeners_list_type>)
        requires
        std::default_initializable<TType>
        : m_value{}
    {
    }

    constexpr ObservableValue(const ObservableValue& other) = delete;

    constexpr ObservableValue(ObservableValue&& other) noexcept(std::is_nothrow_move_constructible_v<TType> &&
        std::is_nothrow_move_constructible_v<listeners_list_type>) requires std::move_constructible<
        TType> : m_value{std::move(other.m_value)}, m_listeners{std::move(other.m_listeners)}
    {
    }

    template <class... Args>
        requires std::constructible_from<TType, Args...>
    constexpr explicit ObservableValue(std::in_place_t, Args&&... args) : m_value{std::forward<Args>(args)...}
    {
    }

    template <class U, class... Args>
        requires std::constructible_from<TType, std::initializer_list<U>, Args...>
    constexpr explicit ObservableValue(std::in_place_t, std::initializer_list<U> ilist, Args&&... args)
        : m_value{ilist, std::forward<Args>(args)...}
    {
    }

    template <class U = TType>
        requires
        std::constructible_from<TType, U&&> &&
        (!std::same_as<std::remove_cvref<U>, std::in_place_t>) &&
        (!std::same_as<std::remove_cvref<U>, ObservableValue>)
    explicit(std::convertible_to<TType, U&&>)
    constexpr ObservableValue(U&& value) : m_value(std::forward<U>(value))
    {
    }
};

#endif //OBSERVABLE_VALUE_HPP
