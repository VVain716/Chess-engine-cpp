#include "core/Types.hpp"
#include "core/Board.hpp"
#include "core/MoveGen.hpp"
#include "core/Notation.hpp"
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

int main() {
    std::cout << "Running Chess Core Unit Tests..." << std::endl;
    test_starting_position_legal_moves();
    test_get_legal_moves_api();
    test_scholars_mate();
    test_castling();
    test_en_passant();
    std::cout << "All Unit Tests Passed!" << std::endl;
    return 0;
}
