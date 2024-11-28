//
// Created by Simon Cros on 28/11/2024.
//

#ifndef PREFIXED_O_STREAM_HPP
#define PREFIXED_O_STREAM_HPP
#include <streambuf>
#include <ostream>
#include <string>

class PrefixedStreamBuf : public std::streambuf
{
public:
    explicit PrefixedStreamBuf(std::streambuf* originalBuffer);

    auto setPrefix(const std::string& prefix) -> void;

protected:
    int overflow(int c) override;

private:
    std::streambuf* m_originalBuffer;
    std::string m_prefix;
    bool m_atStartOfLine;
};

class PrefixedOStream final : PrefixedStreamBuf, public std::ostream
{
public:
    explicit PrefixedOStream(const std::ostream& originalStream);

    using PrefixedStreamBuf::setPrefix; // expose setPrefix from PrefixedStreamBuf
};

#endif //PREFIXED_O_STREAM_HPP
