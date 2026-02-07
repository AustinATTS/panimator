// Public interface for the Panimator parser.
// Defines the core data structures and functions used to
// convert TXT-based frame data into JSON-renderable circles.

#pragma once

#include <filesystem>  // Filesystem paths for input/output.
#include <string>      // Colour representation.
#include <vector>      // Container for circle collections.

namespace parser {

    // Represents a single renderable circle derived from
    // a non-whitespace character in a TXT frame.
    struct Circle {
        int x;              // Horizontal position in the frame grid.
        int y;              // Vertical position in the frame grid.
        int radius;         // Radius applied to the rendered circle.
        std::string colour; // Colour identifier (hex or named).
    };

    // Parses a TXT frame file and converts each visible character
    // into a Circle with the given radius and colour.
    std::vector<Circle> ParseFrame(const std::filesystem::path& txt_file, int radius = 0, const std::string& colour = "orange");

    // Writes a collection of circles to a JSON file in a format
    // expected by the Panimator renderer.
    bool WriteJSON(const std::filesystem::path& out_file, const std::vector<Circle>& circles);

    // Locates and validates the directory containing TXT frame files.
    // Throws an exception if the directory does not exist or is invalid.
    std::filesystem::path FindTXTDirectory(const std::string& default_path = "data/txt_files");

} // namespace parser
