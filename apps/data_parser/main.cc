// Entry point for the Panimator parser tool.
// Responsible for reading frame data from TXT files,
// converting them into JSON circle data, and streaming
// them at a fixed frame rate.

#include "parser.h"              // Core parsing and JSON writing utilities.
#include <filesystem>            // Directory traversal and file handling.
#include <iostream>              // Console I/O.
#include <thread>                // Sleep control for frame timing.
#include <chrono>                // Time units for frame delay.
#include <algorithm>             // Sorting frame files.

// Prints CLI usage information and exits.
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

// Short aliases for commonly-used namespaces.
namespace filesystem = std::filesystem;
namespace parser = ::parser;

int main (int argument_count, char* argument_vector[]) {
    // Default runtime configuration.
    std::string output_file = "web/circles.json";
    int frames_per_second = 30;
    int radius = 0;
    std::string colour = "#4c4f69"; // Catppuccin Latte Text.

    // Basic command-line argument parsing.
    for (int i = 1; i < argument_count; ++i) {
        std::string argument = argument_vector[i];

        // Display help and exit.
        if (argument == "--help") {
            PrintHelp();
            return 0;
        }

        // Override FPS.
        if (argument == "--fps" && i + 1 < argument_count) {
            frames_per_second = std::stoi(argument_vector[++i]);
        }
        else {
            // Override circle radius.
            if (argument == "--radius" && i + 1 < argument_count) {
                radius = std::stoi(argument_vector[++i]);
            }
            else {
                // Override circle colour.
                if (argument == "--colour" && i + 1 < argument_count) {
                    colour = argument_vector[++i];
                }
                else {
                    // Unknown or malformed argument.
                    PrintHelp();
                    return 0;
                }
            }
        }
    }

    // Locate the directory containing frame TXT files.
    filesystem::path txt_dir;
    try {
        txt_dir = parser::FindTXTDirectory("data/txt_files");
    }
    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }

    // Collect all regular files in the frame directory.
    std::vector<filesystem::path> files;
    for (auto& entry : filesystem::directory_iterator(txt_dir)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }

    // Ensure frames are processed in deterministic order.
    std::sort(files.begin(), files.end());

    // Abort if no frame data is present.
    if (files.empty()) {
        std::cerr << "No TXT frames found in " << txt_dir << "\n";
        return 1;
    }

    std::cout << "Loaded " << files.size() << " frames\n";

    // Convert FPS into a per-frame delay in milliseconds.
    int frame_delay_ms = 1000 / frames_per_second;

    // Main render loop: cycles frames indefinitely.
    while (true) {
        for (const auto& file : files) {
            // Parse a single frame into circle definitions.
            auto circles = parser::ParseFrame(file, radius, colour);

            // Write frame output to JSON for the renderer.
            if (!parser::WriteJSON(output_file, circles)) {
                std::cerr << "Failed to write " << output_file << "\n";
                return 1;
            }

            // Enforce frame timing.
            std::this_thread::sleep_for(
                std::chrono::milliseconds(frame_delay_ms)
            );
        }
    }

    return 0;
}
