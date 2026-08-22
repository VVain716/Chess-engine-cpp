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
- **`chess_ui` (`include/ui/`, `src/ui/`)**: Graphics and rendering layer.
  - `TextureManager`: RAII resource manager for piece textures and result banners.
  - `ChessRenderer`: Responsive board rendering, highlights, and check indicators.
  - `Animation`: Smooth piece movement animations.
- **`chess-engine` (`src/main.cpp`)**: Interactive GUI application loop.
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

### 3. Launch the Game
```bash
./build/chess-engine
```