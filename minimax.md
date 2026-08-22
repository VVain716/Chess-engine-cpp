# Minimax Search Optimization Guide

This document outlines the architecture, algorithms, and optimization strategies for implementing an efficient chess search engine.

---

## 1. Core Search Architecture: Negamax with Alpha-Beta Pruning

In chess, **Negamax** simplifies the standard minimax implementation using the zero-sum property:
$$\text{score}(\text{position}) = -\text{score}(\text{opponent\_position})$$

**Alpha ($\alpha$)** represents the minimum score the maximizing player is assured of, while **Beta ($\beta$)** represents the maximum score the opponent will allow. When a move produces a score $\ge \beta$, the opponent would avoid allowing this position, triggering a **beta-cutoff (fail-high)**.

### Pseudocode

```cpp
int negamax(Board& board, int depth, int alpha, int beta) {
    if (depth == 0) {
        return quiescence(board, alpha, beta);
    }

    auto moves = MoveGen::generate_legal_moves(board);
    if (moves.empty()) {
        if (MoveGen::is_in_check(board, board.side_to_move())) {
            return -CHECKMATE_SCORE + ply; // Prefer faster checkmates
        }
        return 0; // Stalemate
    }

    order_moves(moves, board);

    for (const auto& move : moves) {
        board.make_move(move);
        int score = -negamax(board, depth - 1, -beta, -alpha);
        board.undo_move(move);

        if (score >= beta) {
            return beta; // Beta cutoff
        }
        if (score > alpha) {
            alpha = score;
        }
    }
    return alpha;
}
```

---

## 2. Move Ordering (The #1 Performance Factor)

Alpha-Beta pruning efficiency depends directly on searching the best moves first:
- **Worst Case** (poor ordering): $O(b^d)$ nodes evaluated.
- **Best Case** (optimal ordering): $O(\sqrt{b^d}) = O(b^{d/2})$ nodes evaluated (equivalent to doubling search depth for the same compute).

### Ordering Priority
1. **PV / TT Hash Move**: The best move recorded from the Transposition Table or previous Iterative Deepening ply.
2. **Winning & Equal Captures (MVV-LVA)**: *Most Valuable Victim - Least Valuable Attacker*.
   - Example: Pawn takes Queen ($P \times Q$) $>$ Knight takes Queen ($N \times Q$) $>$ Queen takes Pawn ($Q \times P$).
3. **Killer Moves**: Quiet (non-capture) moves that caused a beta cutoff at the same ply in sibling branches (typically keep 2 killer move slots per ply).
4. **History Heuristic**: A table tracking how often quiet moves cause beta cutoffs throughout the search tree.
5. **Losing Captures & Remaining Quiet Moves**.

---

## 3. Quiescence Search (Preventing the Horizon Effect)

Stopping search strictly at `depth == 0` causes the **horizon effect**—evaluating positions mid-tactical exchange (e.g. evaluating immediately after a Queen captures a Pawn without seeing that a Bishop recaptures the Queen on the next ply).

**Quiescence Search** extends the search at leaf nodes by examining only noisy moves (captures and promotions) until a tactically quiet position is reached.

### Pseudocode

```cpp
int quiescence(Board& board, int alpha, int beta) {
    int stand_pat = Eval::evaluate(board);
    if (stand_pat >= beta) return beta;
    if (alpha < stand_pat) alpha = stand_pat;

    auto captures = generate_captures(board);
    order_captures_mvv_lva(captures, board);

    for (const auto& move : captures) {
        board.make_move(move);
        int score = -quiescence(board, -beta, -alpha);
        board.undo_move(move);

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}
```

---

## 4. Transposition Tables (TT) & Zobrist Hashing

Different move paths frequently transpose into the exact same board state (e.g., `1. d4 Nf6 2. c4` vs `1. c4 Nf6 2. d4`).

1. **Zobrist Hashing**:
   - Pre-generate a 64-bit random number for each `(Piece, Square)` combination, castling rights, and en passant files.
   - Maintain the hash incrementally via bitwise XOR (`hash ^= zobrist_table[piece][sq]`) in $O(1)$ time during `make_move` / `undo_move`.
2. **TT Entry Structure**:
   ```cpp
   struct TTEntry {
       uint64_t zobrist_key;
       int depth;
       int score;
       enum class Flag : uint8_t { Exact, LowerBound, UpperBound } flag;
       Move best_move;
   };
   ```
3. If `entry.depth >= current_depth` and the hash matches, return or bound the search score immediately.

---

## 5. Iterative Deepening & Time Management

Instead of searching depth $N$ directly:
1. Search `depth = 1` $\rightarrow$ obtain Principal Variation (PV).
2. Search `depth = 2` $\rightarrow$ use PV move from depth 1 for first-move ordering.
3. Search `depth = 3 \dots N`.

### Advantages:
- Negligible overhead: Due to exponential branching ($b \approx 30$), searching depths $1 \dots d-1$ takes $< 10\%$ of depth $d$.
- Immediate best-move availability: Allows graceful search termination when time limits expire.
- Drastically improves Alpha-Beta pruning at deeper plies.

---

## 6. Advanced Pruning & Reductions

Once the baseline search is established:

1. **Null Move Pruning (NMP)**:
   - If the side to move "passes" their turn (null move) and the search still yields $\ge \beta$, the position is so strong that searching further is unnecessary.
   - Guard conditions: Do not apply if in check, in pure pawn endgames (zugzwang danger), or at shallow depth.
2. **Late Move Reductions (LMR)**:
   - Quiet moves ordered late in the move list are unlikely to be optimal.
   - Search late quiet moves with reduced depth (`depth - 2` or `depth - 1`). If the reduced search beats $\alpha$, re-search at full depth.
3. **Reverse Futility Pruning (Static Null Move Pruning)**:
   - At pre-frontier nodes (`depth == 1` or `2`), if `eval - margin >= beta`, return beta cutoff directly.

---

## 7. Implementation Milestones

| Stage | Techniques | Target Search Depth |
|---|---|---|
| **Phase 1: Baseline** | Negamax + Alpha-Beta Pruning + In-place `make_move`/`undo_move` | Depth 4–5 |
| **Phase 2: Tactical Stability** | Quiescence Search + MVV-LVA Capture Ordering | Depth 5–6 (tactically sound) |
| **Phase 3: Deepening & Memory** | Iterative Deepening + Transposition Table (Zobrist Hash) | Depth 7–8 |
| **Phase 4: Advanced Heuristics** | Killer Moves + History Heuristic + Null Move Pruning + LMR | Depth 9–11+ |
