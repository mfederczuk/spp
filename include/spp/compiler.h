/*
 * Script Preprocessor - a general-purpose preprocessor program.
 * Copyright (C) 2022  Michael Federczuk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SPP_COMPILER_H
#define SPP_COMPILER_H



#define SPP_CXX17  ((__cplusplus      + 0) >= 201703L)
#define SPP_CXX20  ((__cplusplus      + 0) >= 202002L)

#define SPP_C99    ((__STDC_VERSION__ + 0) >= 199901L)
#define SPP_C23    ((__STDC_VERSION__ + 0) >= 202300L)


#define SPP_LINUX  ((__linux__ + 0) || (linux + 0) || (__linux + 0))

#if SPP_LINUX
	#define _GNU_SOURCE
#endif



#if SPP_LINUX
	#include <linux/version.h>

	#define SPP_LINUX_LEAST(major, patchlevel, sublevel) \
		(LINUX_VERSION_CODE >= KERNEL_VERSION(major, patchlevel, sublevel))
#else
	#define SPP_LINUX_LEAST(major, patchlevel, sublevel) 0
#endif



// you'll keep seeing #if GCC_LEAST(...) || defined(clang) since the Clang documentation isn't nearly as extensive as
// the GCC doc, so I can't check when the support for these were added (they may have been since the beginning, but,
// again, the doc is terrible and won't let me check the older Clang versions)

#define SPP_GCC_LEAST(major, minor) \
	(((__GNUC__ + 0) > (major)) || \
	 (((__GNUC__ + 0) == (major)) && ((__GNUC_MINOR__ + 0) >= (minor))))




#ifdef __cplusplus
	#define spp_constexpr  constexpr
#else
	#define spp_constexpr  static inline
#endif



#if (SPP_GCC_LEAST(2,5) || defined(__clang__))
	/**
	 * Use this instead of spp_attr_pure unless the function takes a pointer or reference.
	 */
	#define spp_attr_const  __attribute__((const))
#else
	#define spp_attr_const
#endif

#if (SPP_GCC_LEAST(2,96) || defined(__clang__))
	/**
	 * Only use this instead of spp_attr_const when a function takes a pointer or reference.
	 */
	#define spp_attr_pure  __attribute__((pure))
#else
	#define spp_attr_pure
#endif

#if (SPP_GCC_LEAST(3,1) || defined(__clang__))
	#define spp_attr_noreturn  __attribute__((noreturn))
#else
	#define spp_attr_noreturn
#endif

#if (SPP_CXX17 || SPP_C23)
	#define spp_nodiscard                [[nodiscard]]
	#define spp_attr_warn_unused_result
#elif (SPP_GCC_LEAST(3,4) || defined(__clang__))
	#define spp_nodiscard
	#define spp_attr_warn_unused_result  __attribute__((warn_unused_result))
#else
	#define spp_nodiscard
	#define spp_attr_warn_unused_result
#endif

#if (SPP_GCC_LEAST(3,3) || defined(__clang__))
	#define spp_attr_nonnull(...)  __attribute__((nonnull(__VA_ARGS__)))
	#define spp_attr_nonnull_all   __attribute__((nonnull))
#else
	#define spp_attr_nonnull(...)
	#define spp_attr_nonnull_all
#endif

#if (SPP_GCC_LEAST(4,3) || defined(__clang__))
	#define spp_attr_hot   __attribute__((hot))
	#define spp_attr_cold  __attribute__((cold))
#else
	#define spp_attr_hot
	#define spp_attr_cold
#endif



#ifdef NDEBUG
	#if (SPP_GCC_LEAST(4,5) || defined(__clang__)) && defined(NDEBUG)
		#define spp_unreachable(message)  __builtin_unreachable()
	#else
		#define spp_unreachable(message)  (void)0
	#endif
#else
	#include <assert.h>
	#define spp_unreachable(message)  assert(((void)(message), 0))
#endif



#if SPP_CXX20
	#define spp_if_likely(condition)    if(condition) [[likely]]
	#define spp_if_unlikely(condition)  if(condition) [[unlikely]]
#elif (SPP_GCC_LEAST(3,0) || defined(__clang__))
	#ifdef __cplusplus
		#define spp_if_likely(condition)    if(__builtin_expect(static_cast<long>(static_cast<bool>(condition)), static_cast<long>(true)))
		#define spp_if_unlikely(condition)  if(__builtin_expect(static_cast<long>(static_cast<bool>(condition)), static_cast<long>(false)))
	#elif SPP_C99
		#include <stdbool.h>

		#define spp_if_likely(condition)    if(__builtin_expect((long)(bool)(condition), (long)(true)))
		#define spp_if_unlikely(condition)  if(__builtin_expect((long)(bool)(condition), (long)(false)))
	#else
		#define spp_if_likely(condition)    if(__builtin_expect((long)!!(condition), 1L))
		#define spp_if_unlikely(condition)  if(__builtin_expect((long)!!(condition), 0L))
	#endif
#else
	#define spp_if_likely(condition)    if(condition)
	#define spp_if_unlikely(condition)  if(condition)
#endif



#endif /* SPP_COMPILER_H */
