# =============================================================
# Stage 1: Compile C++ Chess Engine
# =============================================================
FROM gcc:13-bookworm AS builder

WORKDIR /build

# Copy engine C++ source and headers
COPY include/ ./include/
COPY src/core/ ./src/core/
COPY src/uci.cpp ./src/uci.cpp

# Compile standalone chess-uci executable with C++20 and -O3
RUN g++ -std=c++20 -O3 -Wall -Wextra -Iinclude \
    src/core/Types.cpp \
    src/core/Board.cpp \
    src/core/MoveGen.cpp \
    src/core/Notation.cpp \
    src/core/Eval.cpp \
    src/core/Search.cpp \
    src/uci.cpp \
    -o chess-uci

# =============================================================
# Stage 2: Lichess Bot Python Runner
# =============================================================
FROM python:3.11-slim-bookworm

ENV PYTHONDONTWRITEBYTECODE=1
ENV PYTHONUNBUFFERED=1
ENV LICHESS_BOT_DOCKER="true"

WORKDIR /app

# Copy lichess bot source files
COPY lichess/ /app/lichess/

# Copy compiled chess-uci engine from builder stage
COPY --from=builder /build/chess-uci /app/lichess/engines/chess-uci

# Install Python dependencies and ensure execution permissions
RUN pip install --no-cache-dir -r /app/lichess/requirements.txt && \
    chmod +x /app/lichess/engines/chess-uci /app/lichess/run_render.py

WORKDIR /app/lichess

# Render Web Service default port
EXPOSE 10000

CMD ["python3", "run_render.py"]
