# Chess Engine (C++)

A modern C++20 chess engine and interactive GUI built using SDL3 and SDL3_image.

## Architecture

The project is cleanly decoupled into modular layers:

- **`chess_core` (`include/core/`, `src/core/`)**: Pure C++ chess logic with zero GUI dependencies.
  - `Types`: Strongly typed enums (`Piece`, `PieceType`, `Color`, `Square`).
  - `Board`: Board representation, piece placements, state history, and castling rights.
  - `Move`: Move representations (regular moves, castling, promotions, en passant).
  - `MoveGen`: Move generation, ray casting, check/checkmate/stalemate detection.
  - `Notation`: Move notation recorder and algebraic coordinate conversions.
  - `Eval`: Tapered evaluation function with 2D Piece-Square Tables (PSTs).
  - `Search`: Alpha-Beta minimax search with quiescence and move ordering.
- **`chess_ui` (`include/ui/`, `src/ui/`)**: Graphics and rendering layer.
  - `TextureManager`: RAII resource manager for piece textures and result banners.
  - `ChessRenderer`: Responsive board rendering, highlights, and check indicators.
  - `Animation`: Smooth piece movement animations.
- **`chess-engine` (`src/main.cpp`)**: Interactive 2-player local chess GUI.
- **`play-computer` (`src/play-computer.cpp`)**: Play against the AI engine with a live evaluation sidebar and advantage graph.
- **`test_chess_core` (`tests/test_chess_core.cpp`)**: Automated unit test suite.

---

## Prerequisites & Installation

### macOS (Homebrew)
```bash
brew install cmake make sdl3 sdl3_image
```

---

## Build & Run

### 1. Build
```bash
cmake -S . -B build
cmake --build build
```

### 2. Run Tests
```bash
./build/test_chess_core
```

### 3. Play vs Computer (with Live Eval Graph)
```bash
./build/play-computer
```

### 4. Play Local 2-Player Game
```bash
./build/chess-engine
```




## Play on Lichess 
Search for **VVainBot** on Lichess to play against the bot directly!

---

## Deploying to Render

You can host the Lichess bot 24/7 on [Render](https://render.com) using either Docker or Native environment:

### Option 1: Docker Deployment (Recommended)
1. In Render, click **New +** -> **Web Service** (or **Background Worker**).
2. Connect your GitHub repository.
3. Select **Docker** as the Runtime.
4. Under **Environment Variables**, add:
   - `LICHESS_BOT_TOKEN`: `<Your Lichess OAuth Token>`
5. Click **Deploy Web Service**.

### Option 2: Native Build
1. In Render, create a new **Web Service**.
2. Set **Build Command**: `./build.sh`
3. Set **Start Command**: `./start.sh`
4. Add environment variable `LICHESS_BOT_TOKEN`.
5. Click **Deploy**.

