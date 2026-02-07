// TODO(Austin): Add comments.
#include "parser.h"
#include <filesystem>
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>

void PrintHelp () {
    std::cout <<
R"(parser_tool – Panimator

Usage:
  parser_tool [options]

Options:
  --help                 Show this help and exit
  --fps <number>         Frames per second override
  --colour <hex|name>    Circle colour override
  --radius <number>      Circle radius override

Example:
  parser_tool --fps 30
)"
    << std::endl;
}

namespace filesystem = std::filesystem;
namespace parser = ::parser;

int main (int argument_count, char* argument_vector[]) {
    std::string output_file = "web/circles.json";
    int frames_per_second = 30;
    int radius = 0;
    std::string colour = "#4c4f69";

    for (int i = 1; i < argument_count; ++i) {
        std::string argument = argument_vector[i];
        if (argument == "--help") {
            PrintHelp();
            return 0;
        }
        if (argument == "--fps" && i + 1 < argument_count) {
            output_file = argument_vector[++i];
        }
        else {
            if (argument == "--radius" && i + 1 < argument_count) {
                radius = std::stoi(argument_vector[++i]);
            }
            else {
                if (argument == "--colour" && i + 1 < argument_count) {
                    colour = argument_vector[++i];
                }
                else {
                    PrintHelp();
                    return 0;
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
    for (auto& entry : filesystem::directory_iterator(txt_dir)) {
        if (entry.is_regular_file()) files.push_back(entry.path());
    }

    std::sort(files.begin(), files.end());
    if (files.empty()) {
        std::cerr << "No TXT frames found in " << txt_dir << "\n";
        return 1;
    }

    std::cout << "Loaded " << files.size() << " frames\n";

    int frame_delay_ms = 1000 / frames_per_second;
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