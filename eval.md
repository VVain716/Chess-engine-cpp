# Chess Engine Evaluation Function Specification

This document details the criteria, heuristics, and mathematical formulation for a **Hand-Crafted Evaluation (HCE)** function in the chess engine.

---

## 1. Core Architecture: Tapered Evaluation

Evaluation priorities change drastically across game stages:
- **Middlegame (MG)**: King safety, piece mobility, and active tactical potential dominate.
- **Endgame (EG)**: Passed pawns, King activity, and pawn promotion potential dominate.

### Game Phase Calculation
Every non-pawn piece contributes to the total game phase:
- Knight / Bishop: `1`
- Rook: `2`
- Queen: `4`
- **Total Starting Phase** = $4 \times 1 \text{ (Knights)} + 4 \times 1 \text{ (Bishops)} + 4 \times 2 \text{ (Rooks)} + 2 \times 4 \text{ (Queens)} = 24$

$$\text{Phase} = \min\left(24, \sum \text{piece\_weights}\right)$$

### Interpolation Formula
All sub-evaluation terms compute a pair of scores: `(MG_Score, EG_Score)` in **centipawns (cp)** (where 1 pawn = 100 cp).

$$\text{Final Score} = \frac{\text{MG\_Score} \times \text{Phase} + \text{EG\_Score} \times (24 - \text{Phase})}{24}$$

---

## 2. Evaluation Criteria Breakdown

### I. Material Values (Baseline Weight)

| Piece | Middlegame (MG) | Endgame (EG) |
|---|---|---|
| **Pawn** | 100 cp | 120 cp |
| **Knight** | 320 cp | 300 cp |
| **Bishop** | 330 cp | 330 cp |
| **Rook** | 500 cp | 530 cp |
| **Queen** | 900 cp | 950 cp |

---

### II. Piece-Square Tables (PSTs / Positional Placement)
8×8 lookup tables for each piece type:

- **Pawns**:
  - *MG*: Central control (`e4`, `d4`, `e5`, `d5`).
  - *EG*: Advanced ranks (`rank 6`, `rank 7`) rewarding proximity to promotion.
- **Knights**:
  - Reward centralization (`d4`, `d5`, `e4`, `e5`, `c3`, `f3`).
  - Penalize rim/corner squares (`a1`, `h1`, etc.).
- **Bishops**:
  - Reward long diagonals and active center coverage.
  - Penalize being blocked behind friendly pawns.
- **Rooks**:
  - Reward 7th rank infiltration and central open files (`d`, `e`).
- **Queens**:
  - Discourage early exposure in opening/early MG.
  - Reward central mobility in late middlegame / endgame.
- **Kings**:
  - *MG*: Strong incentive for castled safety (`g1`, `c1` / `g8`, `c8`); heavy penalty for exposed center.
  - *EG*: Strong incentive for King centralization (`d4`, `e4`, `d5`, `e5`) to support passed pawns.

---

### III. Pawn Structure

1. **Passed Pawns**:
   - Progressive bonus increasing exponentially with rank (e.g., 5th rank: +30 cp, 6th rank: +70 cp, 7th rank: +140 cp).
   - **Protected Passed Pawn**: Extra bonus if defended by a friendly pawn.
   - **Connected Passed Pawns**: Extra bonus if adjacent to another passed pawn.
   - **King Proximity (Rule of the Square)**: Endgame bonus if friendly King is near or defending the passed pawn.
2. **Structural Weaknesses (Penalties)**:
   - **Doubled Pawns**: Two pawns of the same color on the same file (~ -15 to -25 cp).
   - **Isolated Pawns**: Pawn with no friendly pawns on adjacent files (~ -15 to -20 cp).
   - **Backward Pawns**: Pawn that cannot advance safely and lacks friendly pawn support (~ -10 to -15 cp).
   - **Pawn Islands**: Penalize fragmented pawn chains.

---

### IV. Piece Quality, Activity & Mobility

1. **Mobility**:
   - Count safe reachable destination squares per minor and major piece (bonus per non-attacked square).
2. **Bishop Pair Bonus**:
   - Holding both bishops grants control over both color complexes (~ +30 to +50 cp).
   - **Bad Bishop vs Good Bishop**: Penalize bishops blocked by their own pawns on the same color complex.
3. **Rook Placement**:
   - **Rook on Open File** (no pawns on file): +20 to +30 cp.
   - **Rook on Semi-Open File** (only opponent pawns): +10 to +15 cp.
   - **Rook on 7th Rank**: +25 to +40 cp.
   - **Connected/Doubled Rooks**: Coordination bonus.
4. **Knight Outposts**:
   - Bonus for a Knight positioned on ranks 4–6, defended by a friendly pawn and immune to enemy pawn attacks (~ +20 to +35 cp).

---

### V. King Safety & Shield (Middlegame Dominant)

1. **Pawn Shield**:
   - Evaluate the 3 squares directly in front of the castled King (e.g., `f2-g2-h2` for White kingside).
   - Apply penalties for missing, advanced, or broken shield pawns (open files adjacent to the King).
2. **Enemy King Tropism**:
   - Distance penalty based on the proximity of enemy Queens and Rooks to the friendly King.
3. **Virtual King Safety Zone**:
   - Compute the attack count on a 3×3 perimeter around the King. Apply non-linear penalty for multiple coordinated attackers.

---

### VI. Endgame Knowledge & Draw Recognition

1. **King Activity**:
   - Direct King participation in active escorting and cutting off enemy kings.
2. **Material Imbalances / Draw Scaling**:
   - Insufficient material (e.g., K+N vs K, K+B vs K, K+N vs K+N) $\rightarrow$ Force score to `0.0`.
   - Opposite-colored bishops with no other major pieces $\rightarrow$ Scale evaluation score towards a draw.

---

## 3. Recommended Code Structure

When implementing `eval`:
- **Perspective**: Evaluate from the side to move (negamax friendly) or from White's perspective.
- **Lookup Tables**: Store pre-computed 64-element arrays for piece-square tables.
- **Modularity**: Split evaluation logic into distinct helper routines:
  - `eval_material()`
  - `eval_piece_square_tables()`
  - `eval_pawns()`
  - `eval_pieces()`
  - `eval_king_safety()`
