# TODO(Austin): Add comments.
from http.server import SimpleHTTPRequestHandler
from socketserver import TCPServer
import os
import sys

PORT = 8000
web_dir = os.path.abspath(os.path.dirname(__file__))
os.chdir(web_dir)

VERBOSE = os.getenv("VERBOSE") == "1"

class QuietHTTPRequestHandler(SimpleHTTPRequestHandler):
    def log_message(self, format, *arguments):
        pass

Handler = SimpleHTTPRequestHandler if VERBOSE else QuietHTTPRequestHandler

class ReusableTCPServer(TCPServer):
    allow_reuse_address = True

httpd = None

try:
    httpd = ReusableTCPServer(("", PORT), Handler)
    print(f"Serving web at http://localhost:{PORT}")
    print("Press Ctrl+C to stop the server.")
    httpd.serve_forever()

except KeyboardInterrupt:
    print("\nShutting down server...")

except OSError as error:
    print(f"Failed to start server: {error}")

finally:
    if httpd:
        httpd.server_close()
        print("Server closed cleanly.")
