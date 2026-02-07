// TODO(Austin): Add comments.

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fcntl.h>

void PrintHelp () {
    std::cout <<
R"(pipeline_runner – Panimator

Usage:
  pipeline_runner [options]

Options:
  --help                 Show this help and exit
  --build-files          Build video files before starting pipeline
  --verbose-server       Enable HTTP request logging for the web server
  --video <path>         Input video file (used with --build-files)
  --fps <number>         Frames per second override
  --colour <hex|name>    Circle colour override
  --delay <number>       Circle delay override
  --radius <number>      Circle radius override

Example:
  pipeline_runner --build-files --video badapple.mp4 --fps 30
  pipeline_runner --verbose-server
)"
    << std::endl;
}


void Die (const std::string& message) {
    std::cerr << message << std::endl;
    std::exit(1);
}

void Build (const std::string& target) {
    std::string command = "bazel build " + target;
    if (std::system(command.c_str()) != 0) {
        Die("Build failed: " + target);
    }
}

pid_t Spawn (const std::vector<std::string>& arguments, bool verbose = false) {
    pid_t pid = fork();
    if (pid == 0) {
        if (verbose) {
            setenv("VERBOSE", "1", 1);
        }
        std::vector<char*> cargs;
        for (const auto& argument : arguments) {
            cargs.push_back(const_cast<char*>(argument.c_str()));
        }
        cargs.push_back(nullptr);

        execvp(cargs[0], cargs.data());
        _exit(1);
    }
    return pid;
}

int main (int argument_count, char** argument_vector) {
    bool build_files = false;
    bool verbose = false;
    std::string video, frames_per_second, colour, delay, radius;

    for (int i = 1; i < argument_count; i++) {
        std::string argument = argument_vector[i];
        if (argument == "--help") {
            PrintHelp();
            return 0;
        }
        if (argument == "--build-files") {
            build_files = true;
            Build("//apps/video_processor:video_tool");
        }
        else {
            if (argument == "--verbose") {
                verbose = true;
            }
            else {
                if (argument == "--video" && i + 1 < argument_count) {
                    video = argument_vector[++i];
                }
                else {
                    if (argument == "--fps" && i + 1 < argument_count) {
                        frames_per_second = argument_vector[++i];
                    }
                    else {
                        if (argument == "--colour" && i + 1 < argument_count) {
                            colour = argument_vector[++i];
                        }
                        else {
                            if (argument == "--delay" && i + 1 < argument_count) {
                                delay = argument_vector[++i];
                            }
                            else {
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
    }

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

        int status;
        if (waitpid(converterPid, &status, 0) == 1) {
            Die("Failed to convert video");
        }
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            Die("Video conversion failed");
        }
    }

    Build("//web:server");
    pid_t server = Spawn({
        "bazel-bin/web/server"
    }, verbose);
    sleep(1);

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
    if (!delay.empty()) {
        parser.push_back("--delay");
        parser.push_back(delay);
    }
    pid_t parserPid = Spawn(parser);

    int status;
    waitpid(server, &status, 0);
    waitpid(parserPid, &status, 0);
}
