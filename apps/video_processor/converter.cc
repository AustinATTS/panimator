// TODO(Austin): Add comments.
#include "converter.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace filesystem = std::filesystem;

namespace video {

    bool ExtractFrames (const filesystem::path & video, const filesystem::path & frame_directory, int frames_per_second) {
        if (!filesystem::exists(video)) {
            std::cerr << "[extract] Video not found: " << video << "\n";
            return false;
        }

        filesystem::create_directories(frame_directory);

        std::ostringstream command;
        command << "ffmpeg -y -loglevel error "
                << "-i \"" << video.string() << "\" "
                << "-vf fps=" << frames_per_second << " "
                << "\"" << (frame_directory / "out%04d.jpg").string() << "\"";

        std::cout << "[extract] " << command.str() << "\n";

        return std::system(command.str().c_str()) == 0;
    }

    bool ConvertFramesToAscii (const filesystem::path& frame_directory, const filesystem::path& ascii_directory) {
        if (!filesystem::exists(frame_directory)) {
            std::cerr << "[ascii] Frame directory missing: "
                      << frame_directory << "\n";
            return false;
        }

        filesystem::create_directories(ascii_directory);

        for (const auto& e : filesystem::directory_iterator(ascii_directory)) {
            filesystem::remove_all(e);
        }

        std::vector<filesystem::path> frames;
        for (const auto& e : filesystem::directory_iterator(frame_directory)) {
            if (e.is_regular_file())
                frames.push_back(e.path());
        }

        std::sort(frames.begin(), frames.end());

        if (frames.empty()) {
            std::cerr << "[ascii] No frames found\n";
            return false;
        }

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
