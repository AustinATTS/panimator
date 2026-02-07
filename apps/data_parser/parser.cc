// Parser implementation for Panimator.
// Converts ASCII TXT frame data into circle primitives
// and serialises them into a JSON format for rendering.

#include "parser.h"
#include <fstream>      // File input/output streams.
#include <cctype>       // Character classification (isspace).
#include <thread>       // Reserved for timing-related expansion.
#include <chrono>       // Reserved for timing-related expansion.
#include <iostream>     // Error and debug output.
#include <filesystem>   // Filesystem paths and directory checks.

namespace filesystem = std::filesystem;

namespace parser {

    // Reads a single TXT frame and converts non-whitespace
    // characters into circle objects positioned on a grid.
    std::vector<Circle> ParseFrame(const filesystem::path& txt_file, int radius, const std::string& colour) {
        std::vector<Circle> circles;

        // Open frame file; return empty result if unavailable.
        std::ifstream in(txt_file);
        if (!in) {
            return circles;
        }

        std::string line;
        int y = 0;

        // Process file line-by-line to preserve Y ordering.
        while (std::getline(in, line)) {
            // Each non-space character becomes a circle at (x, y).
            for (int x = 0; x < (int)line.size(); x++) {
                if (!std::isspace((unsigned char)line[x])) {
                    circles.push_back({ x, y, radius, colour });
                }
            }
            y++;
        }

        return circles;
    }

    // Serialises a collection of circles into a flat JSON array.
    // The file is fully rewritten each frame to support live updates.
    bool WriteJSON(const filesystem::path& out_file, const std::vector<Circle>& circles) {
        // Truncate ensures old frame data is discarded.
        std::ofstream out(out_file, std::ios::trunc);
        if (!out) {
            return false;
        }

        out << "[\n";

        // Track first element to correctly place commas.
        bool first = true;
        for (const auto& circle : circles) {
            if (!first) {
                out << ",\n";
            }
            first = false;

            // Emit a single circle object.
            out << "  { \"shape\": \"circle\", \"parameters\": { "
                << "\"x\": " << circle.x << ", "
                << "\"y\": " << circle.y << ", "
                << "\"radius\": " << circle.radius << ", "
                << "\"colour\": \"" << circle.colour << "\" } }";
        }

        out << "\n]\n";
        return true;
    }

    // Validates and returns the directory containing frame TXT files.
    // Throws if the path does not exist or is not a directory.
    filesystem::path FindTXTDirectory(const std::string& default_path) {
        filesystem::path p(default_path);

        if (!filesystem::exists(p) || !filesystem::is_directory(p)) {
            throw std::runtime_error(
                "TXT directory not found: " + default_path
            );
        }

        return p;
    }

} // namespace parser
