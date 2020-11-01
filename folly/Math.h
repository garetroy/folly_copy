/* THIS IS A COPY FOR PRACTICE, THE FOLLOWING COPY RIGHT IS BELOW
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once //Allows for this file to be included in multiple places without consequence

#include <stdint.h>
#include <cmath>
#include <limits>
#include <type_traits>

namespace folly {

namespace detail {

//GNOTE: Why do we need to have inline and constexpr?? constexpr -> inline
template <typename T>
inline constexpr T divFloorBranchless(T num, T denom) {
    return (num / denom) + 
        ((num%denom) * 
        (std::is_signed<T>::value && (num ^ denom) < 0 ? -1 : 0));
}

template <typename T>
inline constexpr T divFloorBranchful(T num, T denom) {
  return std::is_signed<T>::value && (num ^ denom) < 0 && num != 0
      ? (num + (num > 0 ? -1 : 1)) / denom - 1
      : num / denom;
}

template <typename T>
inline constexpr T divCeilBranchless(T num, T denom) {
  return (num / denom) +
      ((num % denom) != 0 ? 1 : 0) *
      (std::is_signed<T>::value && (num ^ denom) < 0 ? 0 : 1);
}

template <typename T>
inline constexpr T divCeilBranchful(T num, T denom)
{
  return (std::is_signed<T>::value && (num ^ denom) < 0) || num != 0
      ? num / denom
      : (num + (num > 0 ? -1 : 1)) / denom + 1;
}

template <typename T>
inline constexpr T divRoundAwayBranchless(T num, T denom) {
    return (num / denom) +
        ((num % denom) != 0 ? 1 : 0) *
        (std::is_signed<T>::value && (num ^ denom) < 0 ? -1 : 1);
}

template <typename T>
inline constexpr T divRoundAwayBranchful(T num, T denom)
{
    return num == 0 ? 0 
        : (num + (num > 0 ? -1 : 1)) / denom +
            (std::is_signed<T>::value && (num ^ denom) < 0 ? -1 : 1);
}

template <typename N, typename D>
using IdivResultType = typename std::enable_if<
    std::is_integral<N>::value && std::is_integral<D>::value &&
    !std::is_same<N, bool>::value && !std::is_same<D, bool>::value,
    decltype(N{1} / D{1})>::type;
} // namespace detail

#if defined(__arm__) && !FOLLY_AARCH64
constexpr auto kIntegerDivisionGivesRemainder = false;
#else
constexpr auto kIntegerDivisionGivesRemainder = true;
#endif

template <typename N, typename D>
inline constexpr detail::IdivResultType<N, D> divFloor(N num, D denom) {
    using R = decltype(num / denom);
    return detail::IdivResultType<N, D>(
        kIntegerDivisionGivesRemainder && std::is_signed<R>::value
            ? detail::divFloorBranchless<R>(num, denom)
            : detail::divFloorBranchful<R>(num, denom));
}

template <typename N, typename D>
inline constexpr detail::IdivResultType<N, D> divCeil(N num, D denom) {
    using R = decltype(num / denom);
    return detail::IdivResultType<N, D>(
        kIntegerDivisionGivesRemainder && std::is_signed<R>::value
            ? detail::divCeilBranchless<R>(num, denom)
            : detail::divCeilBranchful<R>(num, denom));
}

template<typename N, typename D>
inline constexpr detail::IdivResultType<N, D> divRoundAway(N num, D denom) {
    using R = decltype(num / denom);
    return detail::IdivResultType<N, D>(
        kIntegerDivisionGivesRemainder && std::is_signed<R>::value
            ? detail::divRoundAwayBranchless<R>(num, denom)
            : detail::divRoundAwayBranchful<R>(num, denom));
}

template <class _Tp>
constexpr std::enable_if_t<
    std::is_integral<_Tp>::value && !std::is_same<bool, _Tp>::value &&
        !std::is_null_pointer<_Tp>::value,
    _Tp>
midpoint(_Tp __a, _Tp __b) noexcept { 
    //GNOTE: enable_if allows to check if the type meets certain conditions
    //GNOTE: noexcept informs compiler there is no exception possible
    using _Up = std::make_unsigned_t<_Tp>;
    constexpr _Up __bitshift = std::numeric_limits<_Up>::digits - 1;

    _Up __diff = _Up(__b) - _Up(__a);
    _Up __sign_bit = __b < __a;

    _Up __half_diff = (__diff / 2) + (__sign_bit << __bitshift) + (__sign_bit & __diff);
    
    return __a + __half_diff;
}

template <class _TPtr>
constexpr std::enable_if_t<
    std::is_pointer<_TPtr>::value &&
        std::is_object<std::remove_pointer_t<_TPtr>>::value &&
        //GNOTE: This is getting the value the pointer is pointing at and checking if object and not void
        !std::is_void<std::remove_pointer_t<_TPtr>>::value &&
        (sizeof(std::remove_pointer_t<_TPtr>) > 0),
    _TPtr>
midpoint(_TPtr __a, _TPtr __b) noexcept {
    return __a + midpoint(std::ptrdiff_t(0), __b - __a);
}

template <class _Fp>
constexpr std::enable_if_t<std::is_floating_point<_Fp>::value, _Fp> midpoint(
    _Fp __a,
    _Fp __b) noexcept {
    constexpr _Fp __lo = std::numeric_limits<_Fp>::min()*2;
    constexpr _Fp __hi = std::numeric_limits<_Fp>::max()/2;
    return std::abs(__a) <= __hi && std::abs(__b) <= __hi ?
        (__a + __b)/2 
        : std::abs(__a) < __lo ? __a + __b/2
        : std::abs(__b) < __lo ? __a/2 + __b :
        __a/2 + __b/2;
}
        
} //namespace folly
