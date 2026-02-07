// TODO(Austin): Add comments.
#include "converter.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

void PrintHelp () {
    std::cout <<
R"(video_tool – Panimator

Usage:
  video_tool [options]

Options:
  --help                 Show this help and exit
  --video <path>         Input video file (used with --build-files)
  --fps <number>         Frames per second override

Example:
  video_tool --video badapple.mp4 --fps 30
)"
    << std::endl;
}

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

int main (int argument_count, char* argument_vector[]) {
    int frames_per_second = 30;
    std::string video;

    for (int i = 1; i < argument_count; ++i) {
        std::string argument = argument_vector[i];
        if (argument == "--help") {
            PrintHelp();
            return 0;
        }
        if (argument == "--fps" && i + 1 < argument_count) {
            frames_per_second = std::stoi(argument_vector[++i]);
        } else {
            if (argument == "--video" && i + 1 < argument_count) {
                video = argument_vector[++i];
            }
            else {
                PrintHelp();
                return 0;
            }
        }
    }

    try {
        filesystem::path video_path;
        if (!video.empty()) {
            video_path = filesystem::path(video);
            if (!filesystem::exists(video_path)) {
                throw std::runtime_error("Video not found: " + video_path.string());
            }
            video_path = filesystem::canonical(video_path);
        }
        else {
            video_path = FindVideoViaExecutable(argument_vector[0]);
        }

        filesystem::path frames_directory = "data/frames";
        filesystem::path ascii_directory  = "data/txt_files";

        std::cout
            << "Panimator Video Processor\n"
            << "  Video : " << video_path << "\n"
            << "  Frames: " << frames_directory << "\n"
            << "  ASCII : " << ascii_directory << "\n"
            << "  FPS   : " << frames_per_second << "\n";

        if (!video::ProcessVideo(video_path, frames_directory, ascii_directory, frames_per_second)) {
            std::cerr << "Video Processor failed\n";
            return 1;
        }

        std::cout << "Video Processor complete\n";
        return 0;
    }
    catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << "\n";
        return 1;
    }
}
