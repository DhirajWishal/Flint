
# Flint

Flint is a high performance graphics engine.

## How to use

Make sure that you have the following dependencies to build and use this engine.

1. [CMake](https://cmake.org/).
2. A C++ editor and compiler.
3. [Visual Studio](https://visualstudio.microsoft.com/) and the C++ workloads if on Windows.
4. [Visual Studio Code](https://code.visualstudio.com/) or [Intellij Clion](https://www.jetbrains.com/clion/) along with a compiler like [Clang](https://clang.llvm.org/) if on Linux.
5. [Xcode](https://developer.apple.com/xcode/) or [Visual Studio](https://visualstudio.microsoft.com/) if on macOS.
6. [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/).

First off, clone this repository to a local directory.

```bash
git clone https://github.com/DhirajWishal/Flint {SOURCE}
cd {SOURCE}
git submodule update --init --recursive
```

*Note that `{SOURCE}` is the location where the repository is cloned to.*

After cloning the repository and initializing all the submodules, we can proceed to build the repository. But make sure that you have all the [GLFW Dependencies](https://www.glfw.org/docs/latest/compile.html) configured if on Linux.

```bash
cmake -S . -B build
cd build
cmake --build . --config {CONFIG}
```

*`{CONFIG}` is either `Release` or `Debug` if on Windows.*

Once built, set the include directory to `{SOURCE}/Runtime`, set the link directory to `{SOURCE}/build/{CONFIG}` and the link library as `FlintEngine`. From there on, you're free to start building!

## License

This repository is licensed under Apache-2.0.
