#pragma once

#include <filesystem>

namespace video {

    // Extracts still frames from a video file at a fixed frame rate
    // using ffmpeg and writes them sequentially to the frame directory.
    bool ExtractFrames (const std::filesystem::path& video, const std::filesystem::path& frame_directory, int frames_per_second);

    // Converts extracted image frames into ASCII text files using
    // ascii-image-converter.
    //
    // TODO(Austin): Replace hardcoded dimensions (146x54) with a
    // runtime-configurable parameter.
    bool ConvertFramesToAscii (const std::filesystem::path& frame_directory, const std::filesystem::path& ascii_directory);

    // High-level helper that processes a video by extracting frames
    // and converting them into ASCII representations.
    //
    // Pipeline:
    //   video.mp4 -> ExtractFrames() -> ConvertFramesToAscii()
    bool ProcessVideo (const std::filesystem::path& video, const std::filesystem::path& frame_directory, const std::filesystem::path& ascii_directory, int frames_per_second);

} // namespace video
