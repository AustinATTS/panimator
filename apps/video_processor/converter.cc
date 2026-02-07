// Converter implementation for Panimator.
// Handles frame extraction from video files and converts
// each frame into an ASCII representation for further parsing.

#include "converter.h"

#include <cstdlib>      // std::system
#include <filesystem>   // Path and directory management
#include <iostream>     // Status and error output
#include <sstream>      // Command construction
#include <vector>       // Frame collection
#include <algorithm>    // Sorting frame order

namespace filesystem = std::filesystem;

namespace video {

    // Extracts still frames from a video at a fixed frame rate
    // using ffmpeg. Frames are written sequentially to disk.
    bool ExtractFrames (const filesystem::path & video, const filesystem::path & frame_directory, int frames_per_second) {
        // Validate input video exists.
        if (!filesystem::exists(video)) {
            std::cerr << "[extract] Video not found: " << video << "\n";
            return false;
        }

        // Ensure output directory exists.
        filesystem::create_directories(frame_directory);

        // Build ffmpeg command for frame extraction.
        std::ostringstream command;
        command << "ffmpeg -y -loglevel error "
                << "-i \"" << video.string() << "\" "
                << "-vf fps=" << frames_per_second << " "
                << "\"" << (frame_directory / "out%04d.jpg").string() << "\"";

        std::cout << "[extract] " << command.str() << "\n";

        // Execute extraction command.
        return std::system(command.str().c_str()) == 0;
    }

    // Converts extracted image frames into ASCII text files
    // using an external ASCII image conversion tool.
    bool ConvertFramesToAscii (const filesystem::path& frame_directory, const filesystem::path& ascii_directory) {
        // Ensure source frame directory exists.
        if (!filesystem::exists(frame_directory)) {
            std::cerr << "[ascii] Frame directory missing: "
                      << frame_directory << "\n";
            return false;
        }

        // Create or reuse ASCII output directory.
        filesystem::create_directories(ascii_directory);

        // Clear any existing ASCII output to avoid stale frames.
        for (const auto& e : filesystem::directory_iterator(ascii_directory)) {
            filesystem::remove_all(e);
        }

        // Collect extracted frame images.
        std::vector<filesystem::path> frames;
        for (const auto& e : filesystem::directory_iterator(frame_directory)) {
            if (e.is_regular_file()) {
                frames.push_back(e.path());
            }
        }

        // Sort frames to maintain playback order.
        std::sort(frames.begin(), frames.end());

        if (frames.empty()) {
            std::cerr << "[ascii] No frames found\n";
            return false;
        }

        // Convert each frame image to an ASCII text file.
        for (const auto& frame : frames) {
            filesystem::path out = ascii_directory / (frame.stem().string() + ".txt");

            std::ostringstream command;
            command << "ascii-image-converter "
                << "\"" << frame.string() << "\" "
                << "--dimensions 146,54 "
                << "> \"" << out.string() << "\"";

            int ret = std::system(command.str().c_str());
            if (ret != 0) {
                std::cerr << "[ascii] Failed: "
                          << frame << "\n";
                return false;
            }
        }

        return true;
    }

    // High-level orchestration function that converts a video
    // into ASCII frames by chaining extraction and conversion.
    bool ProcessVideo (const filesystem::path& video, const filesystem::path& frame_directory, const filesystem::path& ascii_directory, int frames_per_second) {
        std::cout << "Extracting frames...\n";
        if (!ExtractFrames(video, frame_directory, frames_per_second)) {
            return false;
        }

        std::cout << "Converting frames to ASCII...\n";
        if (!ConvertFramesToAscii(frame_directory, ascii_directory)) {
            return false;
        }

        return true;
    }

} // namespace video
