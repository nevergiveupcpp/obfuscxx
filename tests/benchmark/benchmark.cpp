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

#ifndef NGU_OBFUSCXX_METRICS_BENCHMARK_H
#define NGU_OBFUSCXX_METRICS_BENCHMARK_H

#include <benchmark/benchmark.h>
#include <obfuscxx/obfuscxx.h>

using namespace ngu;

static void BM_IntegerGet_Low(benchmark::State& state) {
    obfxx(int) const value{50};
    for (auto _ : state) {
        benchmark::DoNotOptimize(value.get());
    }
}
BENCHMARK(BM_IntegerGet_Low);

static void BM_IntegerGet_Medium(benchmark::State& state) {
    obfxx(int, obf_level::medium) const value{50};
    for (auto _ : state) {
        benchmark::DoNotOptimize(value.get());
    }
}
BENCHMARK(BM_IntegerGet_Medium);

static void BM_IntegerGet_High(benchmark::State& state) {
    obfxx(int, obf_level::high) const value{50};
    for (auto _ : state) {
        benchmark::DoNotOptimize(value.get());
    }
}
BENCHMARK(BM_IntegerGet_High);

static void BM_FloatGet_Low(benchmark::State& state) {
    obfxx(float) const value{42.5f};
    for (auto _ : state) {
        benchmark::DoNotOptimize(value.get());
    }
}
BENCHMARK(BM_FloatGet_Low);

static void BM_FloatGet_Medium(benchmark::State& state) {
    obfxx(float, obf_level::medium) const value{42.5f};
    for (auto _ : state) {
        benchmark::DoNotOptimize(value.get());
    }
}
BENCHMARK(BM_FloatGet_Medium);

static void BM_FloatGet_High(benchmark::State& state) {
    obfxx(float, obf_level::high) const value{42.5f};
    for (auto _ : state) {
        benchmark::DoNotOptimize(value.get());
    }
}
BENCHMARK(BM_FloatGet_High);

static void BM_StringGet_Low(benchmark::State& state) {
    auto constexpr str = obfxxo("benchmark");
    for (auto _ : state) {
        benchmark::DoNotOptimize(str.to_string().c_str());
    }
}
BENCHMARK(BM_StringGet_Low);

static void BM_StringGet_Medium(benchmark::State& state) {
    auto constexpr str = obfxxo("benchmark", obf_level::medium);
    for (auto _ : state) {
        benchmark::DoNotOptimize(str.to_string().c_str());
    }
}
BENCHMARK(BM_StringGet_Medium);

static void BM_StringGet_High(benchmark::State& state) {
    auto constexpr str = obfxxo("benchmark", obf_level::high);
    for (auto _ : state) {
        benchmark::DoNotOptimize(str.to_string().c_str());
    }
}
BENCHMARK(BM_StringGet_High);

static void BM_WStringGet_Low(benchmark::State& state) {
    auto constexpr str = obfxxo(L"benchmark");
    for (auto _ : state) {
        benchmark::DoNotOptimize(str.to_string().c_str());
    }
}
BENCHMARK(BM_WStringGet_Low);

static void BM_WStringGet_Medium(benchmark::State& state) {
    auto constexpr str = obfxxo(L"benchmark", obf_level::medium);
    for (auto _ : state) {
        benchmark::DoNotOptimize(str.to_string().c_str());
    }
}
BENCHMARK(BM_WStringGet_Medium);

static void BM_WStringGet_High(benchmark::State& state) {
    auto constexpr str = obfxxo(L"benchmark", obf_level::high);
    for (auto _ : state) {
        benchmark::DoNotOptimize(str.to_string().c_str());
    }
}
BENCHMARK(BM_WStringGet_High);

static void BM_ArrayIteration_Low(benchmark::State& state) {
    obfxx(int[100]) const array{};
    for (auto _ : state) {
        int sum = 0;
        for (auto const val : array) {
            sum += val;
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_ArrayIteration_Low);

static void BM_ArrayIteration_Medium(benchmark::State& state) {
    obfxx(int[100], obf_level::medium) const array{};
    for (auto _ : state) {
        int sum = 0;
        for (auto const val : array) {
            sum += val;
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_ArrayIteration_Medium);

static void BM_ArrayIteration_High(benchmark::State& state) {
    obfxx(int[100], obf_level::high) const array{};
    for (auto _ : state) {
        int sum = 0;
        for (auto const val : array) {
            sum += val;
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_ArrayIteration_High);

static void BM_ArrayCopyTo_Low(benchmark::State& state) {
    obfxx(int[100]) const array{};
    for (auto _ : state) {
        benchmark::DoNotOptimize(array.to_array());
    }
}
BENCHMARK(BM_ArrayCopyTo_Low);

static void BM_ArrayCopyTo_Medium(benchmark::State& state) {
    obfxx(int[100], obf_level::medium) const array{};
    for (auto _ : state) {
        benchmark::DoNotOptimize(array.to_array());
    }
}
BENCHMARK(BM_ArrayCopyTo_Medium);

static void BM_ArrayCopyTo_High(benchmark::State& state) {
    obfxx(int[100], obf_level::high) const array{};
    for (auto _ : state) {
        benchmark::DoNotOptimize(array.to_array());
    }
}
BENCHMARK(BM_ArrayCopyTo_High);

static void BM_ArrayGet_Low(benchmark::State& state) {
    obfxx(int[100]) const array{};
    for (auto _ : state) {
        benchmark::DoNotOptimize(array.get(50));
    }
}
BENCHMARK(BM_ArrayGet_Low);

static void BM_ArrayGet_Medium(benchmark::State& state) {
    obfxx(int[100], obf_level::medium) const array{};
    for (auto _ : state) {
        benchmark::DoNotOptimize(array.get(50));
    }
}
BENCHMARK(BM_ArrayGet_Medium);

static void BM_ArrayGet_High(benchmark::State& state) {
    obfxx(int[100], obf_level::high) const array{};
    for (auto _ : state) {
        benchmark::DoNotOptimize(array.get(50));
    }
}
BENCHMARK(BM_ArrayGet_High);

#endif // NGU_OBFUSCXX_METRICS_BENCHMARK_H