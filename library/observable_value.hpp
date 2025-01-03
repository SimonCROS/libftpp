//
// Created by scros on 12/31/24.
//

#ifndef OBSERVABLE_VALUE_HPP
#define OBSERVABLE_VALUE_HPP
#include <utility>
#include <concepts>
#include <functional>
#include <vector>
#include <ranges>

template <class T>
class ObservableValue
{
public:
    using value_type = T;

private:
    using listener_type = std::function<void (const T&)>;
    using listeners_list_type = std::vector<listener_type>;

    T m_value;
    listeners_list_type m_listeners;

public:
    constexpr ObservableValue()
        noexcept(
            std::is_nothrow_default_constructible_v<T> &&
            std::is_nothrow_default_constructible_v<listeners_list_type>)
        requires
        std::default_initializable<T>
        : m_value()
    {
    }

    constexpr ObservableValue(const ObservableValue& other) = delete;

    constexpr ObservableValue(ObservableValue&& other) noexcept(std::is_nothrow_move_constructible_v<T> &&
        std::is_nothrow_move_constructible_v<listeners_list_type>) requires std::move_constructible<
        T> : m_value(std::move(other.m_value)), m_listeners(std::move(other.m_listeners))
    {
    }

    template <class... Args>
        requires std::constructible_from<T, Args...>
    constexpr explicit ObservableValue(std::in_place_t, Args&&... args) : m_value(std::forward<Args>(args)...)
    {
    }

    template <class U, class... Args>
        requires std::constructible_from<T, std::initializer_list<U>, Args...>
    constexpr explicit ObservableValue(std::in_place_t, std::initializer_list<U> ilist, Args&&... args)
        : m_value(ilist, std::forward<Args>(args)...)
    {
    }

    template <class U = T>
        requires
        std::constructible_from<T, U&&> &&
        (!std::same_as<std::remove_cvref<U>, std::in_place_t>) &&
        (!std::same_as<std::remove_cvref<U>, ObservableValue>)
    explicit(!std::convertible_to<T, U&&>)
    constexpr ObservableValue(U&& value) : m_value(std::forward<U>(value))
    {
    }

    auto operator=(const ObservableValue& other) -> ObservableValue& = delete;
    auto operator=(ObservableValue&& other) -> ObservableValue& = delete;

    template <class U>
    auto operator=(U&& newVal) -> ObservableValue& requires
        std::assignable_from<T&, U>
    {
        m_value = std::forward<U>(newVal);
        for (const auto& m_listener : m_listeners)
            m_listener(m_value);
        return *this;
    }

    auto addListener(listener_type listener) -> void
    {
        m_listeners.emplace_back(std::move(listener));
    }

#if __cpp_lib_containers_ranges >= 202202L
    template <class Range>
        requires std::ranges::input_range<Range> && std::convertible_to<
            std::ranges::range_reference_t<Range>, listener_type>
    auto addListeners(Range&& listeners) -> void
    {
        m_listeners.append_range(std::forward<Range>(listeners));
    }
#endif

    [[nodiscard]] auto operator->() & -> T*
    {
        return &m_value;
    }

    [[nodiscard]] auto operator->() const & -> const T*
    {
        return &m_value;
    }

    [[nodiscard]] auto operator*() -> T&
    {
        return m_value;
    }

    [[nodiscard]] auto operator*() const -> const T&
    {
        return m_value;
    }
};

#endif //OBSERVABLE_VALUE_HPP
