# Panimator

converts a video into a whole bunch of text files, then animates them on the web.

## Requirements

- Bazel (Version 9.0.0 but Bazelisk is preferred)
- ascii-image-converter (available on the aur and other stuff)

## Usage

### Build the Pipeline

`bazel build //apps/pipeline:pipeline_runner`

This will handle the building and running of all other dependancies however they can all be built manually if needed.

`bazel build //...`

The individual targets available are

- //apps/pipeline:pipeline_runner
- //apps/data_parser:parser_lib
- //apps/data_parser:parser_tool
- //apps/video_processor:converter_lib
- //apps/video_processor:video_tool
- //web:server

### Generate files

A set of files is provided as an example in the repo (from Bad Apple!!) however to generate your own text files from a video you can either use its own tool, or the pipeline

The executate files from bazel can be found in the `bazel-bin` directory

`./bazel-bin/apps/pipeline/pipeline_runner --build-files`

or if using the video_tool on its own

`./bazel-bin/apps/video_processor/video_tool`
`bazel run //apps/video_processor:video_tool`

#### Flags

The following flags are available

- `--fps N` (The frames per second for the video converter. Defaults to 30)
- `--video PATH` (A custom path to the video file. Not yet implemented fully)


### Starting the web server

The animation gets displayed on a web server that is set up with a basic python server. You can run this using python itself, or by running it with bazel. It will also be automatically started when you run the pipeline.

- `bazel run //web:server`
- `python web/server.py`
- `./bazel-bin/apps/pipeline/pipeline_runner`

### Parsing the text files

To parse the text files you can either use the pipeline which will do it for you , or individually through its executable or bazel script.

`./bazel-bin/apps/pipeline/pipeline_runner`

or

`./bazel-bin/apps/data_parser/parser_tool`
`bazel run //apps/data_parser:parser_tool`

#### Flags (Not really implemented)

The following flags are available

- `--fps N` (The frames per second for the data parser. Currently not implemented so defaults to about 30 (33 to account for delays))
- `--colour COLOUR` (The colour for the circles either as a valid css colour, or hex code)
- `--delay N` (The time for dealy in milliseconds)
- `--radius N` (The radius for the circles)
- `--output PATH` (To define a custom output path)