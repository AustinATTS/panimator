# Panimator

Panimator converts a video into a sequence of text-based frame files and then animates those frames in a web browser.

## Example output:

<video controls autoplay loop muted>
  <source src="assets/videos/example_1.mp4" type="video/mp4">
  Your browser does not support the video tag.
</video>

## Requirements

* **Bazel** (tested with 9.0.0; Bazelisk is recommended)
* **ascii-image-converter** (available on the AUR and other package managers)

## Usage

### Build the pipeline

To build the full pipeline entry point:

```sh
bazel build //apps/pipeline:pipeline_runner
```

This target pulls in and builds all required dependencies.
To build everything manually:

```sh
bazel build //...
```

### Available Bazel targets

* `//apps/pipeline:pipeline_runner`
* `//apps/data_parser:parser_lib`
* `//apps/data_parser:parser_tool`
* `//apps/video_processor:converter_lib`
* `//apps/video_processor:video_tool`
* `//web:server`

## Generating frame files

The repository includes a pre-generated example (from *Bad Apple!!*). To generate your own text frames from a video, you can either run the full pipeline or invoke the video processor directly.

Bazel-built executables are located in the `bazel-bin` directory.

### Using the pipeline

```sh
./bazel-bin/apps/pipeline/pipeline_runner --build-files
```

### Using the video processor directly

```sh
./bazel-bin/apps/video_processor/video_tool
```

or via Bazel:

```sh
bazel run //apps/video_processor:video_tool
```

### Video processor flags

* `--fps N`
  Frames per second for video conversion. Defaults to 30.

* `--video PATH`
  Path to a custom video file. Partially implemented.

## Running the web server

The animation is displayed via a simple Python-based web server. It can be started manually or through Bazel, and it will also start automatically when running the pipeline.

Available options:

```sh
bazel run //web:server
```

```sh
python web/server.py
```

```sh
./bazel-bin/apps/pipeline/pipeline_runner
```

## Parsing text files

Text frames can be parsed either as part of the pipeline or by running the parser directly.

### Using the pipeline

```sh
./bazel-bin/apps/pipeline/pipeline_runner
```

### Using the parser directly

```sh
./bazel-bin/apps/data_parser/parser_tool
```

or via Bazel:

```sh
bazel run //apps/data_parser:parser_tool
```

### Parser flags (partially implemented)

* `--fps N`
  Frames per second for parsing. Currently defaults to ~30 (33ms per frame).

* `--colour COLOUR`
  Circle colour (CSS colour name or hex code).

* `--delay N`
  Frame delay in milliseconds.

* `--radius N`
  Radius of rendered
