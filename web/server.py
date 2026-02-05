# TODO(Austin): Add comments.
from http.server import SimpleHTTPRequestHandler
from socketserver import TCPServer
import os
import sys

PORT = 8000
web_dir = os.path.abspath(os.path.dirname(__file__))
os.chdir(web_dir)

class ReusableTCPServer(TCPServer):
    allow_reuse_address = True

Handler = SimpleHTTPRequestHandler
httpd = None

try:
    httpd = ReusableTCPServer(("", PORT), Handler)
    print(f"Serving web at http://localhost:{PORT}")
    print("Press Ctrl+C to stop the server.")
    httpd.serve_forever()
except KeyboardInterrupt:
    print("\nShutting down server...")
except OSError as e:
    print(f"Failed to start server: {e}")
finally:
    if httpd:
        httpd.server_close()
        print("Server closed cleanly.")
