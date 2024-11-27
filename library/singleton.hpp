//
// Created by scros on 11/27/24.
//

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#if __cpp_inline_variables >= 201606L

#include <optional>
#include <concepts>
#include <stdexcept>

template <class TType>
class Singleton
{
public:
    Singleton() = delete;

    static auto instance() -> TType*
    {
        if (!m_instance)
            throw std::invalid_argument("Instance not yet created");
        return &m_instance.value();
    }

    template <class... TArgs>
        requires std::constructible_from<TType, TArgs...>
    static auto instantiate(TArgs&&... p_args) -> void
    {
        if (m_instance)
            throw std::invalid_argument("Instance already created");
        m_instance.emplace(std::forward<TArgs>(p_args)...);
    }

private:
    static inline std::optional<TType> m_instance = std::nullopt;
};

#endif

#endif //SINGLETON_HPP
