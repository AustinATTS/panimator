#pragma once

#include <filesystem>

namespace video {

    // Extract frames from a video using ffmpeg.
    bool ExtractFrames (const std::filesystem::path& video, const std::filesystem::path& frame_directory, int frames_per_second);

    // Convert extracted frames into ASCII text files using ascii-image-converter.
    // TODO(Austin): Change dimensions based on an input flag instead of hardcoded 146x54.
    bool ConvertFramesToAscii (const std::filesystem::path& frame_directory, const std::filesystem::path& ascii_directory);

    // video.mp4 -> ExtractFrames() -> ConvertFramesToAscii().
    bool ProcessVideo (const std::filesystem::path& video, const std::filesystem::path& frame_directory, const std::filesystem::path& ascii_directory, int frames_per_second);

} // namespace video
