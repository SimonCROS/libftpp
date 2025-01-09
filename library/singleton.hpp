//
// Created by Simon Cros on 11/27/24.
//

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#if __cpp_inline_variables >= 201606L

#include <optional>
#include <concepts>
#include <stdexcept>

template <class Container, class... Args>
concept emplace_constructible_from = requires(Container& container, Args&&... args)
{
    container.emplace(std::forward<Args>(args)...);
};

template <class TType>
class Singleton
{
private:
    using container_type = std::optional<TType>;

public:
    Singleton() = delete;

    static auto instance() -> TType*
    {
        if (!ms_instance)
            throw std::invalid_argument("Instance not yet created");
        return &ms_instance.value();
    }

    // I'm not using std::constructible_from as it does not take context into account (so friendship is ignored)
    template <class... TArgs>
    static auto instantiate(TArgs&&... p_args) -> void requires requires { TType(std::forward<TArgs>(p_args)...); }
    {
        if (ms_instance)
            throw std::invalid_argument("Instance already created");

        if constexpr (emplace_constructible_from<container_type, TArgs...>)
            ms_instance.emplace(std::forward<TArgs>(p_args)...);
        else
            ms_instance.emplace(TType(std::forward<TArgs>(p_args)...));
    }

private:
    static inline container_type ms_instance = std::nullopt;
};

#endif

#endif //SINGLETON_HPP
