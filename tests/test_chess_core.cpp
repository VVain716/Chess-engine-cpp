#include "core/Types.hpp"
#include "core/Board.hpp"
#include "core/MoveGen.hpp"
#include "core/Notation.hpp"
#include "core/Eval.hpp"
#include "core/Search.hpp"
#include <iostream>
#include <cassert>
#include <algorithm>

using namespace chess;

void test_starting_position_legal_moves() {
    Board board;
    auto moves = MoveGen::generate_legal_moves(board);
    std::cout << "[TEST] Starting position legal moves count: " << moves.size() << std::endl;
    // Standard starting position has exactly 20 legal moves (16 pawn pushes + 4 knight moves)
    assert(moves.size() == 20);
    assert(!MoveGen::is_in_check(board, Color::White));
    assert(!MoveGen::is_in_check(board, Color::Black));
    assert(!MoveGen::is_checkmate(board));
    assert(!MoveGen::is_stalemate(board));
}

void test_get_legal_moves_api() {
    Board board;
    // 1. All legal moves for the whole board
    auto all_moves = MoveGen::get_legal_moves(board);
    assert(all_moves.size() == 20);

    // 2. Legal moves for knight on b1 (row 7, col 1 -> square 57)
    Square b1 = make_square(7, 1);
    auto b1_moves = MoveGen::get_legal_moves(board, b1);
    assert(b1_moves.size() == 2); // Na3 and Nc3

    // 3. Legal moves via (row, col) overload for e2 pawn (row 6, col 4)
    auto e2_moves = MoveGen::get_legal_moves(board, 6, 4);
    assert(e2_moves.size() == 2); // e3 and e4

    // 4. Square with no pieces or enemy pieces on white's turn
    auto empty_sq_moves = MoveGen::get_legal_moves(board, 4, 4);
    assert(empty_sq_moves.empty());

    auto enemy_sq_moves = MoveGen::get_legal_moves(board, 1, 4); // Black pawn on e7
    assert(enemy_sq_moves.empty());

    std::cout << "[TEST] get_legal_moves convenience API passed." << std::endl;
}

void test_eval_piece_square_tables() {
    // Test Middlegame Pawn values
    Square e2 = make_square(6, 4);
    Square e4 = make_square(4, 4);
    Square e7 = make_square(1, 4);

    assert(Eval::Middlegame::PawnValue(e2) == Eval::Middlegame::pawn_table[6][4]);
    assert(Eval::Middlegame::PawnValue(e4) == 120); // Centralized pawn
    assert(Eval::Middlegame::PawnValue(e7) == 150); // Rank 7 pawn

    // Test Middlegame Knight values
    Square b1 = make_square(7, 1);
    Square d4 = make_square(4, 3);
    assert(Eval::Middlegame::KnightValue(b1) == Eval::Middlegame::knight_table[7][1]);
    assert(Eval::Middlegame::KnightValue(d4) == 345); // Outpost

    // Test Endgame Pawn values
    assert(Eval::Endgame::PawnValue(e4) == 135);
    assert(Eval::Endgame::PawnValue(e7) == 200); // 7th rank near promotion

    // Test Endgame King centralization
    Square e1 = make_square(7, 4);
    assert(Eval::Endgame::KingValue(e4) > Eval::Endgame::KingValue(e1));

    // Test overall initial board evaluate (should be symmetric 0)
    Board start_board;
    int eval_score = Eval::evaluate(start_board);
    assert(eval_score == 0);

    std::cout << "[TEST] Eval Piece-Square Tables and Evaluation passed." << std::endl;
}

void test_scholars_mate() {
    Board board;
    MoveHistory history;

    // 1. e4 e5
    Move e4(make_square(6, 4), make_square(4, 4));
    history.record_move(board, e4);
    board.make_move(e4);

    Move e5(make_square(1, 4), make_square(3, 4));
    history.record_move(board, e5);
    board.make_move(e5);

    // 2. Qh5 Nc6
    Move Qh5(make_square(7, 3), make_square(3, 7));
    history.record_move(board, Qh5);
    board.make_move(Qh5);

    Move Nc6(make_square(0, 1), make_square(2, 2));
    history.record_move(board, Nc6);
    board.make_move(Nc6);

    // 3. Bc4 Nf6
    Move Bc4(make_square(7, 5), make_square(4, 2));
    history.record_move(board, Bc4);
    board.make_move(Bc4);

    Move Nf6(make_square(0, 6), make_square(2, 5));
    history.record_move(board, Nf6);
    board.make_move(Nf6);

    // 4. Qxf7#
    Move Qxf7(make_square(3, 7), make_square(1, 5));
    history.record_move(board, Qxf7);
    board.make_move(Qxf7);

    assert(MoveGen::is_in_check(board, Color::Black));
    assert(MoveGen::is_checkmate(board));
    history.record_white_win();

    std::cout << "[TEST] Scholar's Mate notation:\n" << history.get_notation() << std::endl;
}

void test_castling() {
    Board board;
    board.clear();

    // Setup: White King at e1 (7,4), White Rook at h1 (7,7), Black King at e8 (0,4)
    board.set_piece(make_square(7, 4), Piece::WhiteKing);
    board.set_piece(make_square(7, 7), Piece::WhiteRook);
    board.set_piece(make_square(0, 4), Piece::BlackKing);
    board.castling_rights().white_kingside = true;
    board.update_king_squares();

    auto moves = MoveGen::generate_legal_moves(board, make_square(7, 4));
    bool found_castle = false;
    for (const auto& m : moves) {
        if (m.type == MoveType::CastlingKingside) {
            found_castle = true;
            break;
        }
    }
    assert(found_castle);

    // Make castle move
    Move castle_move(make_square(7, 4), make_square(7, 6), MoveType::CastlingKingside);
    board.make_move(castle_move);

    assert(board.get_piece(make_square(7, 6)) == Piece::WhiteKing);
    assert(board.get_piece(make_square(7, 5)) == Piece::WhiteRook);
    assert(board.get_piece(make_square(7, 4)) == Piece::None);
    assert(board.get_piece(make_square(7, 7)) == Piece::None);

    std::cout << "[TEST] Castling test passed successfully." << std::endl;
}

void test_en_passant() {
    Board board;
    board.clear();

    // Setup: White pawn on e5 (3, 4), Black pawn on d7 (1, 3), White king on a1, Black king on h8
    board.set_piece(make_square(3, 4), Piece::WhitePawn);
    board.set_piece(make_square(1, 3), Piece::BlackPawn);
    board.set_piece(make_square(7, 0), Piece::WhiteKing);
    board.set_piece(make_square(0, 7), Piece::BlackKing);
    board.set_side_to_move(Color::Black);
    board.update_king_squares();

    // Black plays d7-d5
    Move d5(make_square(1, 3), make_square(3, 3));
    board.make_move(d5);

    assert(board.en_passant_square() == make_square(2, 3)); // d6

    // White can capture en passant (e5xd6)
    auto moves = MoveGen::generate_legal_moves(board, make_square(3, 4));
    bool found_ep = false;
    for (const auto& m : moves) {
        if (m.type == MoveType::EnPassant && m.to == make_square(2, 3)) {
            found_ep = true;
            break;
        }
    }
    assert(found_ep);

    Move ep_move(make_square(3, 4), make_square(2, 3), MoveType::EnPassant);
    board.make_move(ep_move);

    assert(board.get_piece(make_square(2, 3)) == Piece::WhitePawn);
    assert(board.get_piece(make_square(3, 4)) == Piece::None);
    assert(board.get_piece(make_square(3, 3)) == Piece::None); // Black pawn captured

    std::cout << "[TEST] En Passant test passed successfully." << std::endl;
}

void test_minimax_search() {
    // 1. Test Best Move from Start Position
    Board board;
    SearchResult result = Search::search(board, 3);
    assert(result.best_move.is_valid());
    std::cout << "[TEST] Start position best move found: " << Notation::format_move(board, result.best_move)
              << " with eval " << result.score << " (nodes: " << result.nodes << ")" << std::endl;

    // 2. Test Mate-in-1 Detection (Scholar's Mate setup)
    // 1. e4 e5 2. Qh5 Nc6 3. Bc4 Nf6 -> White has mate in 1 with Qxf7#
    Board mate_board;
    mate_board.make_move(Move(make_square(6, 4), make_square(4, 4))); // e4
    mate_board.make_move(Move(make_square(1, 4), make_square(3, 4))); // e5
    mate_board.make_move(Move(make_square(7, 3), make_square(3, 7))); // Qh5
    mate_board.make_move(Move(make_square(0, 1), make_square(2, 2))); // Nc6
    mate_board.make_move(Move(make_square(7, 5), make_square(4, 2))); // Bc4
    mate_board.make_move(Move(make_square(0, 6), make_square(2, 5))); // Nf6

    Move mate_move = Search::get_best_move(mate_board, 2);
    // Best move should be Qh5xf7 (from square 3,7 to square 1,5)
    assert(mate_move.from == make_square(3, 7));
    assert(mate_move.to == make_square(1, 5));
    std::cout << "[TEST] Mate-in-1 found successfully: " << Notation::format_move(mate_board, mate_move) << std::endl;

    // 3. Test Hanging Queen Capture
    Board tactics_board;
    tactics_board.clear();
    tactics_board.set_piece(make_square(7, 4), Piece::WhiteKing);
    tactics_board.set_piece(make_square(7, 0), Piece::WhiteRook);
    tactics_board.set_piece(make_square(2, 0), Piece::BlackQueen); // Hanging queen on a6
    tactics_board.set_piece(make_square(0, 4), Piece::BlackKing);
    tactics_board.update_king_squares();

    Move capture_move = Search::get_best_move(tactics_board, 2);
    // White rook on a1 (7,0) captures black queen on a6 (2,0)
    assert(capture_move.from == make_square(7, 0));
    assert(capture_move.to == make_square(2, 0));
    std::cout << "[TEST] Hanging piece capture found: " << Notation::format_move(tactics_board, capture_move) << std::endl;
}

void test_insufficient_material() {
    Board board;
    board.clear();
    board.set_piece(make_square(7, 4), Piece::WhiteKing);
    board.set_piece(make_square(0, 4), Piece::BlackKing);
    board.update_king_squares();

    // King vs King is a draw by insufficient material
    assert(MoveGen::is_insufficient_material(board));
    assert(MoveGen::is_draw(board));
    std::cout << "[TEST] King vs King draw detected successfully." << std::endl;

    // King + Knight vs King is also a draw
    board.set_piece(make_square(5, 5), Piece::WhiteKnight);
    assert(MoveGen::is_insufficient_material(board));
    assert(MoveGen::is_draw(board));

    // King + Pawn vs King is NOT insufficient material
    board.set_piece(make_square(5, 5), Piece::WhitePawn);
    assert(!MoveGen::is_insufficient_material(board));
    std::cout << "[TEST] Insufficient material tests passed successfully." << std::endl;
}

int main() {
    std::cout << "Running Chess Core Unit Tests..." << std::endl;
    test_starting_position_legal_moves();
    test_get_legal_moves_api();
    test_eval_piece_square_tables();
    test_scholars_mate();
    test_castling();
    test_en_passant();
    test_minimax_search();
    test_insufficient_material();
    std::cout << "All Unit Tests Passed!" << std::endl;
    return 0;
}
