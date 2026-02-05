// TODO(Austin): Add comments.
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace parser {

    struct Circle {
        int x;
        int y;
        int radius;
        std::string colour;
    };

    std::vector<Circle> ParseFrame (const std::filesystem::path& txt_file, int radius = 0, const std::string& colour = "orange");
    bool WriteJSON (const std::filesystem::path& out_file, const std::vector<Circle>& circles);

    std::filesystem::path FindTXTDirectory (const std::string& default_path = "data/txt_files");

} // namespace parser
