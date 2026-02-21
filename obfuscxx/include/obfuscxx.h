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
#elif defined(_MSC_VER) || defined(__clang__)
#include <intrin.h>
#elif defined(__GNUC__)
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
#define OBFUSCXX_MEM_BARRIER(...) __asm__ volatile("" : "+r"(__VA_ARGS__) :: "memory");
#if defined(__aarch64__) || defined(_M_ARM64)
#define OBFUSCXX_MEM_BARRIER_VEC(v0, v1, sum) __asm__ volatile("" : "+w"(v0), "+w"(v1), "+r"(sum) :: "memory");
#else
#define OBFUSCXX_MEM_BARRIER_VEC(v0, v1, sum) __asm__ volatile("" : "+x"(v0), "+x"(v1), "+r"(sum) :: "memory");
#endif
#elif defined(_MSC_VER)
#define OBFUSCXX_MEM_BARRIER(...) _ReadWriteBarrier();
#define OBFUSCXX_MEM_BARRIER_VEC(v0, v1, sum) _ReadWriteBarrier();
#endif

#ifndef OBFUSCXX_DISABLE_WARNS
#define OBFUSCXX_RUNTIME_WARNING [[deprecated("OBFUSCXX: Runtime set() uses encrypt method without SIMD obfuscation. For better protection, initialize at compile-time.")]]
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

        OBFUSCXX_FORCEINLINE std::uint64_t hash_runtime(char const *str) {
            std::size_t length = 0;
            while (str[length])
                ++length;

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

        constexpr std::uint64_t rol64(std::uint64_t x, int n) {
            n &= 63;
            if (n == 0) return x;
            return (x << n) | (x >> (64 - n));
        }

        constexpr std::uint64_t ror64(std::uint64_t x, int n) {
            n &= 63;
            if (n == 0) return x;
            return (x >> n) | (x << (64 - n));
        }
    }

    template<typename Vec> struct simd;

#if defined(__aarch64__) || defined(_M_ARM64)
    template<> struct simd<uint32x4_t> {
        using vec = uint32x4_t;
        static OBFUSCXX_FORCEINLINE vec shl4(vec v)              { return vshlq_n_u32(v, 4); }
        static OBFUSCXX_FORCEINLINE vec shr5(vec v)              { return vshrq_n_u32(v, 5); }
        static OBFUSCXX_FORCEINLINE vec vxor(vec a, vec b)       { return veorq_u32(a, b); }
        static OBFUSCXX_FORCEINLINE vec vadd(vec a, vec b)       { return vaddq_u32(a, b); }
        static OBFUSCXX_FORCEINLINE vec vsub(vec a, vec b)       { return vsubq_u32(a, b); }
        static OBFUSCXX_FORCEINLINE vec broadcast(std::uint32_t v) { return vdupq_n_u32(v); }
        static OBFUSCXX_FORCEINLINE vec from_scalar(std::uint32_t v) { return vdupq_n_u32(v); }
        static OBFUSCXX_FORCEINLINE std::uint32_t to_scalar(vec v)   { return vgetq_lane_u32(v, 0); }
    };
#else
    template<> struct simd<__m128i> {
        using vec = __m128i;
        static OBFUSCXX_FORCEINLINE vec shl4(vec v)              { return _mm_slli_epi32(v, 4); }
        static OBFUSCXX_FORCEINLINE vec shr5(vec v)              { return _mm_srli_epi32(v, 5); }
        static OBFUSCXX_FORCEINLINE vec vxor(vec a, vec b)       { return _mm_xor_si128(a, b); }
        static OBFUSCXX_FORCEINLINE vec vadd(vec a, vec b)       { return _mm_add_epi32(a, b); }
        static OBFUSCXX_FORCEINLINE vec vsub(vec a, vec b)       { return _mm_sub_epi32(a, b); }
        static OBFUSCXX_FORCEINLINE vec broadcast(std::uint32_t v) { return _mm_set1_epi32(v); }
        static OBFUSCXX_FORCEINLINE vec from_scalar(std::uint32_t v) { return _mm_cvtsi32_si128(v); }
        static OBFUSCXX_FORCEINLINE std::uint32_t to_scalar(vec v)   { return _mm_cvtsi128_si32(v); }
    };
#if OBFUSCXX_HAS_AVX2
    template<> struct simd<__m256i> {
        using vec = __m256i;
        static OBFUSCXX_FORCEINLINE vec shl4(vec v)              { return _mm256_slli_epi32(v, 4); }
        static OBFUSCXX_FORCEINLINE vec shr5(vec v)              { return _mm256_srli_epi32(v, 5); }
        static OBFUSCXX_FORCEINLINE vec vxor(vec a, vec b)       { return _mm256_xor_si256(a, b); }
        static OBFUSCXX_FORCEINLINE vec vadd(vec a, vec b)       { return _mm256_add_epi32(a, b); }
        static OBFUSCXX_FORCEINLINE vec vsub(vec a, vec b)       { return _mm256_sub_epi32(a, b); }
        static OBFUSCXX_FORCEINLINE vec broadcast(std::uint32_t v) { return _mm256_set1_epi32(v); }
        static OBFUSCXX_FORCEINLINE vec from_scalar(std::uint32_t v) { return _mm256_set1_epi32(v); }
        static OBFUSCXX_FORCEINLINE std::uint32_t to_scalar(vec v)   { return _mm256_extract_epi32(v, 0); }
    };
#endif
#endif

#define OBFUSCXX_HASH( s ) detail::hash_compile_time( s )
#define OBFUSCXX_HASH_RT( s ) detail::hash_runtime( s )

#if defined(_KERNEL_MODE) || defined(_WIN64_DRIVER)
#define OBFUSCXX_ENTROPY ( \
detail::splitmix64( \
(OBFUSCXX_HASH(__FILE__) * 0x517cc1b727220a95ULL) + \
((std::uint64_t)__LINE__ * 0x9e3779b97f4a7c15ULL) + \
(detail::rol64((std::uint64_t)__COUNTER__, 37) ^ ((std::uint64_t)__LINE__ * 0xff51afd7ed558ccdULL)) \
) \
)
#else
#define OBFUSCXX_ENTROPY ( \
detail::splitmix64( \
OBFUSCXX_HASH(__FILE__) + \
((std::uint64_t)__LINE__ * 0x9e3779b97f4a7c15ULL) + \
(OBFUSCXX_HASH(__TIME__) ^ ((std::uint64_t)__COUNTER__ << 32)) \
) \
)
#endif

    enum class obf_level : std::uint8_t { Low, Medium, High };
    template<class Type, std::size_t Size = 1, obf_level Level = obf_level::Low, std::uint64_t Entropy = OBFUSCXX_ENTROPY>
    class obfuscxx {
        static constexpr bool is_single = Size == 1;
        static constexpr bool is_array = Size > 1;
        static constexpr bool is_single_pointer = std::is_pointer_v<Type> && Size == 1;
        static constexpr bool is_char = std::is_same_v<Type, char> || std::is_same_v<Type, const char>;
        static constexpr bool is_wchar = std::is_same_v<Type, wchar_t> || std::is_same_v<Type, const wchar_t>;

        static constexpr std::size_t storage_multiple = OBFUSCXX_HAS_AVX2 ? 8 : 4;
        static constexpr std::size_t storage_alignment = OBFUSCXX_HAS_AVX2 ? 32 : 16;
        static constexpr std::size_t storage_size = is_array ? (Size + storage_multiple - 1) & ~(storage_multiple - 1) : Size;

        struct passkey { explicit passkey() = default; };

        static constexpr std::uint64_t seed{ Entropy };
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

        static constexpr std::uint32_t xtea_rounds =
                (Level == obf_level::Low) ? 2 : (Level == obf_level::Medium) ? 6 : (6 + ((unique_index & 0x7) * 2));

        static constexpr std::uint32_t xtea_delta = (0x9E3779B9 ^ static_cast<std::uint32_t>(unique_value)) | 1;

        static constexpr std::uint64_t encrypt(Type value) {
            std::uint64_t val = to_uint64(value);

            std::uint32_t v0 = static_cast<std::uint32_t>(val);
            std::uint32_t v1 = static_cast<std::uint32_t>(val >> 32);
            std::uint32_t sum = 0;

            for (std::uint32_t i{}; i < xtea_rounds; ++i) {
                v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^
                        (sum + static_cast<std::uint32_t>(iv[sum & 3]));
                sum += xtea_delta;
                v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^
                        (sum + static_cast<std::uint32_t>(iv[(sum >> 11) & 3]));
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

        template<typename Vec> static OBFUSCXX_FORCEINLINE Type decrypt_scalar(std::uint32_t v0, std::uint32_t v1) {
            using S = simd<Vec>;
            std::uint32_t sum = xtea_delta * xtea_rounds;

            for (std::uint32_t i{}; i < xtea_rounds; ++i) {
                OBFUSCXX_MEM_BARRIER(v0, v1, sum)
                auto sv0 = S::from_scalar(v0);
                auto sv1 = S::from_scalar(v1);
                sv1 = xtea_half_round(sv0, sv1, sum + static_cast<std::uint32_t>(iv[(sum >> 11) & 3]));
                v1 = S::to_scalar(sv1);

                sum -= xtea_delta;

                OBFUSCXX_MEM_BARRIER(v0, v1, sum)
                sv1 = S::from_scalar(v1);
                sv0 = S::from_scalar(v0);
                sv0 = xtea_half_round(sv1, sv0, sum + static_cast<std::uint32_t>(iv[sum & 3]));
                v0 = S::to_scalar(sv0);
            }

            return from_uint64((static_cast<std::uint64_t>(v1) << 32) | v0);
        }

        template<typename Vec> static OBFUSCXX_FORCEINLINE void decrypt_rounds(Vec &v0_vec, Vec &v1_vec) {
            std::uint32_t sum = xtea_delta * xtea_rounds;

            for (std::uint32_t i{}; i < xtea_rounds; ++i) {
                OBFUSCXX_MEM_BARRIER_VEC(v0_vec, v1_vec, sum)
                v1_vec = xtea_half_round(v0_vec, v1_vec, sum + static_cast<std::uint32_t>(iv[(sum >> 11) & 3]));
                sum -= xtea_delta;
                OBFUSCXX_MEM_BARRIER_VEC(v0_vec, v1_vec, sum)
                v0_vec = xtea_half_round(v1_vec, v0_vec, sum + static_cast<std::uint32_t>(iv[sum & 3]));
            }
        }

        static OBFUSCXX_FORCEINLINE Type decrypt(std::uint64_t value) {
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

        static OBFUSCXX_FORCEINLINE void decrypt_vectorized(const volatile std::uint64_t *src, Type *dst, std::size_t count) {
            std::size_t aligned_count = (count + storage_multiple - 1) & ~(storage_multiple - 1);

#if defined(__aarch64__) || defined(_M_ARM64)
            for (std::size_t i{}; i < aligned_count; i += 4) {
                uint32x4x2_t loaded = vld2q_u32(reinterpret_cast<const std::uint32_t *>(const_cast<const std::uint64_t *>(src + i)));
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
                    const auto *p = reinterpret_cast<const __m256i*>(const_cast<const std::uint64_t *>(src + i));

                    __m256i d0 = _mm256_load_si256(p);
                    __m256i d1 = _mm256_load_si256(p + 1);
                    __m256i perm_even = _mm256_set_epi32(0, 0, 0, 0, 6, 4, 2, 0);
                    __m256i perm_odd  = _mm256_set_epi32(0, 0, 0, 0, 7, 5, 3, 1);
                    __m256i v0_vec = _mm256_permute2x128_si256(
                        _mm256_permutevar8x32_epi32(d0, perm_even),
                        _mm256_permutevar8x32_epi32(d1, perm_even), 0x20);
                    __m256i v1_vec = _mm256_permute2x128_si256(
                        _mm256_permutevar8x32_epi32(d0, perm_odd),
                        _mm256_permutevar8x32_epi32(d1, perm_odd), 0x20);

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
                    const auto *p = reinterpret_cast<const __m128i *>(const_cast<const std::uint64_t *>(src + i));

                    __m128i d0 = _mm_load_si128(p);
                    __m128i d1 = _mm_load_si128(p + 1);
                    __m128i mask = _mm_set1_epi64x(0xFFFFFFFF);
                    __m128i v0_vec = _mm_castps_si128(_mm_shuffle_ps(
                        _mm_castsi128_ps(_mm_and_si128(d0, mask)),
                        _mm_castsi128_ps(_mm_and_si128(d1, mask)),
                        _MM_SHUFFLE(2, 0, 2, 0)));
                    __m128i v1_vec = _mm_castps_si128(_mm_shuffle_ps(
                        _mm_castsi128_ps(_mm_srli_epi64(d0, 32)),
                        _mm_castsi128_ps(_mm_srli_epi64(d1, 32)),
                        _MM_SHUFFLE(2, 0, 2, 0)));

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
                const auto *p = reinterpret_cast<const __m128i *>(const_cast<const std::uint64_t *>(src + i));

                __m128i d0 = _mm_load_si128(p);
                __m128i d1 = _mm_load_si128(p + 1);
                __m128i mask = _mm_set1_epi64x(0xFFFFFFFF);
                __m128i v0_vec = _mm_castps_si128(_mm_shuffle_ps(
                    _mm_castsi128_ps(_mm_and_si128(d0, mask)),
                    _mm_castsi128_ps(_mm_and_si128(d1, mask)),
                    _MM_SHUFFLE(2, 0, 2, 0)));
                __m128i v1_vec = _mm_castps_si128(_mm_shuffle_ps(
                    _mm_castsi128_ps(_mm_srli_epi64(d0, 32)),
                    _mm_castsi128_ps(_mm_srli_epi64(d1, 32)),
                    _MM_SHUFFLE(2, 0, 2, 0)));

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
#endif
        }

        static constexpr std::uint64_t to_uint64(Type value) {
            if constexpr (std::is_pointer_v<Type>) {
                return reinterpret_cast<std::uint64_t>(value);
            } else if constexpr (std::is_floating_point_v<Type>) {
                if constexpr (sizeof(Type) == 4) {
                    return __builtin_bit_cast(std::uint32_t, value);
                } else {
                    return __builtin_bit_cast(std::uint64_t, value);
                }
            } else {
                return static_cast<std::uint64_t>(value);
            }
        }

        static OBFUSCXX_FORCEINLINE Type from_uint64(std::uint64_t value) {
            if constexpr (std::is_pointer_v<Type>) {
                return reinterpret_cast<Type>(value);
            } else if constexpr (std::is_floating_point_v<Type>) {
                if constexpr (sizeof(Type) == 4) {
                    return __builtin_bit_cast(Type, static_cast<std::uint32_t>(value));
                } else {
                    return __builtin_bit_cast(Type, value);
                }
            } else {
                return static_cast<Type>(value);
            }
        }

        static OBFUSCXX_FORCEINLINE bool cpu_has_avx2() {
#if !defined(__aarch64__) && !defined(_M_ARM64)
            static const bool cached = []() {
#if defined(_MSC_VER)
                int cpuInfo[4]{};
                __cpuidex(cpuInfo, 7, 0);
                return (cpuInfo[1] & (1 << 5)) != 0;
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
            for (std::size_t i{}; i < Size; ++i) {
                storage_[i] = seed ^ iv[i & iv_size];
            }
        }

        explicit consteval obfuscxx(Type val) : obfuscxx(passkey{}) {
            storage_[0] = encrypt(val);
        }

        explicit consteval obfuscxx(Type (&arr)[Size]) : obfuscxx(passkey{}) {
            for (std::size_t i{}; i < Size; ++i) {
                storage_[i] = encrypt(arr[i]);
            }
        }

        explicit consteval obfuscxx(const Type (&arr)[Size]) : obfuscxx(passkey{}) {
            for (std::size_t i{}; i < Size; ++i) {
                storage_[i] = encrypt(arr[i]);
            }
        }

        consteval obfuscxx(const std::initializer_list<Type> &list) : obfuscxx(passkey{}) {
            for (std::size_t i{}; const auto &v: list)
                storage_[i++] = encrypt(v);
        }

        OBFUSCXX_FORCEINLINE Type get() const requires is_single {
            volatile const std::uint64_t *ptr = &storage_[0];
            std::uint64_t val = *ptr;
            return decrypt(val);
        }

        OBFUSCXX_FORCEINLINE Type get(std::size_t i) const requires is_array {
            volatile const std::uint64_t *ptr = &storage_[i];
            std::uint64_t val = *ptr;
            return decrypt(val);
        }

        OBFUSCXX_FORCEINLINE void copy_to(Type *out, std::size_t count) const requires is_array {
            std::size_t effective_count = (count < Size) ? count : Size;
            decrypt_vectorized(storage_, out, effective_count);
        }

        OBFUSCXX_RUNTIME_WARNING OBFUSCXX_FORCEINLINE void set(Type val) requires is_single {
            storage_[0] = encrypt(val);
        }

        OBFUSCXX_RUNTIME_WARNING OBFUSCXX_FORCEINLINE void set(Type val, std::size_t i) requires is_array {
            storage_[i] = encrypt(val);
        }

        OBFUSCXX_RUNTIME_WARNING OBFUSCXX_FORCEINLINE void set(const std::initializer_list<Type> &list) requires is_array {
            for (std::size_t i{}; const auto &val: list) {
                if (i < Size) {
                    storage_[i++] = encrypt(val);
                }
            }
        }

        OBFUSCXX_FORCEINLINE Type operator()() const requires is_single {
            return get();
        }

        OBFUSCXX_FORCEINLINE Type operator[](std::size_t i) const requires is_array {
            return get(i);
        }

        OBFUSCXX_FORCEINLINE obfuscxx &operator=(Type val) requires is_single {
            set(val);
            return *this;
        }

        OBFUSCXX_FORCEINLINE obfuscxx &operator=(const std::initializer_list<Type> &list) requires is_array {
            set(list);
            return *this;
        }

        OBFUSCXX_FORCEINLINE bool operator==(const obfuscxx &rhs) const requires is_single {
            return get() == rhs.get();
        }

        OBFUSCXX_FORCEINLINE bool operator==(const obfuscxx &rhs) const requires is_array {
            for (std::size_t i{}; i < Size; ++i) {
                if (get(i) != rhs.get(i)) {
                    return false;
                }
            }
            return true;
        }

        OBFUSCXX_FORCEINLINE bool operator!=(const obfuscxx &rhs) const {
            return !(*this == rhs);
        }

        OBFUSCXX_FORCEINLINE operator Type() const requires is_single {
            return get();
        }

        OBFUSCXX_FORCEINLINE bool operator<(const obfuscxx &rhs) const {
            return get() < rhs.get();
        }

        OBFUSCXX_FORCEINLINE bool operator>(const obfuscxx &rhs) const {
            return get() > rhs.get();
        }

        OBFUSCXX_FORCEINLINE bool operator<=(const obfuscxx &rhs) const {
            return get() <= rhs.get();
        }

        OBFUSCXX_FORCEINLINE bool operator>=(const obfuscxx &rhs) const {
            return get() >= rhs.get();
        }

        OBFUSCXX_FORCEINLINE Type operator+(const obfuscxx &rhs) const {
            return get() + rhs.get();
        }

        OBFUSCXX_FORCEINLINE Type operator-(const obfuscxx &rhs) const {
            return get() - rhs.get();
        }

        OBFUSCXX_FORCEINLINE Type operator*(const obfuscxx &rhs) const requires (!is_single_pointer) {
            return get() * rhs.get();
        }

        OBFUSCXX_FORCEINLINE Type operator/(const obfuscxx &rhs) const requires (!is_single_pointer) {
            return get() / rhs.get();
        }

        OBFUSCXX_FORCEINLINE obfuscxx &operator+=(const obfuscxx &rhs) requires (!is_single_pointer) {
            set(get() + rhs.get());
            return *this;
        }

        OBFUSCXX_FORCEINLINE obfuscxx &operator-=(const obfuscxx &rhs) requires (!is_single_pointer) {
            set(get() - rhs.get());
            return *this;
        }

        OBFUSCXX_FORCEINLINE Type operator->() requires is_single_pointer {
            return get();
        }

        OBFUSCXX_FORCEINLINE Type &operator*() requires is_single_pointer {
            return *get();
        }

        struct iterator {
            const obfuscxx *parent;
            std::size_t index;

            Type operator*() const { return parent->get(index); }

            iterator &operator++() {
                ++index;
                return *this;
            }

            bool operator!=(const iterator &other) const { return index != other.index; }
            bool operator==(const iterator &other) const { return index == other.index; }
        };

        iterator begin() const requires is_array { return {this, 0}; }
        iterator end() const requires is_array { return {this, Size}; }
        static constexpr std::size_t size() { return Size; }

        template<class CharType, std::size_t N> struct string_copy {
        private:
            static constexpr bool is_char = std::is_same_v<CharType, char> ||
                                            std::is_same_v<CharType, const char>;
            static constexpr bool is_wchar = std::is_same_v<CharType, wchar_t> ||
                                             std::is_same_v<CharType, const wchar_t>;

        public:
            operator const char *() const requires is_char { return data; }
            operator const wchar_t *() const requires is_wchar { return data; }

            const CharType *c_str() const { return data; }
            const CharType &operator[](std::size_t i) const { return data[i]; }

            const CharType *begin() const { return data; }
            const CharType *end() const { return data + N; }

            constexpr std::size_t size() const { return N; }

            CharType data[N];
        };

        template<class ArrayType, std::size_t N> struct array_copy {
            const ArrayType *get() const { return data; }

            const ArrayType *begin() const { return data; }
            const ArrayType *end() const { return data + N; }

            constexpr std::size_t size() const { return N; }
            constexpr std::size_t size_bytes() const { return N * sizeof(ArrayType); }

            ArrayType data[N];
        };

        OBFUSCXX_FORCEINLINE string_copy<Type, Size> to_string() const requires (is_char || is_wchar) {
            string_copy<Type, Size> result{};
            if constexpr (is_array) {
                copy_to(result.data, Size);
            } else {
                result.data[0] = get();
            }
            return result;
        }

        OBFUSCXX_FORCEINLINE array_copy<Type, Size> to_array() const requires (is_array) {
            array_copy<Type, Size> result{};
            copy_to(result.data, Size);
            return result;
        }

    private:
        alignas(storage_alignment) OBFUSCXX_VOLATILE std::uint64_t storage_[storage_size]{};
    };
}

#if defined(__clang__) || defined(__GNUC__)
template<typename CharType, CharType... chars> constexpr auto operator""_obf() {
    constexpr CharType str[] = {chars..., '\0'};
    return ngu::obfuscxx(str).to_string();
}
#endif

#define obfusv(val) ngu::obfuscxx(val).get()
#define obfuss(str) ngu::obfuscxx(str).to_string().c_str()

#endif // NGU_OBFUSCXX_H