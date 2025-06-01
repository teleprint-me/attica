# DSA

**Data Structures and Algorithms in Pure C**

A collection of portable C libraries for foundational algorithms and data
structures, designed for real-world systems programming and experimentation.

## 🚀 Quick Start

### Clone

```sh
git clone https://github.com/teleprint-me/dsa.c dsa
cd dsa
```

### Build

```sh
# Use Release for optimized builds
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug -j $(nproc)
```

### Test

```sh
ctest --rerun-failed --output-on-failure --test-dir build
```

## 🪄 Projects

### ⚠️ Status

> **Work in progress.** Modules and APIs may change frequently. Pull requests
> and feedback are welcome!

_Note: All modules are evolving. See `tests/` for usage and examples._

### Add as submodule

```sh
git submodule add https://github.com/teleprint-me/dsa.c dsa
```

### Add include

```c
#include "allocator/arena.h"
#include "logger.h"
```

### Sync updates

```sh
git submodule update --remote dsa
```

## 📚 References

### Mathematical Background

- [Calculus (Apex)](https://leanpub.com/apexcalculus)
- [Discrete Mathematics](https://discrete.openmathbooks.org/dmoi3.html)
- [Statistics and Probability](https://stats.libretexts.org/Courses/Las_Positas_College/Math_40%3A_Statistics_and_Probability)
- [Linear Algebra](https://understandinglinearalgebra.org/home.html)
- [Digital Signal Processing](https://www.dspguide.com/)

### Programming References

- [C Standard](https://www.open-std.org/JTC1/SC22/WG14/)
- [C Reference](https://en.cppreference.com/w/c)
- [CMake Reference](https://cmake.org/cmake/help/latest/)
- [Bash Reference](https://www.gnu.org/software/bash/manual/html_node/index.html)

## 🪪 License

This project is licensed under the **AGPL License**. See the [LICENSE](LICENSE)
file for details.
