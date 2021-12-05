# tabdeeli

An interactive search/replace tool for when you want to refactor without being nervous.

https://user-images.githubusercontent.com/101928/144780716-a6c0be23-78e6-4ab3-8788-209a9e336db1.mov

## Usage

```bash

$ tabdeeli --help
tabdeeli: interactive search/replace tool

SYNOPSIS
        ./tabdeeli [-s <search_regex>] [-r <replacement_string>] [-d <search_directory>]

OPTIONS
        -s, --search-regex
                    search regex string

        -r, --replacement-string
                    replacement text string

        -d, --search-directory
                    top level directory to launch search

```

## Dependencies

* Clang or GCC (with C++17 support)
* CMake (version with FetchContent support)
* Libraries (apt-get packages): `liblzma-dev`, `libpcre3-dev`

## Installation

### Linux

#### Usage the pre-built [Release](https://github.com/typon/tabdeeli/releases)

The release has been built using [exodus](https://github.com/intoli/exodus). It should be portable across Linux distros.

#### Build from source

Install the pre-requisite dependencies and then:

```bash
git clone https://github.com/typon/tabdeeli
cd tabdeeli
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
./tabdeeli
```

---

### Immense ❤️ for

[ftxui](https://github.com/ArthurSonzogni/FTXUI), [libag](https://github.com/Theldus/libag), [fmt](https://github.com/fmtlib/fmt), [rttr](https://github.com/rttrorg/rttr), [nlohmann\_json](https://github.com/nlohmann/json), [catch2](https://github.com/catchorg/Catch2), [clipp](https://github.com/muellan/clipp), [rangeless](https://github.com/ast-al/rangeless)

### Contributions & Feedback

Please feel free to open issues if you experience any bugs

If you have suggestions for features please open an issue or (if you're feeling very generous) make a PR
