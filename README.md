# llvm

A minimal Bazel project with a C++ "hello world" binary, built with a
**fully hermetic LLVM toolchain** (`llvm` module by
[hermetic-llvm](https://github.com/hermeticbuild/hermetic-llvm)).

Hermetic means the compiler, linker, C/C++ runtimes and libc are all pinned and
built/linked by Bazel — the build does **not** depend on the host's Xcode,
system SDK, or which compiler happens to be installed. The same inputs produce
the same outputs on any machine, which also makes remote execution possible.

## Layout

```
llvm
├── MODULE.bazel          # Bazel module (bzlmod) + hermetic llvm toolchain
├── .bazelrc              # per-platform output dirs + cross-compile configs
├── .bazelversion         # Pinned Bazel version
└── main
    ├── BUILD             # cc_binary target
    └── hello-world.cc    # Hello world source
```

> Note: the Bazel *module* is named `hello_llvm`, not `llvm`, because the
> hermetic toolchain is published under the module name `llvm` and a module
> cannot share a name with one of its dependencies. The directory name is
> unrelated.

## Build & run (native)

```
bazel build //main:hello-world
bazel run //main:hello-world
bazel run //main:hello-world -- avah
```

## Cross-compile to Linux

Targets are selected with `--platforms`; no sysroot or extra configuration is
needed. `--experimental_platform_in_output_dir` (set in `.bazelrc`) keeps each
platform's output in its own directory so artifacts don't overwrite each other.

```
# glibc (default 2.28), dynamically linked
bazel build --config=linux //main:hello-world

# musl, fully statically linked
bazel build --config=linux-musl //main:hello-world
```

Equivalent explicit form:

```
bazel build --platforms=@llvm//platforms:linux_x86_64      //main:hello-world
bazel build --platforms=@llvm//platforms:linux_x86_64_musl //main:hello-world
```

Other useful target platforms: `@llvm//platforms:linux_aarch64`,
`...:linux_x86_64_gnu.2.28` (pin a minimum glibc), `...:linux_aarch64_musl`.

The cross-compiled binaries are Linux ELF files and cannot be run on macOS;
copy them to a Linux host (or use emulation) to execute.
