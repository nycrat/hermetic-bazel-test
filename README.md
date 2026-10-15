# llvm

A minimal Bazel project with a C++ library, binary and test, built with a
**fully hermetic LLVM toolchain** (`llvm` module by
[hermetic-llvm](https://github.com/hermeticbuild/hermetic-llvm)).

Hermetic means the compiler, linker, C/C++ runtimes and libc are all pinned and
built/linked by Bazel — the build does **not** depend on the host's Xcode,
system SDK, or which compiler happens to be installed. The same inputs produce
the same outputs on any machine, which also makes remote execution possible.

## Layout

```
llvm
├── MODULE.bazel              # Bazel module (bzlmod), hermetic toolchain + deps
├── .bazelrc                  # per-platform output dirs + cross-compile configs
├── .bazelversion             # Pinned Bazel version
├── .github/workflows/ci.yml  # CI matrix (native + cross-compile)
├── third_party
│   ├── BUILD.bazel           # exports vendored BUILD files + patch
│   ├── bullet3.BUILD          # Bazel BUILD for the bullet3 external repo
│   ├── g3log.BUILD           # Bazel BUILD for the g3log external repo
│   └── g3log_musl_no_backtrace.patch
└── main
    ├── BUILD                 # cc_library / cc_binary / cc_test targets
    ├── greeting.h            # std::string get_greet(const std::string&)
    ├── greeting.cc
    ├── greeting_test.cc      # GoogleTest unit tests
    ├── hello-world.cc        # the binary
    ├── logging_demo.cc       # g3log usage demo
    └── bullet_demo.cc        # Bullet Physics example
```

> Note: the Bazel *module* is named `hello_llvm`, not `llvm`, because the
> hermetic toolchain is published under the module name `llvm` and a module
> cannot share a name with one of its dependencies. The directory name is
> unrelated.

## Build, run & test (native)

```
bazel build //main:hello-world
bazel run   //main:hello-world
bazel run   //main:hello-world -- avah

bazel test //...
```

## Dependencies

- **googletest** (Bazel Central Registry) — the unit tests.
- **g3log** (v2.6.1) — asynchronous logging.
- **Bullet Physics SDK** (`bullet3`, revision `272c7099`) — rigid-body physics.

g3log is *not* in the Bazel Central Registry and ships no Bazel support, so
`MODULE.bazel` fetches the release tarball and builds it with
`third_party/g3log.BUILD`. That file reproduces g3log's CMake build: it
generates the `g3log/generated_definitions.hpp` header that CMake normally
writes into the build tree, and compiles the non-Windows sources.

A small patch (`third_party/g3log_musl_no_backtrace.patch`) guards g3log's use
of `<execinfo.h>`/`backtrace()` behind `G3LOG_DISABLE_BACKTRACE`, which is set
only for musl targets (musl has no `backtrace()`). The patch is a no-op
everywhere else, so fatal-signal handling keeps its stack traces on
macOS/glibc.

Run the demo (logs to stdout via a custom sink):

```
bazel run //main:logging-demo
```

Bullet is likewise not in the BCR and has no Bazel support, so `MODULE.bazel`
pins the source tarball and `third_party/bullet3.BUILD` mirrors its CMake
library layout: `LinearMath`, `BulletCollision`, `BulletDynamics`,
`BulletSoftBody`, `Bullet3Common`, `BulletInverseDynamics`, `Bullet3Geometry`,
`Bullet3Collision`, `Bullet3Dynamics` and `Bullet2FileLoader`, plus a `bullet`
umbrella target. Depend on `@bullet3//:bullet` (or an individual component) and
`#include <btBulletDynamicsCommon.h>`.

```
bazel build @bullet3//...
```

`Bullet3OpenCL`, `Extras/` and `examples/` are intentionally not built. The
defaults match upstream CMake (single precision, no multithreading); pass
`--copt=-DBT_USE_DOUBLE_PRECISION` to build with doubles.

Run the example (a sphere falls onto a static ground plane; it prints the height
at intervals and settles at `1.0`, the sphere's radius):

```
bazel run //main:bullet-demo
```

## Cross-compile to Linux

Targets are selected with `--platforms`; no sysroot or extra configuration is
needed. `--experimental_platform_in_output_dir` (set in `.bazelrc`) keeps each
platform's output in its own directory so artifacts don't overwrite each other.

```
# glibc (default 2.28), dynamically linked
bazel build --config=linux //...

# musl, fully statically linked
bazel build --config=linux-musl //...
```

Equivalent explicit form:

```
bazel build --platforms=@llvm//platforms:linux_x86_64      //...
bazel build --platforms=@llvm//platforms:linux_x86_64_musl --dynamic_mode=off //...
```

> The musl platform is fully static, so `--dynamic_mode=off` is required when
> any target links the C++ runtime dynamically (e.g. `cc_test`); otherwise the
> link fails with `attempted static link of dynamic object libc++.so.1`. The
> `--config=linux-musl` config already sets this.

Other useful target platforms: `@llvm//platforms:linux_aarch64`,
`...:linux_x86_64_gnu.2.28` (pin a minimum glibc), `...:linux_aarch64_musl`.

The cross-compiled binaries are Linux ELF files and cannot be run on macOS;
copy them to a Linux host (or use emulation) to execute.

## CI

`.github/workflows/ci.yml` runs a matrix that exercises the hermetic toolchain:

| Job | Runner | Command |
| --- | --- | --- |
| Linux / native test | `ubuntu-latest` | `bazel test //...` |
| Linux / cross to aarch64 | `ubuntu-latest` | `bazel build --platforms=@llvm//platforms:linux_aarch64 //...` |
| macOS / native test | `macos-latest` | `bazel test //...` |
| macOS / cross to x86_64 (glibc) | `macos-latest` | `bazel build --config=linux //...` |
| macOS / cross to x86_64 (musl) | `macos-latest` | `bazel build --config=linux-musl //...` |

Cross-compiled targets are only *built*, not run, since the resulting Linux
binaries can't execute on the build host.
