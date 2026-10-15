load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
    name = "greeting",
    srcs = ["greeting.cc"],
    hdrs = ["greeting.h"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
    deps = [":greeting"],
)

cc_test(
    name = "greeting_test",
    srcs = ["greeting_test.cc"],
    deps = [
        ":greeting",
        "@googletest//:gtest_main",
    ],
)

# Demonstrates consuming the external g3log dependency.
cc_binary(
    name = "logging-demo",
    srcs = ["logging_demo.cc"],
    deps = ["@g3log//:g3log"],
)
