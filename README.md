# Data Structures and Algorithms

**Data Structures and Algorithms in Pure C**

A collection of portable C libraries for foundational algorithms and data
structures, designed for real-world systems programming and experimentation.

## 🗃️ Dependencies

- `libc` (C standard library)
- `math` (math library, typically linked as `-lm`)
- `realtime` (real-time extensions, usually `-lrt`)
- `pthread` (POSIX threads, `-lpthread`)
- `pcre2-8` (Perl Compatible Regular Expressions, 8-bit)

> **Note:**
>
> - POSIX compliance required.
> - **Officially supported on Arch Linux.** 
>   _Other Linux distros may work, but are not guaranteed._
> - Windows users: Use WSL for full compatibility.
> - Mac OS X and BSD are _not_ supported at this time.

## 🚀 Quick Start

### 1. Clone

```sh
git clone https://github.com/teleprint-me/dsa.c.git dsa
cd dsa
```

### 2. Build

```sh
# Use Release for optimized builds
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug -j $(nproc)
```

### 3. Run Tests

```sh
ctest --rerun-failed --output-on-failure --test-dir build
```

### 4. Generate Documentation

```sh
cmake --build build --target run_doxy -j 16
```

## 🪄 Integration

### Add as a Git Submodule

```sh
git submodule add https://github.com/teleprint-me/dsa.c.git dsa
```

### Include in Your Project

```c
#include "core/memory.h"
#include "core/logger.h"
#include "allocator/arena.h"
```

### Update Submodule

```sh
git submodule update --remote dsa
```

## 🧪 Project Status

> **Work in progress.** APIs and modules change frequently.
> Feedback and pull requests are welcome!

_See `tests/` for up-to-date usage examples._

## 📚 References

### Mathematics

- [Apex Calculus](https://leanpub.com/apexcalculus)
- [Discrete Mathematics](https://discrete.openmathbooks.org/dmoi3.html)
- [Statistics & Probability](https://stats.libretexts.org/Courses/Las_Positas_College/Math_40%3A_Statistics_and_Probability)
- [Understanding Linear Algebra](https://understandinglinearalgebra.org/home.html)
- [DSP Guide](https://www.dspguide.com/)

### Programming

- [C Standard](https://www.open-std.org/JTC1/SC22/WG14/)
- [cppreference (C)](https://en.cppreference.com/w/c)
- [CMake Documentation](https://cmake.org/cmake/help/latest/)
- [GNU Bash Manual](https://www.gnu.org/software/bash/manual/html_node/index.html)
- [Doxygen Manual](https://www.doxygen.nl/manual/index.html)

## 🪪 License

This project is licensed under the **LGPL**. See [LICENSE](LICENSE) for details.
