// TODO(Austin): Add comments.
#include "parser.h"

#include <fstream>
#include <cctype>
#include <thread>
#include <chrono>
#include <iostream>
#include <filesystem>

namespace filesystem = std::filesystem;

namespace parser {

    std::vector<Circle> ParseFrame(const filesystem::path& txt_file, int radius, const std::string& colour) {
        std::vector<Circle> circles;
        std::ifstream in(txt_file);
        if (!in) return circles;

        std::string line;
        int y = 0;
        while (std::getline(in, line)) {
            for (int x = 0; x < (int)line.size(); x++) {
                if (!std::isspace((unsigned char)line[x])) {
                    circles.push_back({x, y, radius, colour});
                }
            }
            y++;
        }
        return circles;
    }

    bool WriteJSON(const filesystem::path& out_file, const std::vector<Circle>& circles) {
        std::ofstream out(out_file, std::ios::trunc);
        if (!out) return false;

        out << "[\n";
        bool first = true;
        for (const auto& circle : circles) {
            if (!first) out << ",\n";
            first = false;
            out << "  { \"shape\": \"circle\", \"parameters\": { "
                << "\"x\": " << circle.x << ", "
                << "\"y\": " << circle.y << ", "
                << "\"radius\": " << circle.radius << ", "
                << "\"colour\": \"" << circle.colour << "\" } }";
        }
        out << "\n]\n";
        return true;
    }

    filesystem::path FindTXTDirectory(const std::string& default_path) {
        filesystem::path p(default_path);
        if (!filesystem::exists(p) || !filesystem::is_directory(p)) {
            throw std::runtime_error("TXT directory not found: " + default_path);
        }
        return p;
    }

} // namespace parser
