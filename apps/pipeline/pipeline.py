# TODO(Austin): Add comments.
import os
import subprocess
import sys
import time
import signal

def get_runfile_path(path):
    runfiles_dir = os.environ.get("RUNFILES_DIR")
    if runfiles_dir:
        return os.path.join(runfiles_dir, path)
    manifest = os.environ.get("RUNFILES_MANIFEST_FILE")
    if manifest:
        with open(manifest) as f:
            for line in f:
                if line.startswith(path + " "):
                    return line.strip().split(" ", 1)[1]
    return path

video_tool = get_runfile_path("apps/video_processor/video_tool")
video_path = get_runfile_path("data/video.mp4")
frames_path = get_runfile_path("data/frames")
ascii_path = get_runfile_path("data/txt_files")

subprocess.run([
    video_tool,
    "--video", video_path,
    "--frames", frames_path,
    "--ascii", ascii_path,
    "--fps", "30"
], check=True)


def run_video_tool(fps=30):
    print(f"Running video tool: {video_tool}")
    cmd = [
        video_tool,
        "--video", video_path,
        "--frames", frames_path,
        "--ascii", ascii_path,
        "--fps", str(fps),
    ]
    subprocess.run(cmd, check=True)


def run_parser(output_json="data/circles.json", fps_ms=33, color="orange"):
    parser_tool = get_runfile_path("apps/data_parser/parser_tool")
    print(f"Running parser tool: {parser_tool}")
    cmd = [
        parser_tool,
        "--output", output_json,
        "--delay", str(fps_ms),
        "--color", color
    ]
    subprocess.run(cmd, check=True)

def run_server(port=8000):
    server_script = get_runfile_path("web/server")
    print(f"Starting server: {server_script} on port {port}")
    return subprocess.Popen([sys.executable, server_script, str(port)])

def main():
    try:
        run_video_tool()

        run_parser()

        server_proc = run_server()

        print("Pipeline running! Press Ctrl+C to stop.")
        server_proc.wait()

    except KeyboardInterrupt:
        print("\nStopping pipeline...")
        if server_proc:
            server_proc.send_signal(signal.SIGINT)
            server_proc.wait()
        sys.exit(0)

    except subprocess.CalledProcessError as e:
        print(f"❌ Command failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
