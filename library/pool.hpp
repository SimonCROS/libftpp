//
// Created by scros on 11/10/24.
//

#ifndef POOL_H
#define POOL_H
#include <cassert>
#include <stack>
#include <unordered_set>
#include <utility>
#include <cstdint>
#include <concepts>

template <class TType>
class Pool
{
public:
    class Object
    {
    public:
        template <class... TArgs>
            requires std::constructible_from<TType, TArgs...>
        Object(Pool* pool, TType* memory, TArgs... args): m_pool(pool), m_handle(memory)
        {
            std::construct_at(m_handle, std::forward<TArgs>(args)...);
        }

        Object(const Object& other) = delete;
        auto operator=(const Object& other) -> Object& = delete;

        Object(Object&& other) noexcept :
            m_pool(std::exchange(other.m_pool, nullptr)), m_handle(std::exchange(other.m_handle, nullptr))
        {
        }

        auto operator=(Object&& other) noexcept -> Object&
        {
            if (this != &other)
            {
                std::swap(m_pool, other.m_pool);
                std::swap(m_handle, other.m_handle);
            }

            return *this;
        }

        ~Object()
        {
            if (m_pool != nullptr)
            {
                assert(m_handle != nullptr && "m_handle should also be set to nullptr when Object is moved");

                std::destroy_at(m_handle);
                m_pool->release(m_handle);
            }
        }

        [[nodiscard]] auto operator->() -> TType*
        {
            return m_handle;
        }

        [[nodiscard]] auto operator->() const -> const TType*
        {
            return m_handle;
        }

        [[nodiscard]] auto operator*() -> TType&
        {
            return *m_handle;
        }

        [[nodiscard]] auto operator*() const -> const TType&
        {
            return *m_handle;
        }

        [[nodiscard]] auto get() -> TType*
        {
            return *m_handle;
        }

        [[nodiscard]] auto get() const -> const TType*
        {
            return *m_handle;
        }

    private:
        Pool* m_pool = nullptr;
        TType* m_handle = nullptr;
    };

    class ResourceAcquisitionException final : public std::exception
    {
        [[nodiscard]] const char* what() const noexcept override
        {
            return "Failed to acquire a new object";
        }
    };

    Pool() = default;

    explicit Pool(const size_t& size)
    {
        resize(size);
    }

    Pool(Pool& other) = delete;
    auto operator=(Pool& other) -> Pool& = delete;

    Pool(Pool&& other) = delete;
    auto operator=(Pool&& other) -> Pool& = delete;

    ~Pool()
    {
        assert(m_size == m_data.size() &&
            "Some objects have not been returned to the pool, this will create dandling pointers, stopping.");

        while (!m_data.empty())
        {
            m_allocator.deallocate(m_data.top(), 1);
            m_data.pop();
        }
    }

    auto resize(const size_t& size) -> void
    {
        while (size > m_size)
        {
            m_data.push(m_allocator.allocate(1));
            ++m_size;
        }
    }

    [[nodiscard]] auto getSize() const -> uint32_t
    {
        return m_size;
    }

    [[nodiscard]] auto getAvailableCount() const -> uint32_t
    {
        return m_data.size();
    }

    template <class... TArgs>
        requires std::constructible_from<TType, TArgs...>
    auto acquire(TArgs&&... p_args) -> Object
    {
        if (m_data.empty())
            throw ResourceAcquisitionException();

        TType* ptr = m_data.top();
        m_data.pop();
        return Object(this, ptr, std::forward<TArgs>(p_args)...);
    }

private:
    uint32_t m_size = 0;
    std::allocator<TType> m_allocator;
    std::stack<TType*> m_data;

    auto release(TType* ptr) -> void
    {
        m_data.push(ptr);
    }
};


#endif //POOL_H
