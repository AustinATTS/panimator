# Panimator

Panimator converts a video into a sequence of ASCII text frames and animates those frames in a web browser by translating characters into rendered primitives.

The pipeline is split into three main stages:

1. **Video processing** - extract frames and convert them to ASCII
2. **Parsing** - translate ASCII frames into JSON animation data
3. **Rendering** - stream frames to a browser via a lightweight web server

## Example output

<video controls autoplay loop muted>
  <source src="https://github.com/AustinATTS/panimator/assets/videos/example_1.mp4" type="video/mp4">
  Your browser does not support the video tag.
</video>

## Requirements

* **Bazel** (tested with 9.0.0; Bazelisk recommended)
* **ffmpeg** (for frame extraction)
* **ascii-image-converter** (available on the AUR and other package managers)
* **Python 3** (for the web server)

## Building

### Build the full pipeline entry point

```sh
bazel build //apps/pipeline:pipeline_runner
```

This target pulls in and builds all required components.

### Build everything

```sh
bazel build //...
```

### Available Bazel targets

#### Pipeline

* `//apps/pipeline:pipeline_runner`

#### Video processing

* `//apps/video_processor:converter_lib`
* `//apps/video_processor:video_tool`

#### Parsing

* `//apps/data_parser:parser_lib`
* `//apps/data_parser:parser_tool`

#### Web

* `//web:server`

Build executables are placed under `bazel-bin/`.

## Running the full pipeline

The pipeline runner coordinates:

- Optional video conversion
- Starting the web server
- Running the parser loop

```sh
./bazel-bin/apps/pipeline/pipeline_runner
```

### Pipeline flags

- `--build-files`
  Extract frames and generate ASCII files before starting the pipeline.
- `--video PATH`
  Input video file for conversion (used with `--build-files`).
- `--fps N`
  Frames per second override.
- `--colour COLOUR`
  Circle colour (CSS colour name or hex code).
- `--radius N`
  Radius of rendered circles.
- `--verbose`
  Enable HTTP reequest logging in the web server.

#### Example:

```sh
./bazel-bin/apps/pipeline/pipeline_runner \
  --build-files \
  --video badapple.mp4 \
  --fps 30 \
  --colour #4c4f69 \
  --radius 1
```

## Video Processing

The video processor converts a video into ASCII text frames.

It performs two steps:

1. Extracts frames using `ffmpeg`
2. Converts frames to ASCII using `ascii-image-converter`

### Run directly

```sh
./bazel-bin/apps/video_processor/video_tool
```

or via Bazel:

```sh
bazel run //apps/video_processor:video_tool
```

### Video processor flags

* `--fps N`
  Frames per second for conversion (Default: 30).

* `--video PATH`
  Path to a custom video file.

#### Output Directories:

- Extracted frames: `data/frames/`
- ASCII frames: `data/txt_files/`

## Parsing ASCII frames

The parser converts ASCII frames into JSON circle data that the web renderer consumes.

### Run via pipeline

```sh
./bazel-bin/apps/pipeline/pipeline_runner
```

### Run directly

```sh
./bazel-bin/apps/data_parser/parser_tool
```

or via Bazel:

```sh
bazel run //apps/data_parser:parser_tool
```

### Parser flags

* `--fps N`
  Playback rate for parsed frames.

* `--colour COLOUR`
  Circle colour (CSS colour name or hex code).

* `--radius N`
  Radius applied to each rendered circle.

Parsed output is written continuously to:

`web/circles.json`


## Web server

The animation is displayed in a browser via a simple Python HTTP server.

It serves static files from the `web/` directory and streams live updates.

### Run manually

```python
python web/server.py
```

### Run via Bazel

```sh
bazel run //web:server
```

### Run via pipeline

The server starts automatically when using `pipeline_runner`.

### Verbose logging

Enable request logging by setting:

```python
VERBOSE=1
```

or by using the pipeline flag:

```sh
--verbose
```

## Notes

- ASCII frame dimensions are currently fixed in the converter.
- JSON output is rewritten every frame to support live animation.
- The pipeline is designed for simplicity over throughput.