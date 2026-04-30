// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2025-2026 nevergiveupcpp

// Copyright 2025-2026 nevergiveupcpp
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NGU_OBFUSCXX_H
#define NGU_OBFUSCXX_H

#include <cstdint>
#include <initializer_list>

#if defined(_KERNEL_MODE) || defined(_WIN64_DRIVER)
using byte = std::uint8_t;
using max_align_t = double;
#endif

#include <type_traits>

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__ARM_NEON)
#include <arm_neon.h>
#elif defined(_WIN32)
#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
#include <immintrin.h>
#include <cpuid.h>
#else
#error Unsupported platform
#endif

#if (defined(__GNUC__) || defined(__clang__)) && defined(__AVX2__)
#define OBFUSCXX_HAS_AVX2 1
#elif defined(_MSC_VER) && !defined(__clang__) && !defined(_M_ARM64)
#define OBFUSCXX_HAS_AVX2 1
#else
#define OBFUSCXX_HAS_AVX2 0
#endif

#if defined(__clang__) || defined(__GNUC__)
#define OBFUSCXX_VOLATILE
#elif defined(_MSC_VER)
#define OBFUSCXX_VOLATILE volatile
#endif

#if defined(__clang__) || defined(__GNUC__)
#define OBFUSCXX_FORCEINLINE __attribute__((always_inline)) inline
#else
#define OBFUSCXX_FORCEINLINE __forceinline
#endif

#if defined(_KERNEL_MODE) || defined(_WIN64_DRIVER)
#define _mm256_extract_epi32(vec, idx) (((int32_t*)&(vec))[(idx)])
#endif

#if defined(__clang__) || defined(__GNUC__)
#define OBFUSCXX_MEM_BARRIER(...) __asm__ volatile("" : "+r"(__VA_ARGS__)::"memory");
#if defined(__aarch64__) || defined(_M_ARM64)
#define OBFUSCXX_MEM_BARRIER_VEC(v0, v1, sum) __asm__ volatile("" : "+w"(v0), "+w"(v1), "+r"(sum)::"memory");
#else
#define OBFUSCXX_MEM_BARRIER_VEC(v0, v1, sum) __asm__ volatile("" : "+x"(v0), "+x"(v1), "+r"(sum)::"memory");
#endif
#elif defined(_MSC_VER)
#define OBFUSCXX_MEM_BARRIER(...) _ReadWriteBarrier();
#define OBFUSCXX_MEM_BARRIER_VEC(v0, v1, sum) _ReadWriteBarrier();
#endif

#ifndef OBFUSCXX_DISABLE_WARNS
#define OBFUSCXX_RUNTIME_WARNING                                                                                       \
    [[deprecated(                                                                                                      \
        "OBFUSCXX: Runtime set() uses encrypt method without SIMD obfuscation. For better protection, "                \
        "initialize at compile-time."                                                                                  \
    )]]
#else
#define OBFUSCXX_RUNTIME_WARNING
#endif

namespace ngu {
    namespace detail {
        constexpr std::uint64_t splitmix64(std::uint64_t x) {
            x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
            x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
            return x ^ (x >> 31);
        }

        template<std::size_t N> OBFUSCXX_FORCEINLINE consteval std::uint64_t hash_compile_time(char const (&data)[N]) {
            std::uint64_t hash = 0;

            for (std::size_t i{}; i < N - 1; ++i) {
                hash += data[i] >= 'A' && data[i] <= 'Z' ? data[i] + ('a' - 'A') : data[i];
                hash += hash << 8;
                hash ^= hash >> 11;
            }

            hash += hash << 5;
            hash ^= hash >> 13;
            hash += hash << 10;

            return hash;
        }

        OBFUSCXX_FORCEINLINE std::uint64_t hash_runtime(char const* str) {
            std::size_t length = 0;
            while (str[length] != 0) {
                ++length;
            }

            std::uint64_t hash = 0;

            for (std::size_t i{}; i < length; ++i) {
                hash += str[i] >= 'A' && str[i] <= 'Z' ? str[i] + ('a' - 'A') : str[i];
                hash += hash << 8;
                hash ^= hash >> 11;
            }

            hash += hash << 5;
            hash ^= hash >> 13;
            hash += hash << 10;

            return hash;
        }

        OBFUSCXX_FORCEINLINE constexpr std::uint64_t rol64(const std::uint64_t x, int n) {
            n &= 63;
            if (n == 0) {
                return x;
            }
            return (x << n) | (x >> (64 - n));
        }

        OBFUSCXX_FORCEINLINE constexpr std::uint64_t ror64(const std::uint64_t x, int n) {
            n &= 63;
            if (n == 0) {
                return x;
            }
            return (x >> n) | (x << (64 - n));
        }

        OBFUSCXX_FORCEINLINE constexpr std::size_t align_up(const std::size_t size, const std::size_t multiple) {
            return (size + multiple - 1) & ~(multiple - 1);
        }
    } // namespace detail

    template<typename Vec> struct simd;

#if defined(__aarch64__) || defined(_M_ARM64)
    template<> struct simd<uint32x4_t> {
        using vec = uint32x4_t;
        static OBFUSCXX_FORCEINLINE vec shl4(const vec v) {
            return vshlq_n_u32(v, 4);
        }
        static OBFUSCXX_FORCEINLINE vec shr5(const vec v) {
            return vshrq_n_u32(v, 5);
        }
        static OBFUSCXX_FORCEINLINE vec vxor(const vec a, const vec b) {
            return veorq_u32(a, b);
        }
        static OBFUSCXX_FORCEINLINE vec vadd(const vec a, const vec b) {
            return vaddq_u32(a, b);
        }
        static OBFUSCXX_FORCEINLINE vec vsub(const vec a, const vec b) {
            return vsubq_u32(a, b);
        }
        static OBFUSCXX_FORCEINLINE vec broadcast(const std::uint32_t v) {
            return vdupq_n_u32(v);
        }
        static OBFUSCXX_FORCEINLINE vec from_scalar(const std::uint32_t v) {
            return vdupq_n_u32(v);
        }
        static OBFUSCXX_FORCEINLINE std::uint32_t to_scalar(const vec v) {
            return vgetq_lane_u32(v, 0);
        }
    };
#else
    template<> struct simd<__m128i> {
        using vec = __m128i;
        static OBFUSCXX_FORCEINLINE vec shl4(const vec v) {
            return _mm_slli_epi32(v, 4);
        }
        static OBFUSCXX_FORCEINLINE vec shr5(const vec v) {
            return _mm_srli_epi32(v, 5);
        }
        static OBFUSCXX_FORCEINLINE vec vxor(const vec a, const vec b) {
            return _mm_xor_si128(a, b);
        }
        static OBFUSCXX_FORCEINLINE vec vadd(const vec a, const vec b) {
            return _mm_add_epi32(a, b);
        }
        static OBFUSCXX_FORCEINLINE vec vsub(const vec a, const vec b) {
            return _mm_sub_epi32(a, b);
        }
        static OBFUSCXX_FORCEINLINE vec broadcast(const std::uint32_t v) {
            return _mm_set1_epi32(v);
        }
        static OBFUSCXX_FORCEINLINE vec from_scalar(const std::uint32_t v) {
            return _mm_cvtsi32_si128(v);
        }
        static OBFUSCXX_FORCEINLINE std::uint32_t to_scalar(const vec v) {
            return _mm_cvtsi128_si32(v);
        }
    };
#if OBFUSCXX_HAS_AVX2
    template<> struct simd<__m256i> {
        using vec = __m256i;
        static OBFUSCXX_FORCEINLINE vec shl4(const vec v) {
            return _mm256_slli_epi32(v, 4);
        }
        static OBFUSCXX_FORCEINLINE vec shr5(const vec v) {
            return _mm256_srli_epi32(v, 5);
        }
        static OBFUSCXX_FORCEINLINE vec vxor(const vec a, const vec b) {
            return _mm256_xor_si256(a, b);
        }
        static OBFUSCXX_FORCEINLINE vec vadd(const vec a, const vec b) {
            return _mm256_add_epi32(a, b);
        }
        static OBFUSCXX_FORCEINLINE vec vsub(const vec a, const vec b) {
            return _mm256_sub_epi32(a, b);
        }
        static OBFUSCXX_FORCEINLINE vec broadcast(const std::uint32_t v) {
            return _mm256_set1_epi32(v);
        }
        static OBFUSCXX_FORCEINLINE vec from_scalar(const std::uint32_t v) {
            return _mm256_set1_epi32(v);
        }
        static OBFUSCXX_FORCEINLINE std::uint32_t to_scalar(const vec v) {
            return _mm256_extract_epi32(v, 0);
        }
    };
#endif
#endif

#define OBFUSCXX_HASH(s) ngu::detail::hash_compile_time(s)
#define OBFUSCXX_HASH_RT(s) ngu::detail::hash_runtime(s)

#if defined(_KERNEL_MODE) || defined(_WIN64_DRIVER)
#define OBFUSCXX_ENTROPY                                                                                               \
    (detail::splitmix64(                                                                                               \
        (OBFUSCXX_HASH(__FILE__) * 0x517cc1b727220a95ULL) + ((std::uint64_t)__LINE__ * 0x9e3779b97f4a7c15ULL) +        \
        (detail::rol64((std::uint64_t)__COUNTER__, 37) ^ ((std::uint64_t)__LINE__ * 0xff51afd7ed558ccdULL))            \
    ))
#else
#define OBFUSCXX_ENTROPY                                                                                               \
    (ngu::detail::splitmix64(                                                                                          \
        OBFUSCXX_HASH(__FILE__) + ((std::uint64_t)__LINE__ * 0x9e3779b97f4a7c15ULL) +                                  \
        (OBFUSCXX_HASH(__TIME__) ^ ((std::uint64_t)__COUNTER__ << 32))                                                 \
    ))
#endif

    /// The obfuscation levels are based on the number of XTEA rounds. The more rounds, the more iterations are
    /// performed on the expanded state
    enum class obf_level : std::uint8_t {
        /// 2 xtea rounds
        low,
        /// 6 xtea rounds
        medium,
        /// a random number of xtea rounds, between 8 and 32
        high
    };

    template<class RawT, obf_level Level = obf_level::low, std::uint64_t Entropy = 0> class obfuscxx {
        template<std::uint64_t E> using rebind = obfuscxx<RawT, Level, E>;

        static_assert(Entropy != 0, "Entropy must not be zero, use the specified macros");

        using type = std::remove_extent_t<RawT>;

        static constexpr bool is_array = std::is_array_v<RawT>;
        static constexpr bool is_single = !is_array;
        static constexpr bool is_single_pointer = std::is_pointer_v<type> && is_single;
        static constexpr bool is_char = std::is_same_v<type, char> || std::is_same_v<type, const char>;
        static constexpr bool is_wchar = std::is_same_v<type, wchar_t> || std::is_same_v<type, const wchar_t>;

        static constexpr std::size_t type_size = std::is_array_v<RawT> ? std::extent_v<RawT> : 1;

        static constexpr std::size_t simd_alignment = OBFUSCXX_HAS_AVX2 ? 32 : 16;
        static constexpr std::size_t storage_multiple = OBFUSCXX_HAS_AVX2 ? 8 : 4;
        static constexpr std::size_t storage_alignment = is_array ? simd_alignment : alignof(std::uint64_t);
        static constexpr std::size_t storage_size =
            is_array ? detail::align_up(type_size, storage_multiple) : type_size;

        struct passkey {
            explicit passkey() = default;
        };

        static constexpr std::uint64_t seed{Entropy};
        static constexpr std::uint64_t iv[8] = {
            0xcbf43b227a01fe5aULL ^ seed,
            0x32703be7aaa7c38fULL ^ detail::ror64(seed, 13),
            0xb589959b3d854bbcULL ^ detail::rol64(seed, 29),
            0x73b3ef5578a97c8aULL ^ detail::ror64(seed, 41),
            0x92afafd27c6e16e9ULL ^ detail::rol64(seed, 7),
            0xee8291ae3070720aULL ^ detail::ror64(seed, 53),
            0xe2c0d70f73d6c4a0ULL ^ detail::rol64(seed, 19),
            0x82742897b912855bULL ^ detail::ror64(seed, 37),
        };
        static constexpr std::uint64_t iv_size = (sizeof(iv) / 8) - 1;
        static constexpr std::uint64_t unique_index = seed & iv_size;
        static constexpr std::uint64_t unique_value = iv[unique_index];

        static constexpr std::uint32_t xtea_rounds = (Level == obf_level::low)      ? 2
                                                     : (Level == obf_level::medium) ? 6
                                                                                    : (8 + ((unique_value % 13) * 2));

        static constexpr std::uint32_t xtea_delta = (0x9E3779B9 ^ static_cast<std::uint32_t>(unique_value)) | 1;

        static constexpr std::uint64_t encrypt(type value) {
            std::uint64_t val = to_uint64(value);

            std::uint32_t v0 = static_cast<std::uint32_t>(val);
            std::uint32_t v1 = static_cast<std::uint32_t>(val >> 32);
            std::uint32_t sum = 0;

            for (std::uint32_t i{}; i < xtea_rounds; ++i) {
                v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + static_cast<std::uint32_t>(iv[sum & 3]));
                sum += xtea_delta;
                v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + static_cast<std::uint32_t>(iv[(sum >> 11) & 3]));
            }

            return (static_cast<std::uint64_t>(v1) << 32) | v0;
        }

        template<typename Vec> static OBFUSCXX_FORCEINLINE Vec xtea_half_round(Vec va, Vec vb, std::uint32_t key_val) {
            using S = simd<Vec>;
            auto left = S::shl4(va);
            auto right = S::shr5(va);
            auto temp = S::vxor(left, right);
            temp = S::vadd(temp, va);
            auto key = S::broadcast(key_val);
            temp = S::vxor(temp, key);
            return S::vsub(vb, temp);
        }

        template<typename Vec> static OBFUSCXX_FORCEINLINE type decrypt_scalar(std::uint32_t v0, std::uint32_t v1) {
            using S = simd<Vec>;
            auto sv0 = S::from_scalar(v0);
            auto sv1 = S::from_scalar(v1);

            decrypt_rounds(sv0, sv1);

            v0 = S::to_scalar(sv0);
            v1 = S::to_scalar(sv1);
            return from_uint64((static_cast<std::uint64_t>(v1) << 32) | v0);
        }

        template<typename Vec> static OBFUSCXX_FORCEINLINE void decrypt_rounds(Vec& v0_vec, Vec& v1_vec) {
            std::uint32_t sum = xtea_delta * xtea_rounds;

            for (std::uint32_t i{}; i < xtea_rounds; ++i) {
                OBFUSCXX_MEM_BARRIER_VEC(v0_vec, v1_vec, sum)
                v1_vec = xtea_half_round(v0_vec, v1_vec, sum + static_cast<std::uint32_t>(iv[(sum >> 11) & 3]));
                sum -= xtea_delta;
                OBFUSCXX_MEM_BARRIER_VEC(v0_vec, v1_vec, sum)
                v0_vec = xtea_half_round(v1_vec, v0_vec, sum + static_cast<std::uint32_t>(iv[sum & 3]));
            }
        }

        static OBFUSCXX_FORCEINLINE type decrypt(std::uint64_t value) {
            OBFUSCXX_MEM_BARRIER(value)

            auto const v0 = static_cast<std::uint32_t>(value);
            auto const v1 = static_cast<std::uint32_t>(value >> 32);

#if defined(__aarch64__) || defined(_M_ARM64)
            return decrypt_scalar<uint32x4_t>(v0, v1);
#elif OBFUSCXX_HAS_AVX2
#if defined(_MSC_VER) && !defined(__clang__) && !defined(OBFUSCXX_MSVC_FORCE_AVX2)
            if (cpu_has_avx2()) {
                return decrypt_scalar<__m256i>(v0, v1);
            }
            return decrypt_scalar<__m128i>(v0, v1);
#else
            return decrypt_scalar<__m256i>(v0, v1);
#endif
#else
            return decrypt_scalar<__m128i>(v0, v1);
#endif
        }

        static OBFUSCXX_FORCEINLINE void decrypt_vectorized(
            const volatile std::uint64_t* src, type* dst, std::size_t count
        ) {
            std::size_t const aligned_count = detail::align_up(count, storage_multiple);

#if defined(__aarch64__) || defined(_M_ARM64)
            for (std::size_t i{}; i < aligned_count; i += 4) {
                uint32x4x2_t loaded =
                    vld2q_u32(reinterpret_cast<const std::uint32_t*>(const_cast<const std::uint64_t*>(src + i)));
                uint32x4_t v0_vec = loaded.val[0];
                uint32x4_t v1_vec = loaded.val[1];

                decrypt_rounds(v0_vec, v1_vec);

                uint32x4x2_t zipped = vzipq_u32(v0_vec, v1_vec);
                alignas(16) std::uint64_t out64[4];
                vst1q_u32(reinterpret_cast<std::uint32_t*>(&out64[0]), zipped.val[0]);
                vst1q_u32(reinterpret_cast<std::uint32_t*>(&out64[2]), zipped.val[1]);

                for (std::size_t j{}; j < 4 && (i + j) < count; ++j) {
                    dst[i + j] = from_uint64(out64[j]);
                }
            }

#elif OBFUSCXX_HAS_AVX2
#if defined(_MSC_VER) && !defined(__clang__) && !defined(OBFUSCXX_MSVC_FORCE_AVX2)
            if (cpu_has_avx2()) {
#endif
                for (std::size_t i{}; i < aligned_count; i += 8) {
                    const auto* p = reinterpret_cast<const __m256i*>(const_cast<const std::uint64_t*>(src + i));

                    __m256i d0 = _mm256_load_si256(p);
                    __m256i d1 = _mm256_load_si256(p + 1);
                    __m256i perm_even = _mm256_set_epi32(0, 0, 0, 0, 6, 4, 2, 0);
                    __m256i perm_odd = _mm256_set_epi32(0, 0, 0, 0, 7, 5, 3, 1);
                    __m256i v0_vec = _mm256_permute2x128_si256(
                        _mm256_permutevar8x32_epi32(d0, perm_even), _mm256_permutevar8x32_epi32(d1, perm_even), 0x20
                    );
                    __m256i v1_vec = _mm256_permute2x128_si256(
                        _mm256_permutevar8x32_epi32(d0, perm_odd), _mm256_permutevar8x32_epi32(d1, perm_odd), 0x20
                    );

                    decrypt_rounds(v0_vec, v1_vec);

                    __m128i v0_128lo = _mm256_castsi256_si128(v0_vec);
                    __m128i v1_128lo = _mm256_castsi256_si128(v1_vec);
                    __m128i v0_128hi = _mm256_extracti128_si256(v0_vec, 1);
                    __m128i v1_128hi = _mm256_extracti128_si256(v1_vec, 1);

                    alignas(32) std::uint64_t out64[8];
                    _mm_store_si128(reinterpret_cast<__m128i*>(&out64[0]), _mm_unpacklo_epi32(v0_128lo, v1_128lo));
                    _mm_store_si128(reinterpret_cast<__m128i*>(&out64[2]), _mm_unpackhi_epi32(v0_128lo, v1_128lo));
                    _mm_store_si128(reinterpret_cast<__m128i*>(&out64[4]), _mm_unpacklo_epi32(v0_128hi, v1_128hi));
                    _mm_store_si128(reinterpret_cast<__m128i*>(&out64[6]), _mm_unpackhi_epi32(v0_128hi, v1_128hi));

                    for (std::size_t j{}; j < 8 && (i + j) < count; ++j) {
                        dst[i + j] = from_uint64(out64[j]);
                    }
                }
#if defined(_MSC_VER) && !defined(__clang__) && !defined(OBFUSCXX_MSVC_FORCE_AVX2)
            } else {
                for (std::size_t i{}; i < aligned_count; i += 4) {
                    const auto* p = reinterpret_cast<const __m128i*>(const_cast<const std::uint64_t*>(src + i));

                    __m128i d0 = _mm_load_si128(p);
                    __m128i d1 = _mm_load_si128(p + 1);
                    __m128i mask = _mm_set1_epi64x(0xFFFFFFFF);
                    __m128i v0_vec = _mm_castps_si128(_mm_shuffle_ps(
                        _mm_castsi128_ps(_mm_and_si128(d0, mask)),
                        _mm_castsi128_ps(_mm_and_si128(d1, mask)),
                        _MM_SHUFFLE(2, 0, 2, 0)
                    ));
                    __m128i v1_vec = _mm_castps_si128(_mm_shuffle_ps(
                        _mm_castsi128_ps(_mm_srli_epi64(d0, 32)),
                        _mm_castsi128_ps(_mm_srli_epi64(d1, 32)),
                        _MM_SHUFFLE(2, 0, 2, 0)
                    ));

                    decrypt_rounds(v0_vec, v1_vec);

                    __m128i lo64 = _mm_unpacklo_epi32(v0_vec, v1_vec);
                    __m128i hi64 = _mm_unpackhi_epi32(v0_vec, v1_vec);
                    alignas(16) std::uint64_t out64[4];
                    _mm_store_si128(reinterpret_cast<__m128i*>(&out64[0]), lo64);
                    _mm_store_si128(reinterpret_cast<__m128i*>(&out64[2]), hi64);

                    for (std::size_t j{}; j < 4 && (i + j) < count; ++j) {
                        dst[i + j] = from_uint64(out64[j]);
                    }
                }
            }
#endif
#else
            for (std::size_t i{}; i < aligned_count; i += 4) {
                const auto* p = reinterpret_cast<const __m128i*>(const_cast<const std::uint64_t*>(src + i));

                __m128i const d0 = _mm_load_si128(p);
                __m128i const d1 = _mm_load_si128(p + 1);
                __m128i const mask = _mm_set1_epi64x(0xFFFFFFFF);
                __m128i v0_vec = _mm_castps_si128(_mm_shuffle_ps(
                    _mm_castsi128_ps(_mm_and_si128(d0, mask)),
                    _mm_castsi128_ps(_mm_and_si128(d1, mask)),
                    _MM_SHUFFLE(2, 0, 2, 0)
                ));
                __m128i v1_vec = _mm_castps_si128(_mm_shuffle_ps(
                    _mm_castsi128_ps(_mm_srli_epi64(d0, 32)),
                    _mm_castsi128_ps(_mm_srli_epi64(d1, 32)),
                    _MM_SHUFFLE(2, 0, 2, 0)
                ));

                decrypt_rounds(v0_vec, v1_vec);

                __m128i const lo64 = _mm_unpacklo_epi32(v0_vec, v1_vec);
                __m128i const hi64 = _mm_unpackhi_epi32(v0_vec, v1_vec);
                alignas(16) std::uint64_t out64[4];
                _mm_store_si128(reinterpret_cast<__m128i*>(&out64[0]), lo64);
                _mm_store_si128(reinterpret_cast<__m128i*>(&out64[2]), hi64);

                for (std::size_t j{}; j < 4 && (i + j) < count; ++j) {
                    dst[i + j] = from_uint64(out64[j]);
                }
            }
#endif
        }

        static constexpr std::uint64_t to_uint64(type value) {
            if constexpr (std::is_pointer_v<type>) {
                return reinterpret_cast<std::uint64_t>(value);
            } else if constexpr (std::is_floating_point_v<type>) {
                if constexpr (sizeof(type) == 4) {
                    return __builtin_bit_cast(std::uint32_t, value);
                } else {
                    return __builtin_bit_cast(std::uint64_t, value);
                }
            } else {
                return static_cast<std::uint64_t>(value);
            }
        }

        static OBFUSCXX_FORCEINLINE type from_uint64(std::uint64_t value) {
            if constexpr (std::is_pointer_v<type>) {
                return reinterpret_cast<type>(value);
            } else if constexpr (std::is_floating_point_v<type>) {
                if constexpr (sizeof(type) == 4) {
                    return __builtin_bit_cast(type, static_cast<std::uint32_t>(value));
                } else {
                    return __builtin_bit_cast(type, value);
                }
            } else {
                return static_cast<type>(value);
            }
        }

        static OBFUSCXX_FORCEINLINE bool cpu_has_avx2() {
#if !defined(__aarch64__) && !defined(_M_ARM64)
            static const bool cached = []() {
#if defined(_MSC_VER)
                int cpu_info[4]{};
                __cpuidex(cpu_info, 7, 0);
                return (cpu_info[1] & (1 << 5)) != 0;
#else
                unsigned int eax, ebx, ecx, edx;
                if (!__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx))
                    return false;
                return (ebx & (1 << 5)) != 0;
#endif
            }();
            return cached;
#else
            return false;
#endif
        }

    public:
        explicit consteval obfuscxx(passkey) {
            for (std::size_t i{}; i < type_size; ++i) {
                storage_[i] = seed ^ iv[i & iv_size];
            }
        }

        explicit consteval obfuscxx(type val) : obfuscxx(passkey{}) {
            storage_[0] = encrypt(val);
        }

        explicit consteval obfuscxx(type (&arr)[type_size]) : obfuscxx(passkey{}) {
            for (std::size_t i{}; i < type_size; ++i) {
                storage_[i] = encrypt(arr[i]);
            }
        }

        explicit consteval obfuscxx(const type (&arr)[type_size]) : obfuscxx(passkey{}) {
            for (std::size_t i{}; i < type_size; ++i) {
                storage_[i] = encrypt(arr[i]);
            }
        }

        consteval obfuscxx(const std::initializer_list<type>& list) : obfuscxx(passkey{}) {
            for (std::size_t i{}; const auto& v : list) {
                storage_[i++] = encrypt(v);
            }
        }

        OBFUSCXX_FORCEINLINE type get() const
            requires is_single
        {
            volatile const std::uint64_t* ptr = &storage_[0];
            std::uint64_t val = *ptr;
            return decrypt(val);
        }

        OBFUSCXX_FORCEINLINE type get(const std::size_t i) const
            requires is_array
        {
            volatile const std::uint64_t* ptr = &storage_[i];
            std::uint64_t val = *ptr;
            return decrypt(val);
        }

        OBFUSCXX_FORCEINLINE void copy_to(type* out, const std::size_t count) const
            requires is_array
        {
            std::size_t effective_count = (count < type_size) ? count : type_size;
            decrypt_vectorized(storage_, out, effective_count);
        }

        OBFUSCXX_RUNTIME_WARNING OBFUSCXX_FORCEINLINE void set(type val)
            requires is_single
        {
            storage_[0] = encrypt(val);
        }

        OBFUSCXX_RUNTIME_WARNING OBFUSCXX_FORCEINLINE void set(type val, const std::size_t i)
            requires is_array
        {
            storage_[i] = encrypt(val);
        }

        OBFUSCXX_RUNTIME_WARNING OBFUSCXX_FORCEINLINE void set(const std::initializer_list<type>& list)
            requires is_array
        {
            for (std::size_t i{}; const auto& val : list) {
                if (i < type_size) {
                    storage_[i++] = encrypt(val);
                }
            }
        }

        OBFUSCXX_FORCEINLINE type operator()() const
            requires is_single
        {
            return get();
        }

        OBFUSCXX_FORCEINLINE type operator[](const std::size_t i) const
            requires is_array
        {
            return get(i);
        }

        OBFUSCXX_FORCEINLINE obfuscxx& operator=(type val)
            requires is_single
        {
            set(val);
            return *this;
        }

        OBFUSCXX_FORCEINLINE obfuscxx& operator=(const std::initializer_list<type>& list)
            requires is_array
        {
            set(list);
            return *this;
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE bool operator==(const rebind<E>& rhs) const
            requires is_single
        {
            return get() == rhs.get();
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE bool operator==(const rebind<E>& rhs) const
            requires is_array
        {
            for (std::size_t i{}; i < type_size; ++i) {
                if (get(i) != rhs.get(i)) {
                    return false;
                }
            }
            return true;
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE bool operator!=(const rebind<E>& rhs) const {
            return !(*this == rhs);
        }

        OBFUSCXX_FORCEINLINE operator type() const
            requires is_single
        {
            return get();
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE bool operator<(const rebind<E>& rhs) const {
            return get() < rhs.get();
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE bool operator>(const rebind<E>& rhs) const {
            return get() > rhs.get();
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE bool operator<=(const rebind<E>& rhs) const {
            return get() <= rhs.get();
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE bool operator>=(const rebind<E>& rhs) const {
            return get() >= rhs.get();
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE type operator+(const rebind<E>& rhs) const {
            return get() + rhs.get();
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE type operator-(const rebind<E>& rhs) const {
            return get() - rhs.get();
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE type operator*(const rebind<E>& rhs) const
            requires(!is_single_pointer)
        {
            return get() * rhs.get();
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE type operator/(const rebind<E>& rhs) const
            requires(!is_single_pointer)
        {
            return get() / rhs.get();
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE obfuscxx& operator+=(const rebind<E>& rhs)
            requires(!is_single_pointer)
        {
            set(get() + rhs.get());
            return *this;
        }

        template<std::uint64_t E> OBFUSCXX_FORCEINLINE obfuscxx& operator-=(const rebind<E>& rhs)
            requires(!is_single_pointer)
        {
            set(get() - rhs.get());
            return *this;
        }

        OBFUSCXX_FORCEINLINE type operator->()
            requires is_single_pointer
        {
            return get();
        }

        OBFUSCXX_FORCEINLINE type& operator*()
            requires is_single_pointer
        {
            return *get();
        }

        struct iterator {
            const obfuscxx* parent;
            std::size_t index;

            type operator*() const {
                return parent->get(index);
            }

            iterator& operator++() {
                ++index;
                return *this;
            }

            bool operator!=(const iterator& other) const {
                return index != other.index;
            }
            bool operator==(const iterator& other) const {
                return index == other.index;
            }
        };

        iterator begin() const
            requires is_array
        {
            return {this, 0};
        }
        iterator end() const
            requires is_array
        {
            return {this, type_size};
        }
        static constexpr std::size_t size() {
            return type_size;
        }

        template<class CharT, std::size_t N> struct string_copy {
        private:
            static constexpr bool is_char = std::is_same_v<CharT, char> || std::is_same_v<CharT, const char>;
            static constexpr bool is_wchar = std::is_same_v<CharT, wchar_t> || std::is_same_v<CharT, const wchar_t>;

        public:
            operator const char*() const
                requires is_char
            {
                return data;
            }
            operator const wchar_t*() const
                requires is_wchar
            {
                return data;
            }

            const CharT* c_str() const {
                return data;
            }
            const CharT& operator[](std::size_t i) const {
                return data[i];
            }

            const CharT* begin() const {
                return data;
            }
            const CharT* end() const {
                return data + N;
            }

            constexpr std::size_t size() const {
                return N;
            }

            CharT data[N];
        };

        template<class ArrayT, std::size_t N> struct array_copy {
            const ArrayT* get() const {
                return data;
            }

            const ArrayT* begin() const {
                return data;
            }
            const ArrayT* end() const {
                return data + N;
            }

            constexpr std::size_t size() const {
                return N;
            }
            constexpr std::size_t size_bytes() const {
                return N * sizeof(ArrayT);
            }

            ArrayT data[N];
        };

        OBFUSCXX_FORCEINLINE string_copy<type, type_size> to_string() const
            requires(is_char || is_wchar)
        {
            string_copy<type, type_size> result{};
            if constexpr (is_array) {
                copy_to(result.data, type_size);
            } else {
                result.data[0] = get();
            }
            return result;
        }

        OBFUSCXX_FORCEINLINE array_copy<type, type_size> to_array() const
            requires(is_array)
        {
            array_copy<type, type_size> result{};
            copy_to(result.data, type_size);
            return result;
        }

    private:
        alignas(storage_alignment) OBFUSCXX_VOLATILE std::uint64_t storage_[storage_size]{};
    };
} // namespace ngu

#define OBFXX_EXPAND(x) x
#define OBFXX_GET(_1, _2, NAME, ...) NAME

#define OBFXX_T1(type) ngu::obfuscxx<type, ngu::obf_level::low, OBFUSCXX_ENTROPY>
#define OBFXX_T2(type, level) ngu::obfuscxx<type, level, OBFUSCXX_ENTROPY>

#define OBFXX_O1(obj) (ngu::obfuscxx<std::remove_cvref_t<decltype(obj)>, ngu::obf_level::low, OBFUSCXX_ENTROPY>(obj))
#define OBFXX_O2(obj, level) (ngu::obfuscxx<std::remove_cvref_t<decltype(obj)>, level, OBFUSCXX_ENTROPY>(obj))

#define OBFXX_V1(val) (OBFXX_O1(val).get())
#define OBFXX_V2(val, level) (OBFXX_O2(val, level).get())

#define OBFXX_S1(str) (OBFXX_O1(str).to_string().c_str())
#define OBFXX_S2(str, level) (OBFXX_O2(str, level).to_string().c_str())

/// Obfuscate type (low by default; pass ngu::obf_level::{medium,high} as second arg)
#define obfxx(...) OBFXX_EXPAND(OBFXX_GET(__VA_ARGS__, OBFXX_T2, OBFXX_T1)(__VA_ARGS__))

/// Obfuscate value (low by default; pass ngu::obf_level::{medium,high} as second arg)
#define obfxxv(...) OBFXX_EXPAND(OBFXX_GET(__VA_ARGS__, OBFXX_V2, OBFXX_V1)(__VA_ARGS__))

/// Obfuscate string (low by default; pass ngu::obf_level::{medium,high} as second arg)
#define obfxxs(...) OBFXX_EXPAND(OBFXX_GET(__VA_ARGS__, OBFXX_S2, OBFXX_S1)(__VA_ARGS__))

/// Obfuscate objects (low by default; pass ngu::obf_level::{medium,high} as second arg)
#define obfxxo(...) OBFXX_EXPAND(OBFXX_GET(__VA_ARGS__, OBFXX_O2, OBFXX_O1)(__VA_ARGS__))

#if defined(__clang__) || defined(__GNUC__)
#define OBFXX_LITERAL(str, type, level) ngu::obfuscxx<type, level, OBFUSCXX_ENTROPY>(str).to_string()

/// Obfuscate string literal (low level)
template<typename CharT, CharT... Chars> constexpr auto operator""_obf() {
    constexpr CharT str[] = {Chars..., '\0'};
    return OBFXX_LITERAL(str, std::remove_cvref_t<decltype(str)>, ngu::obf_level::low);
}
/// Obfuscate string literal (medium level)
template<typename CharT, CharT... Chars> constexpr auto operator""_obfm() {
    constexpr CharT str[] = {Chars..., '\0'};
    return OBFXX_LITERAL(str, std::remove_cvref_t<decltype(str)>, ngu::obf_level::medium);
}
/// Obfuscate string literal (high level)
template<typename CharT, CharT... Chars> constexpr auto operator""_obfh() {
    constexpr CharT str[] = {Chars..., '\0'};
    return OBFXX_LITERAL(str, std::remove_cvref_t<decltype(str)>, ngu::obf_level::high);
}
#endif
#endif // NGU_OBFUSCXX_H