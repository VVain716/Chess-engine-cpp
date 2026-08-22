#include "ui/TextureManager.hpp"
#include <iostream>
#include <vector>

#ifndef CHESS_SOURCE_DIR
#define CHESS_SOURCE_DIR "."
#endif

namespace chess::ui {

TextureManager::~TextureManager() {
    destroy();
}

TextureManager::TextureManager(TextureManager&& other) noexcept {
    piece_textures_ = other.piece_textures_;
    white_win_texture_ = other.white_win_texture_;
    black_win_texture_ = other.black_win_texture_;
    draw_texture_ = other.draw_texture_;

    other.piece_textures_.fill(nullptr);
    other.white_win_texture_ = nullptr;
    other.black_win_texture_ = nullptr;
    other.draw_texture_ = nullptr;
}

TextureManager& TextureManager::operator=(TextureManager&& other) noexcept {
    if (this != &other) {
        destroy();
        piece_textures_ = other.piece_textures_;
        white_win_texture_ = other.white_win_texture_;
        black_win_texture_ = other.black_win_texture_;
        draw_texture_ = other.draw_texture_;

        other.piece_textures_.fill(nullptr);
        other.white_win_texture_ = nullptr;
        other.black_win_texture_ = nullptr;
        other.draw_texture_ = nullptr;
    }
    return *this;
}

void TextureManager::destroy() {
    for (auto& tex : piece_textures_) {
        if (tex) {
            SDL_DestroyTexture(tex);
            tex = nullptr;
        }
    }
    if (white_win_texture_) {
        SDL_DestroyTexture(white_win_texture_);
        white_win_texture_ = nullptr;
    }
    if (black_win_texture_) {
        SDL_DestroyTexture(black_win_texture_);
        black_win_texture_ = nullptr;
    }
    if (draw_texture_) {
        SDL_DestroyTexture(draw_texture_);
        draw_texture_ = nullptr;
    }
}

std::filesystem::path TextureManager::resolve_assets_path(const std::string& custom_path) {
    if (!custom_path.empty() && std::filesystem::exists(custom_path)) {
        return custom_path;
    }

    std::vector<std::filesystem::path> candidates = {
        std::filesystem::current_path(),
        std::filesystem::path(CHESS_SOURCE_DIR),
        std::filesystem::current_path() / "..",
        std::filesystem::current_path() / ".." / ".."
    };

    for (const auto& base : candidates) {
        if (std::filesystem::exists(base / "pieces") && std::filesystem::exists(base / "results")) {
            return base;
        }
    }

    return std::filesystem::current_path();
}

bool TextureManager::load_all_textures(SDL_Renderer* renderer, const std::string& base_path) {
    destroy();

    auto root = resolve_assets_path(base_path);
    auto white_dir = root / "pieces" / "white";
    auto black_dir = root / "pieces" / "black";
    auto results_dir = root / "results";

    auto load_tex = [&](const std::filesystem::path& path) -> SDL_Texture* {
        SDL_Texture* tex = IMG_LoadTexture(renderer, path.string().c_str());
        if (!tex) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Failed to load texture from %s: %s", path.string().c_str(), SDL_GetError());
        }
        return tex;
    };

    piece_textures_[static_cast<size_t>(Piece::WhitePawn)]   = load_tex(white_dir / "pawn.png");
    piece_textures_[static_cast<size_t>(Piece::WhiteKnight)] = load_tex(white_dir / "knight.png");
    piece_textures_[static_cast<size_t>(Piece::WhiteBishop)] = load_tex(white_dir / "bishop.png");
    piece_textures_[static_cast<size_t>(Piece::WhiteRook)]   = load_tex(white_dir / "rook.png");
    piece_textures_[static_cast<size_t>(Piece::WhiteQueen)]  = load_tex(white_dir / "queen.png");
    piece_textures_[static_cast<size_t>(Piece::WhiteKing)]   = load_tex(white_dir / "king.png");

    piece_textures_[static_cast<size_t>(Piece::BlackPawn)]   = load_tex(black_dir / "pawn.png");
    piece_textures_[static_cast<size_t>(Piece::BlackKnight)] = load_tex(black_dir / "knight.png");
    piece_textures_[static_cast<size_t>(Piece::BlackBishop)] = load_tex(black_dir / "bishop.png");
    piece_textures_[static_cast<size_t>(Piece::BlackRook)]   = load_tex(black_dir / "rook.png");
    piece_textures_[static_cast<size_t>(Piece::BlackQueen)]  = load_tex(black_dir / "queen.png");
    piece_textures_[static_cast<size_t>(Piece::BlackKing)]   = load_tex(black_dir / "king.png");

    white_win_texture_ = load_tex(results_dir / "white-win.png");
    black_win_texture_ = load_tex(results_dir / "black-win.png");
    draw_texture_      = load_tex(results_dir / "draw.png");

    return true;
}

SDL_Texture* TextureManager::get_piece_texture(Piece piece) const {
    auto idx = static_cast<size_t>(piece);
    if (idx < piece_textures_.size()) {
        return piece_textures_[idx];
    }
    return nullptr;
}

SDL_Texture* TextureManager::get_result_texture(ResultType result) const {
    switch (result) {
        case ResultType::WhiteWin: return white_win_texture_;
        case ResultType::BlackWin: return black_win_texture_;
        case ResultType::Draw:     return draw_texture_;
        default: return nullptr;
    }
}

} // namespace chess::ui
