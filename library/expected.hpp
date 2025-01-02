//
// Created by Simon Cros on 02/01/2025.
//

#ifndef EXPECTED_HPP
#define EXPECTED_HPP

#include <version>

#if __cpp_lib_expected >= 202211L && !defined(LIBFTPP_EXPECTED)

#include <expected>

template<class T, class E>
using Expected = std::expected<T, E>;

template<class E>
using Unexpected = std::unexpected<E>;

#else

#ifndef LIBFTPP_EXPECTED
#define LIBFTPP_EXPECTED
#endif

template <class T, class E>
class Expected
{

};

#endif

#endif //EXPECTED_HPP
