load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

# Handles converting each frame of the video to an image as well as converting those to ascii.
cc_library(
    name = "converter_lib",
    srcs = ["converter.cc"],
    hdrs = ["converter.h"],
)

# Main video converter build.
cc_binary(
    name = "video_tool",
    srcs = ["main.cc"],
    data = ["//data:video"],  # Requires the video.
    tags = ["no-sandbox"],
    visibility = ["//visibility:public"],
    deps = [":converter_lib"],  # Depends on the converter library.
)
