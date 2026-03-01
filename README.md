# obfuscxx

<p align="center">
  <img src="images/banner.png">
</p>

## Description
Header-only compile-time variables obfuscation library for C++20 and later.

## How it works
During compilation, data is encrypted via eXtended Tiny Encryption Algorithm (XTEA). Decryption uses SIMD instructions (AVX2/SSE2/NEON) at runtime, making static analysis considerably more complicated. Key entropy is based on the preprocessor macro `__COUNTER__`, the file name(`__FILE__`), and the line number (`__LINE__`) where the variable is defined, and the build time (`__TIME__`) (note: build time is not included when compiling with WDM).

By selecting different encryption levels (Low, Medium, High), you can control the number of encryption rounds. With Low, there are 2 rounds; Medium uses 6; and High adjusts the number of rounds dynamically based on the key entropy, ranging from 8 to 32. This lets you apply lighter encryption to frequently accessed data, and stronger encryption to data that’s used less often.

### Why XTEA
XTEA was chosen for several reasons. The primary one is its solid cryptographic strength combined with a minimal algorithm footprint (yes, “cryptographic strength” is somewhat overstated here - the keys and the algorithm itself are visible, the goal here is making analysis more difficult, not cryptographic security). Additionally, XTEA operates on 64-bit blocks, which maps perfectly to a single scalar - one block corresponds to one protected value.

### SIMD for obfuscation
When working with scalar data, SIMD instructions are used not for vectorization, but to complicate static analysis, data extraction, and emulation-based deobfuscation. For vector data (arrays, strings), SIMD is used to its full potential, enabling parallel decryption without compromising protection quality.

### SIMD platform support
The library supports multiple SIMD instruction sets depending on the target architecture and compiler flags. On MSVC, SSE2 is used as a fallback by default, this ensures compatibility with both older and newer processors. If support for older processors is not required, define the OBFUSCXX_MSVC_FORCE_AVX2 macro.

## Decompilation view
The screenshots show only a small portion of the int main() function. In reality, the function can grow to around 250 lines depending on the compiler.

<table align="center">
<tr>
<td><img src="images/msvc.png" width="400"/></td>
<td><img src="images/llvm.png" width="400"/></td>
<td><img src="images/gcc.png" width="400"/></td>
</tr>
</table>
<p align="center"><em>MSVC, LLVM, GCC compilation (int main(), Level: Low, Arch: x86-64, SIMD Insn: AVX2, SSE2)</em></p>

## Installation
Just add the `include/` directory to your include path and use `#include <obfuscxx/obfuscxx.h>`

> To disable the SSE2 fallback on MSVC, define `OBFUSCXX_MSVC_FORCE_AVX2` before including the header or in your CMakeLists.txt.

## Examples
### Basic
```cpp
#include <obfuscxx/obfuscxx.h>

int main() {
    obfuscxx<int> int_value{ 100 };
    std::cout << int_value.get() << '\n';
    int_value = 50;
    std::cout << int_value.get() << '\n';

    obfuscxx<float> float_value{ 1.5f };
    std::cout << float_value.get() << '\n';

    obfuscxx<int, 4> array{ 1, 2, 3, 4 };
    for (auto val : array) {
        std::cout << val << " ";
    }
    std::cout << '\n';

    obfuscxx str("Hello, World!");
    std::cout << str.to_string() << '\n';

    obfuscxx<int*> pointer{};
    pointer = new int{101};
    std::cout << pointer.get() << " " << *pointer.get() << '\n';
    delete pointer.get();
}
```

### User-Defined literal (Clang/GCC only)
```cpp
#include <obfuscxx/obfuscxx.h>

int main() {
    std::cout << "Hello, World!"_obf << '\n';
}
```

### Macros
```cpp
#include <obfuscxx/obfuscxx.h>

int main() {
    std::cout << obfusv(42) << " " << obfusv(3.14159f) << '\n';
    std::cout << obfuss("Hello, World!") << '\n';
}
```

## Benchmarks

> Benchmark results may vary depending on compiler flags and the toolchain used. The results below were obtained using the configuration available in [CMakeLists.txt](CMakeLists.txt). Note that High-level results may vary between builds, as the number of encryption rounds is dynamically determined based on key entropy.

### Runtime performance impact
| Operation | MSVC | LLVM | GCC |
|-----------|------|------|-----|
| **Integer Get (Low)** | 4.61 ns | **4.24 ns** | 4.99 ns |
| **Integer Get (Medium)** | 22.5 ns | **13.5 ns** | 15.6 ns |
| **Integer Get (High)** | 57.3 ns | **51.5 ns** | 62.4 ns |
| **Float Get (Low)** | 4.93 ns | **4.08 ns** | 4.35 ns |
| **Float Get (Medium)** | 22.8 ns | **12.9 ns** | 15.1 ns |
| **Float Get (High)** | 56.0 ns | **48.1 ns** | 60.9 ns |
| **String Get (Low)** | 12.0 ns | **7.62 ns** | 13.7 ns |
| **String Get (Medium)** | **24.5 ns** | 28.6 ns | 30.3 ns |
| **String Get (High)** | 68.7 ns | 102 ns | **62.9 ns** |
| **Wide String Get (Low)** | 12.7 ns | **7.57 ns** | 11.7 ns |
| **Wide String Get (Medium)** | **28.3 ns** | 30.3 ns | 31.6 ns |
| **Wide String Get (High)** | 70.2 ns | 101 ns | **63.7 ns** |
| **Array Iteration (100 elem, Low)** | 483 ns | 445 ns | **443 ns** |
| **Array Iteration (100 elem, Medium)** | 2,348 ns | **1,342 ns** | 1,607 ns |
| **Array Iteration (100 elem, High)** | 5,553 ns | **4,640 ns** | 5,887 ns |
| **Array CopyTo (100 elem, Low)** | 91.9 ns | **66.5 ns** | 89.8 ns |
| **Array CopyTo (100 elem, Medium)** | 201 ns | **185 ns** | 225 ns |
| **Array CopyTo (100 elem, High)** | 477 ns | 642 ns | **417 ns** |
| **Array Element Access (Low)** | 4.99 ns | **4.23 ns** | 4.78 ns |
| **Array Element Access (Medium)** | 23.4 ns | **13.5 ns** | 15.8 ns |
| **Array Element Access (High)** | 56.8 ns | **51.8 ns** | 60.4 ns |

### Test environment
- CPU: 16 cores @ 2496 MHz
- L1 Data Cache: 48 KiB (x8)
- L1 Instruction Cache: 32 KiB (x8)
- L2 Unified Cache: 512 KiB (x8)
- L3 Unified Cache: 16384 KiB (x1)
- Date: 2026-02-22

## Building tests and benchmarks
1. Install `vcpkg` and set `VCPKG_ROOT` environment variable
2. Fetch baseline: `cd $VCPKG_ROOT && git fetch origin 34823ada10080ddca99b60e85f80f55e18a44eea`
3. Configure: `cmake --preset <compiler>` (MSVC/Clang/GCC)
4. Build: `cmake --build --preset <compiler>` (--config Release/Debug)

## Requirements
- C++20 or later
- Compiler with SIMD support (SSE2/AVX2/NEON)
- CMake 3.15+ (for building tests)
- vcpkg (for dependencies)

## Platform Support
> Although the library supports multiple architectures and compilers, there are some caveats. NEON instructions on ARM64 are aggressively optimized by the compiler regardless of attempts to prevent it. While constants remain encrypted, the decryption process itself may be simplified by the optimizer into plain scalar operations, reducing the effectiveness of the obfuscation. Due to limited testing on ARM64, the quality of obfuscation on this architecture has not been fully validated. If you are using the library on a NEON-based platform, it is recommended to manually inspect the generated code.

### Compilers
- MSVC (+ WDM)
- GCC
- Clang

### Architectures
- x86 (SSE2/AVX2)
- x86-64 (SSE2/AVX2)
- ARM64 (NEON)

### Operating Systems
- Windows
- Linux
- macOS

## License
**obfuscxx** is distributed under the [Apache License 2.0](LICENSE).