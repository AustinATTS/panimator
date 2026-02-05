// TODO(Austin): Add comments.
#include "converter.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace filesystem = std::filesystem;

filesystem::path FindVideoViaExecutable (char* argv0) {
    filesystem::path executable = filesystem::canonical(argv0);

    filesystem::path runfiles =
        executable.parent_path() /
        (executable.filename().string() + ".runfiles") /
        "_main" /
        "data" /
        "video.mp4";

    if (!filesystem::exists(runfiles)) {
        throw std::runtime_error("Runfile not found at expected location: " + runfiles.string());
    }

    return runfiles;
}

int main (int argc, char* argv[]) {
    int frames_per_second = 30;

    for (int i = 1; i < argc; ++i) {
        std::string arguments = argv[i];
        if (arguments == "--fps" && i + 1 < argc) {
            frames_per_second = std::stoi(argv[++i]);
        } else {
            std::cerr << "Usage: bazel run //apps/video_processor:video_tool -- [--fps N]\n";
            return 1;
        }
    }

    try {
        filesystem::path video = FindVideoViaExecutable(argv[0]);

        filesystem::path frames_directory = "data/frames";
        filesystem::path ascii_directory  = "data/txt_files";

        std::cout
            << "Bad Apple pipeline\n"
            << "  Video : " << video << "\n"
            << "  Frames: " << frames_directory << "\n"
            << "  ASCII : " << ascii_directory << "\n"
            << "  FPS   : " << frames_per_second << "\n";

        if (!video::ProcessVideo(video, frames_directory, ascii_directory, frames_per_second)) {
            std::cerr << "Pipeline failed\n";
            return 1;
        }

        std::cout << "Pipeline complete\n";
        return 0;
    }
    catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << "\n";
        return 1;
    }
}
