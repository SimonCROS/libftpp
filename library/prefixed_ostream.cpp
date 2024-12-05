//
// Created by Simon Cros on 28/11/2024.
//

#include "prefixed_ostream.hpp"

#include <iostream>

PrefixedStreamBuf::PrefixedStreamBuf(std::streambuf* originalBuffer): m_originalBuffer(originalBuffer),
                                                                      m_atStartOfLine(true) {}

auto PrefixedStreamBuf::setPrefix(const std::string& prefix) -> void
{
    if (prefix.size() > std::numeric_limits<std::streamsize>::max())
        throw std::runtime_error("Prefix too long");

    m_prefix = prefix;
}

auto PrefixedStreamBuf::overflow(const int c) -> int
{
    if (c != traits_type::eof())
    {
        if (m_atStartOfLine && !m_prefix.empty())
        {
            const auto wrote = m_lineBuffer.sputn(m_prefix.c_str(), static_cast<std::streamsize>(m_prefix.size()));
            if (static_cast<std::streamsize>(m_prefix.size()) != wrote)
                return traits_type::eof();
        }
        m_atStartOfLine = c == '\n';
    }

    const auto res = m_lineBuffer.sputc(static_cast<char>(c));
    if (res == traits_type::eof())
        return res;

    if (m_atStartOfLine)
        sync();

    return c;
}

auto PrefixedStreamBuf::sync() -> int
{
    const auto line = m_lineBuffer.view();
    const auto lineLength = static_cast<std::streamsize>(line.length());
    const auto wrote = m_originalBuffer->sputn(line.data(), lineLength);

    m_lineBuffer = std::stringbuf();

    if (lineLength != wrote)
        return -1;

    return 0;
}

PrefixedOStream::PrefixedOStream(const std::ostream& originalStream): PrefixedStreamBuf(originalStream.rdbuf()),
                                                                      std::ostream(this) {}
