# Lightweight HTTP server for Panimator.
# Serves static web assets from the script directory and
# optionally suppresses request logging for cleaner output.

from http.server import SimpleHTTPRequestHandler
from socketserver import TCPServer
import os
import sys

# Port the web server listens on.
PORT = 8000

# Resolve and switch to the directory containing this script
# so static files are served relative to the web root.
web_dir = os.path.abspath(os.path.dirname(__file__))
os.chdir(web_dir)

# Enable verbose logging when launched with VERBOSE=1.
VERBOSE = os.getenv("VERBOSE") == "1"

# Custom request handler that suppresses access logs.
class QuietHTTPRequestHandler(SimpleHTTPRequestHandler):
    def log_message(self, format, *arguments):
        # Intentionally ignore HTTP request log output.
        pass

# Select handler based on verbosity preference.
Handler = SimpleHTTPRequestHandler if VERBOSE else QuietHTTPRequestHandler

# TCP server variant that allows immediate socket reuse
# to avoid "address already in use" errors on restart.
class ReusableTCPServer(TCPServer):
    allow_reuse_address = True

httpd = None

try:
    # Create and start the HTTP server.
    httpd = ReusableTCPServer(("", PORT), Handler)
    print(f"Serving web at http://localhost:{PORT}")
    print("Press Ctrl+C to stop the server.")
    httpd.serve_forever()

except KeyboardInterrupt:
    # Graceful shutdown on user interrupt.
    print("\nShutting down server...")

except OSError as error:
    # Handle startup failures such as port binding issues.
    print(f"Failed to start server: {error}")

finally:
    # Ensure the socket is released on exit.
    if httpd:
        httpd.server_close()
        print("Server closed cleanly.")
