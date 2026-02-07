// Entry point for the Panimator video processing tool.
// Resolves an input video, extracts frames, and converts them
// into ASCII representations for downstream parsing.

#include "converter.h"

#include <filesystem>   // Path resolution and existence checks.
#include <iostream>     // Console output.
#include <stdexcept>    // Exception handling.
#include <string>       // Argument storage.

// Prints CLI usage information and exits.
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

// Resolves a default video path relative to the executable.
// Intended for Bazel runfiles where assets are bundled
// alongside the binary at runtime.
filesystem::path FindVideoViaExecutable (char* argv0) {
    // Canonicalise the executable path to avoid symlink issues.
    filesystem::path executable = filesystem::canonical(argv0);

    // Construct the expected runfiles location for the video asset.
    filesystem::path runfiles =
        executable.parent_path() /
        (executable.filename().string() + ".runfiles") /
        "_main" /
        "data" /
        "video.mp4";

    if (!filesystem::exists(runfiles)) {
        throw std::runtime_error(
            "Runfile not found at expected location: " +
            runfiles.string()
        );
    }

    return runfiles;
}

int main (int argument_count, char* argument_vector[]) {
    // Default processing parameters.
    int frames_per_second = 30;
    std::string video;

    // Parse command-line arguments.
    for (int i = 1; i < argument_count; ++i) {
        std::string argument = argument_vector[i];

        if (argument == "--help") {
            PrintHelp();
            return 0;
        }

        // Override frame rate.
        if (argument == "--fps" && i + 1 < argument_count) {
            frames_per_second = std::stoi(argument_vector[++i]);
        }
        else {
            // Explicit input video path.
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

        // Resolve video path from CLI or fallback runfiles.
        if (!video.empty()) {
            video_path = filesystem::path(video);

            if (!filesystem::exists(video_path)) {
                throw std::runtime_error(
                    "Video not found: " + video_path.string()
                );
            }

            video_path = filesystem::canonical(video_path);
        }
        else {
            video_path = FindVideoViaExecutable(argument_vector[0]);
        }

        // Output directories for frame images and ASCII frames.
        filesystem::path frames_directory = "data/frames";
        filesystem::path ascii_directory  = "data/txt_files";

        // Emit resolved configuration for visibility.
        std::cout
            << "Panimator Video Processor\n"
            << "  Video : " << video_path << "\n"
            << "  Frames: " << frames_directory << "\n"
            << "  ASCII : " << ascii_directory << "\n"
            << "  FPS   : " << frames_per_second << "\n";

        // Execute the video processing pipeline.
        if (!video::ProcessVideo(video_path, frames_directory, ascii_directory, frames_per_second)) {
            std::cerr << "Video Processor failed\n";
            return 1;
        }

        std::cout << "Video Processor complete\n";
        return 0;
    }
    catch (const std::exception& error) {
        // Catch and report any fatal configuration or runtime errors.
        std::cerr << "Fatal error: " << error.what() << "\n";
        return 1;
    }
}
