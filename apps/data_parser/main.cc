// TODO(Austin): Add comments.
#include "parser.h"
#include <filesystem>
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>

namespace filesystem = std::filesystem;
namespace parser = ::parser;

int main (int argc, char* argv[]) {
    std::string output_file = "web/circles.json";
    int frame_delay_ms = 33;
    int radius = 0;
    std::string colour = "#4c4f69";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        }
        else {
            if (arg == "-d" && i + 1 < argc) {
                frame_delay_ms = std::stoi(argv[++i]);
            }
            else {
                if (arg == "-r" && i + 1 < argc) {
                    radius = std::stoi(argv[++i]);
                }
                else {
                    if (arg == "-c" && i + 1 < argc) {
                        colour = argv[++i];
                    }
                    else {
                        std::cerr << "Usage: " << argv[0] << " [-o output.json] [-d delay_ms] [-r radius] [-c colour]\n";
                        return 1;
                    }
                }
            }
        }
    }

    filesystem::path txt_dir;
    try {
        txt_dir = parser::FindTXTDirectory("data/txt_files");
    }
    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }

    std::vector<filesystem::path> files;
    for (auto& e : filesystem::directory_iterator(txt_dir)) {
        if (e.is_regular_file()) files.push_back(e.path());
    }

    std::sort(files.begin(), files.end());
    if (files.empty()) {
        std::cerr << "No TXT frames found in " << txt_dir << "\n";
        return 1;
    }

    std::cout << "Loaded " << files.size() << " frames\n";

    while (true) {
        for (const auto& file : files) {
            auto circles = parser::ParseFrame(file, radius, colour);
            if (!parser::WriteJSON(output_file, circles)) {
                std::cerr << "Failed to write " << output_file << "\n";
                return 1;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(frame_delay_ms));
        }
    }

    return 0;
}