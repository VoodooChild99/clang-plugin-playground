# Clang Plugin Playground

Ready-to-use environment that makes Clang plugin development easy,
inspired by [clang-tutor](https://github.com/banach-space/clang-tutor).

## How to use

- Add new plugins to [lib](./lib) and modify `CMakeFiles.txt` accordingly.

- Add plugin information to [the provided compiler wrapper](./compiler/compiler.cpp) by modifying `plugins`.

- To build plugins, run `cmake-config.sh` first, then `cd build && make`.

- To use different LLVM/Clang versions, set the env var `LLVM_CONFIG` before you run `cmake-config.sh`,
e.g., `LLVM_CONFIG=llvm-config-15 ./cmake-config.sh` if you want to use LLVM-15/clang-15. By default,
`llvm-config` is used.

- Use the provided compiler wrappers (`build/compiler/custom-clang` and `build/compiler/custom-clang++`) to compile your programs. The wrappers will automatically load your plugins.
