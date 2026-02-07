load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

# Handles converting text files into a set of circles.
cc_library(
    name = "parser_lib",
    srcs = ["parser.cc"],
    hdrs = ["parser.h"],
)

# Main parser build.
cc_binary(
    name = "parser_tool",
    srcs = ["main.cc"],
    data = [
        "//data:ascii_frames",  # Requires all the text files to read from.
        "//web:circles_json",  # as well as the json file to write to.
    ],
    tags = ["no-sandbox"],
    visibility = ["//visibility:public"],
    deps = [":parser_lib"],  # Depends on the parser library.
)
