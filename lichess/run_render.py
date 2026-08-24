#!/usr/bin/env python3
"""
Render entrypoint for Lichess Bot.
Runs a lightweight HTTP health-check server (required for Render Web Services)
and launches the lichess-bot process.
"""
import os
import sys
import stat
import logging
import threading
from http.server import HTTPServer, BaseHTTPRequestHandler

# Configure logging
logging.basicConfig(level=logging.INFO, format="[%(asctime)s] %(levelname)s: %(message)s")
logger = logging.getLogger("render_launcher")

class HealthCheckHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "application/json")
        self.end_headers()
        self.wfile.write(b'{"status":"ok","engine":"Chess-Engine-Cpp","bot":"online"}\n')

    def log_message(self, format, *args):
        # Suppress verbose health check access logs
        pass

def start_health_server(port: int):
    try:
        server = HTTPServer(("0.0.0.0", port), HealthCheckHandler)
        logger.info(f"Render health check server listening on port {port}")
        server.serve_forever()
    except Exception as e:
        logger.warning(f"Health check server error: {e}")

def main():
    # 1. Ensure engine binary has executable permissions
    engine_path = os.path.join(os.path.dirname(__file__), "engines", "chess-uci")
    if os.path.exists(engine_path):
        st = os.stat(engine_path)
        os.chmod(engine_path, st.st_mode | stat.S_IEXEC | stat.S_IXGRP | stat.S_IXOTH)
        logger.info(f"Set executable permissions on {engine_path}")
    else:
        logger.warning(f"Engine binary not found at {engine_path}. Build step may be required.")

    # 2. If running as a Render Web Service, start HTTP health check server in background
    port_str = os.environ.get("PORT")
    if port_str:
        try:
            port = int(port_str)
            http_thread = threading.Thread(target=start_health_server, args=(port,), daemon=True)
            http_thread.start()
            logger.info(f"Started background HTTP server on port {port} for Render keep-alive")
        except ValueError:
            logger.warning(f"Invalid PORT env var: {port_str}")

    # 3. Change directory to lichess directory so relative paths work cleanly
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)
    sys.path.insert(0, script_dir)

    # 4. Launch Lichess Bot
    logger.info("Starting lichess-bot...")
    from lib.lichess_bot import start_program
    start_program()

if __name__ == "__main__":
    main()
