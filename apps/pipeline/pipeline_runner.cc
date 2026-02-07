// TODO(Austin): Add comments.

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fcntl.h>

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
    std::string video, fps, colour;

    for (int i = 1; i < argument_count; i++) {
        std::string argument;
        argument = argument_vector[i];
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
                        fps = argument_vector[++i];
                    }
                    else {
                        if (argument == "--colour" && i + 1 < argument_count) {
                            colour = argument_vector[++i];
                        }
                    }
                }
            }
        }
    }

    Build("//web:server");
    Build("//apps/data_parser:parser_tool");

    pid_t server = Spawn({
        "bazel-bin/web/server"
    }, verbose);

    sleep(1);

    if (build_files) {
        std::vector<std::string> converter = {
            "bazel-bin/apps/video_processor/video_tool"
        };
        if (!video.empty()) {
            converter.push_back("--video=" + video);
        }
        if (!fps.empty()) {
            converter.push_back("--fps=" + fps);
        }
        Spawn(converter);
    }

    std::vector<std::string> parser = {
        "bazel-bin/apps/data_parser/parser_tool"
    };
    if (!fps.empty()) {
        parser.push_back("--fps=" + fps);
    }
    if (!colour.empty()) {
        parser.push_back("--colour=" + colour);
    }

    pid_t parserPid = Spawn(parser);

    int status;
    waitpid(server, &status, 0);
    waitpid(parserPid, &status, 0);
}
