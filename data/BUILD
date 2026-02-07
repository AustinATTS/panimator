package(default_visibility = ["//visibility:public"])

# Video file, single source.
filegroup(
    name = "video",
    srcs = ["video.mp4"],
)

# txt files for each video frame.
filegroup(
    name = "ascii_frames",
    srcs = glob(
        ["txt_files/*.txt"],
        allow_empty = True,
    ),
)

# Image files for each frame.
filegroup(
    name = "raw_frames",
    srcs = glob(
        ["frames/*"],
        allow_empty = True,
    ),
)
