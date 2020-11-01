/*
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

//GNOTE: A lot of this is a bit over my head, Though I am still looking up things that I haven't seen in here
// this will be more of a "familiarization"
#include <folly/portability/Config.h>

#ifndef __GNU_PREREQ
#if defined __GNUC__ && defined __GNUC_MINOR__
#define __GNUC_PREREQ(maj, min) \
 ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#define __GNUC_PREREQ(maj, min) 0
#endif
#endif

#ifndef __CLANG_PREREQ
#if defined __clang__ && defined __clang_major__ && defined __clang_minor__
#define __CLANG_PREREQ(maj, min) \
 ((__clang_major__ << 16) + __clang_minor__ >= ((maj) << 16) + (min))
#else
#define __CLANG_PREREQ(maj, min) 0
#endif
#endif

#if defined(__has_builtin)
#define FOLLY_HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
#define FOLLY_HAS_BUILTIN(...) 0
#endif

#if defined(__has_feature)
#define FOLLY_HAS_FEATURE(...) __has_feature(__VA_ARGS__)
#else
#define FOLLY_HAS_FEATURE(...) 0
#endif

#ifndef FOLLY_SANITIZE_ADDRESS
#if FOLLY_HAS_FEATURE(address_sanitizer) || __SANITIZE_ADDRESS__
#define FOLLY_SANITIZE_ADDRESS 1
#endif
#endif

#ifdef FOLLY_SANITIZE_ADDRESS
#if defined(__clang__)
#if __has_attribute(__no_sanitize__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_sanitize__("address"), __noinline__))
#elif __has_attribute(__no_address_safety_analysis__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_address_safety_analysis__, __noinline__))
#elif __has_attribute(__no_sanitize_address__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_sanitize_address__, __noinline__))
#endif
#elif defined(__GNUC__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_address_safety_analysis__, __noinline__))
#endif
#endif

#ifndef FOLLY_SANITIZE_THREAD
#if FOLLY_HAS_FEATURE(thread_sanitizer) || __SANITIZE_THREAD__
#define FOLLY_SANITIZE_THREAD 1
#endif
#endif

#if FOLLY_SANITIZE_THREAD
#define FOLLY_DISABLE_THREAD_SANITIZER \
  __attribute__((no_sanitize_thread, noinline))
#else
#define FOLLY_DISABLE_THREAD_SANITIZER
#endif

#ifndef FOLLY_SANITIZE_MEMORY
#if FOLLY_HAS_FEATURE(memory_sanitizer) || __SANITIZE_MEMORY__
#define FOLLY_SANITIZE_MEMORY 1
#endif
#endif

#if FOLLY_SANITIZE_MEMORY
#define FOLLY_DISABLE_MEMORY_SANITIZER \
  __attribute__((no_sanitize_memory, noinline))
#else
#define FOLLY_DISABLE_MEMORY_SANITIZER
#endif

#ifndef FOLLY_SANITIZE
#if defined(FOLLY_SANITIZE_ADDRESS) || defined(FOLLY_SANITIZE_THREAD) || \
    defined(FOLLY_SANITIZE_MEMORY)
#define FOLLY_SANITIZE 1
#endif
#endif

#if FOLLY_SANITIZE
#define FOLLY_DISABLE_UNDEFINED_BEHAVIOR_SANITIZER(...) \
  __attribute__((no_sanitize(__VA_ARGS__)))
#else
#define FOLLY_DISABLE_UNDEFINED_BEHAVIOR_SANITIZER(...)
#endif // FOLLY_SANITIZE

#define FOLLY_DISABLE_SANITIZERS                                 \
  FOLLY_DISABLE_ADDRESS_SANITIZER FOLLY_DISABLE_THREAD_SANITIZER \
      FOLLY_DISABLE_UNDEFINED_BEHAVIOR_SANITIZER("undefined")

#if defined(__GNUC__)
#define FOLLY_EXPORT __attribute__((__visibility__("default")))
#else
#define FOLLY_EXPORT
#endif

#ifdef _MSC_VER
#define FOLLY_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define FOLLY_NOINLINE __attribute__((__noinline__))
#else
#define FOLLY_NOINLINE
#endif

#ifdef _MSC_VER
#define FOLLY_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
#define FOLLY_ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
#define FOLLY_ALWAYS_INLINE inline
#endif

#if defined(_MSC_VER)
#define FOLLY_ATTR_VISIBILITY_HIDDEN
#elif defined(__GNUC__)
#define FOLLY_ATTR_VISIBILITY_HIDDEN __attribute__((__visibility__("hidden")))
#else
#define FOLLY_ATTR_VISIBILITY_HIDDEN
#endif

#if FOLLY_HAVE_WEAK_SYMBOLS
#define FOLLY_ATTR_WEAK __attribute__((__weak__))
#else
#define FOLLY_ATTR_WEAK
#endif

#ifndef FOLLY_MICROSOFT_ABI_VER
#ifdef _MSC_VER
#define FOLLY_MICROSOFT_ABI_VER _MSC_VER
#endif
#endif

#define FOLLY_ERASE FOLLY_ALWAYS_INLINE FOLLY_ATTR_VISIBILITY_HIDDEN

#if defined(__GNUC__) && !defined(__clang__)
#define FOLLY_ERASE_HACK_GCC FOLLY_ALWAYS_INLINE
#else
#define FOLLY_ERASE_HACK_GCC FOLLY_ERASE
#endif

#ifdef _MSC_VER
#define FOLLY_ERASE_TRYCATCH inline
#else
#define FOLLY_ERASE_TRYCATCH FOLLY_ERASE
#endif
