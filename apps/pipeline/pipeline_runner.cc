// TODO(Austin): Add comments.

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

void die (const std::string& msg) {
    std::cerr << msg << std::endl;
    std::exit(1);
}

void build (const std::string& target) {
    std::string cmd = "bazel build " + target;
    if (std::system(cmd.c_str()) != 0) {
        die("Build failed: " + target);
    }
}

pid_t spawn (const std::vector<std::string>& args) {
    pid_t pid = fork();
    if (pid == 0) {
        std::vector<char*> cargs;
        for (const auto& s : args) {
            cargs.push_back(const_cast<char*>(s.c_str()));
        }
        cargs.push_back(nullptr);

        execvp(cargs[0], cargs.data());
        _exit(1);
    }
    return pid;
}

int main (int argc, char** argv) {
    bool buildFiles = false;
    std::string video, fps, colour;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--build-files") {
            buildFiles = true;
        }
        else {
            if (arg == "--video" && i + 1 < argc) {
                video = argv[++i];
            }
            else {
                if (arg == "--fps" && i + 1 < argc) {
                    fps = argv[++i];
                }
                else {
                    if (arg == "--colour" && i + 1 < argc) {
                        colour = argv[++i];
                    }
                }
            }
        }
    }

    build("//web:server");
    build("//apps/data_parser:parser_tool");

    if (buildFiles) {
        build("//apps/video_processor:video_tool");
    }

    pid_t server = spawn({
        "bazel-bin/web/server"
    });

    if (buildFiles) {
        std::vector<std::string> gen = {
            "bazel-bin/apps/video_processor/video_tool"
        };
        if (!video.empty()) {
            gen.push_back("--video=" + video);
        }
        if (!fps.empty()) {
            gen.push_back("--fps=" + fps);
        }
        spawn(gen);
    }

    std::vector<std::string> parser = {"bazel-bin/apps/data_parser/parser_tool"};
    if (!fps.empty()) {
        parser.push_back("--fps=" + fps);
    }
    if (!colour.empty()) {
        parser.push_back("--colour=" + colour);
    }

    pid_t parserPid = spawn(parser);

    int status;
    waitpid(server, &status, 0);
    waitpid(parserPid, &status, 0);
}
