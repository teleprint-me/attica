# DSA

**Data Structures and Algorithms in Pure C**

A collection of portable C libraries for foundational algorithms and data
structures, designed for real-world systems programming and experimentation.

## ⚠️ Status

> **Work in progress.** Modules and APIs may change frequently. Pull requests
> and feedback are welcome!

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

## 📁 Modules

- `float_is_close` — Floating-point comparison utilities
- `hash_table` — Linear probing hash table
- `lease` — Simple memory ownership/lease API
- `lehmer` — Lehmer RNG
- `linked_list` — Singly-linked list
- `logger` — Thread-safe logger
- `memory` — Simple memory management utilities
- `node` — Generic node struct for tree/list use
- `prime` — Prime number helpers
- `queue`, `stack` — Classic queue/stack ADTs
- `unit_test` — Minimal C unit test framework
- `utf8` — UTF-8 byte/raw/string utilities

_Note: All modules are evolving. See `tests/` for usage and examples._

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
