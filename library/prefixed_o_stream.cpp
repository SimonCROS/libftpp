//
// Created by Simon Cros on 28/11/2024.
//

#include "prefixed_o_stream.hpp"

PrefixedStreamBuf::PrefixedStreamBuf(std::streambuf* originalBuffer): m_originalBuffer(originalBuffer), m_atStartOfLine(true)
{
}

auto PrefixedStreamBuf::setPrefix(const std::string& prefix) -> void
{
    if (prefix.size() > std::numeric_limits<std::streamsize>::max())
        throw std::runtime_error("Prefix too long");

    m_prefix = prefix;
}

int PrefixedStreamBuf::overflow(const int c)
{
    if (c != std::char_traits<char>::eof())
    {
        if (m_atStartOfLine && !m_prefix.empty())
        {
            const auto wrote = m_originalBuffer->sputn(m_prefix.c_str(), static_cast<std::streamsize>(m_prefix.size()));
            if (static_cast<std::streamsize>(m_prefix.size()) != wrote)
                return std::char_traits<char>::eof();
        }
        m_atStartOfLine = c == '\n';
    }
    return m_originalBuffer->sputc(static_cast<char>(c));
}

PrefixedOStream::PrefixedOStream(const std::ostream& originalStream): PrefixedStreamBuf(originalStream.rdbuf()), std::ostream(this)
{
}
