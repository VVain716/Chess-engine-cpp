#include "ui/Heatmap.hpp"
#include "core/Eval.hpp"
#include <iostream>

int main() {
    cv::Mat pawn_table_middlegame = chess::ui::createHeatmap(chess::Eval::Middlegame::pawn_table);
    cv::Mat knight_table_middlegame = chess::ui::createHeatmap(chess::Eval::Middlegame::knight_table);
    cv::Mat bishop_table_middlegame = chess::ui::createHeatmap(chess::Eval::Middlegame::bishop_table);
    cv::Mat rook_table_middlegame = chess::ui::createHeatmap(chess::Eval::Middlegame::rook_table);
    cv::Mat queen_table_middlegame = chess::ui::createHeatmap(chess::Eval::Middlegame::queen_table);
    cv::Mat king_table_middlegame = chess::ui::createHeatmap(chess::Eval::Middlegame::king_table);

    chess::ui::saveHeatmap(pawn_table_middlegame, "heatmaps/middlegame/pawn_table.png");
    chess::ui::saveHeatmap(knight_table_middlegame, "heatmaps/middlegame/knight_table.png");
    chess::ui::saveHeatmap(bishop_table_middlegame, "heatmaps/middlegame/bishop_table.png");
    chess::ui::saveHeatmap(rook_table_middlegame, "heatmaps/middlegame/rook_table.png");
    chess::ui::saveHeatmap(queen_table_middlegame, "heatmaps/middlegame/queen_table.png");
    chess::ui::saveHeatmap(king_table_middlegame, "heatmaps/middlegame/king_table.png");

    cv::Mat pawn_table_endgame = chess::ui::createHeatmap(chess::Eval::Endgame::pawn_table);
    cv::Mat knight_table_endgame = chess::ui::createHeatmap(chess::Eval::Endgame::knight_table);
    cv::Mat bishop_table_endgame = chess::ui::createHeatmap(chess::Eval::Endgame::bishop_table);
    cv::Mat rook_table_endgame = chess::ui::createHeatmap(chess::Eval::Endgame::rook_table);
    cv::Mat queen_table_endgame = chess::ui::createHeatmap(chess::Eval::Endgame::queen_table);
    cv::Mat king_table_endgame = chess::ui::createHeatmap(chess::Eval::Endgame::king_table);

    chess::ui::saveHeatmap(pawn_table_endgame, "heatmaps/endgame/pawn_table.png");
    chess::ui::saveHeatmap(knight_table_endgame, "heatmaps/endgame/knight_table.png");
    chess::ui::saveHeatmap(bishop_table_endgame, "heatmaps/endgame/bishop_table.png");
    chess::ui::saveHeatmap(rook_table_endgame, "heatmaps/endgame/rook_table.png");
    chess::ui::saveHeatmap(queen_table_endgame, "heatmaps/endgame/queen_table.png");
    chess::ui::saveHeatmap(king_table_endgame, "heatmaps/endgame/king_table.png");

    return 0;
}
