// Pipeline runner for Panimator.
// Coordinates building binaries, optional video preprocessing,
// launching the web server, and streaming parsed frame data.

#include <sys/types.h>   // pid_t.
#include <sys/wait.h>    // waitpid, process status macros.
#include <unistd.h>      // fork, exec, sleep.
#include <cstdlib>       // system, exit.
#include <iostream>      // Console I/O.
#include <vector>        // Argument lists.
#include <string>        // String handling.
#include <fcntl.h>       // Reserved for future I/O redirection.

// Prints CLI usage information and exits.
void PrintHelp () {
    std::cout <<
R"(pipeline_runner – Panimator

Usage:
  pipeline_runner [options]

Options:
  --help                 Show this help and exit
  --build-files          Build video files before starting pipeline
  --verbose              Enable HTTP request logging for the web server
  --video <path>         Input video file (used with --build-files)
  --fps <number>         Frames per second override
  --colour <hex|name>    Circle colour override
  --radius <number>      Circle radius override

Example:
  pipeline_runner --build-files --video badapple.mp4 --fps 30
  pipeline_runner --verbose-server
)"
    << std::endl;
}

// Prints an error message and terminates immediately.
void Die (const std::string& message) {
    std::cerr << message << std::endl;
    std::exit(1);
}

// Builds a Bazel target and aborts on failure.
void Build (const std::string& target) {
    std::string command = "bazel build " + target;
    if (std::system(command.c_str()) != 0) {
        Die("Build failed: " + target);
    }
}

// Spawns a new process using fork/exec.
// Optionally enables verbose mode via environment variable.
pid_t Spawn (const std::vector<std::string>& arguments, bool verbose = false) {
    pid_t pid = fork();

    // Child process.
    if (pid == 0) {
        if (verbose) {
            setenv("VERBOSE", "1", 1);
        }

        // Convert std::string arguments to exec-compatible format.
        std::vector<char*> cargs;
        for (const auto& argument : arguments) {
            cargs.push_back(const_cast<char*>(argument.c_str()));
        }
        cargs.push_back(nullptr);

        execvp(cargs[0], cargs.data());

        // exec only returns on failure.
        _exit(1);
    }

    // Parent receives child PID.
    return pid;
}

int main (int argument_count, char** argument_vector) {
    // Runtime flags and optional parameter overrides.
    bool build_files = false;
    bool verbose = false;
    std::string video, frames_per_second, colour, delay, radius;

    // Parse command-line arguments.
    for (int i = 1; i < argument_count; i++) {
        std::string argument = argument_vector[i];

        if (argument == "--help") {
            PrintHelp();
            return 0;
        }

        // Trigger video preprocessing build.
        if (argument == "--build-files") {
            build_files = true;
            Build("//apps/video_processor:video_tool");
        }
        else {
            // Enable verbose server logging.
            if (argument == "--verbose") {
                verbose = true;
            }
            else {
                // Optional input video path.
                if (argument == "--video" && i + 1 < argument_count) {
                    video = argument_vector[++i];
                }
                else {
                    // Optional FPS override.
                    if (argument == "--fps" && i + 1 < argument_count) {
                        frames_per_second = argument_vector[++i];
                    }
                    else {
                        // Optional colour override.
                        if (argument == "--colour" && i + 1 < argument_count) {
                            colour = argument_vector[++i];
                        }
                        else {
                            // Optional radius override.
                            if (argument == "--radius" && i + 1 < argument_count) {
                                radius = argument_vector[++i];
                            }
                            else {
                                PrintHelp();
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }

    // Run video conversion pipeline if requested.
    if (build_files) {
        std::vector<std::string> converter = {
            "bazel-bin/apps/video_processor/video_tool"
        };

        if (!video.empty()) {
            converter.push_back("--video");
            converter.push_back(video);
        }
        if (!frames_per_second.empty()) {
            converter.push_back("--fps");
            converter.push_back(frames_per_second);
        }

        pid_t converterPid = Spawn(converter);

        // Wait for conversion to complete and validate exit status.
        int status;
        if (waitpid(converterPid, &status, 0) == 1) {
            Die("Failed to convert video");
        }
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            Die("Video conversion failed");
        }
    }

    // Build and launch the web server.
    Build("//web:server");
    pid_t server = Spawn({
        "bazel-bin/web/server"
    }, verbose);

    // Give the server time to initialise.
    sleep(1);

    // Build and launch the frame parser.
    Build("//apps/data_parser:parser_tool");
    std::vector<std::string> parser = {
        "bazel-bin/apps/data_parser/parser_tool"
    };

    if (!frames_per_second.empty()) {
        parser.push_back("--fps");
        parser.push_back(frames_per_second);
    }
    if (!colour.empty()) {
        parser.push_back("--colour");
        parser.push_back(colour);
    }
    if (!radius.empty()) {
        parser.push_back("--radius");
        parser.push_back(radius);
    }

    pid_t parserPid = Spawn(parser);

    // Block until child processes terminate.
    int status;
    waitpid(server, &status, 0);
    waitpid(parserPid, &status, 0);
}
