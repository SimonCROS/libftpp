//
// Created by scros on 11/10/24.
//

#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP
#include <cstdint>
#include <streambuf>
#include <vector>

class DataBuffer
{
public:
    class DataDeserializationException final : public std::exception
    {
        [[nodiscard]] const char* what() const noexcept override
        {
            return "Failed to retrieve data from buffer";
        }
    };

    template <typename TType>
    auto operator<<(const TType& rhs) -> DataBuffer&
    {
        mBuffer << rhs;
        //
        // auto data = static_cast<const void*>(&rhs);
        // bytes.insert(bytes.end(), data, data + sizeof(TType));
        return *this;
    }

    auto operator>>(int& rhs) -> DataBuffer&;

private:
    std::vector<uint8_t> bytes;
};

#endif //DATA_BUFFER_HPP
