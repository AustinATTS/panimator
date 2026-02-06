# TODO(Austin): Add comments.
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "pipeline",
    srcs = ["pipeline.py"],
    data = [
        "//apps/data_parser:parser_tool",
        "//apps/video_processor:video_tool",
        "//data:ascii_frames",
        "//data:raw_frames",
        "//data:video",
        "//web:server",
    ],
    main = "pipeline.py",
    visibility = ["//visibility:public"],
)
