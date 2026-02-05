load("@rules_python//python:defs.bzl", "py_binary", "py_library")

# circles.json is publicly accessible.
filegroup(
    name = "circles_json",
    srcs = ["circles.json"],
    visibility = ["//visibility:public"],
)

# Group files needed for the site together.
filegroup(
    name = "web_assets",
    srcs = [
        "circles_json",
        "index.html",
        "scripts.js",
        "styles.css",
    ],
)

# Server build config.
py_binary(
    name = "server",
    srcs = ["server.py"],
    data = [":web_assets"],
    tags = ["no-sandbox"],  # TODO(Austin): Write an overall build sequence so it can be sandboxed together.
    visibility = ["//visibility:public"],
)
