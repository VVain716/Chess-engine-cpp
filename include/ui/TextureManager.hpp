#pragma once

#include "core/Types.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <array>
#include <string>
#include <filesystem>

namespace chess::ui {

enum class ResultType {
    None,
    WhiteWin,
    BlackWin,
    Draw
};

class TextureManager {
private:
    std::array<SDL_Texture*, 13> piece_textures_{};
    SDL_Texture* white_win_texture_ = nullptr;
    SDL_Texture* black_win_texture_ = nullptr;
    SDL_Texture* draw_texture_      = nullptr;

    std::filesystem::path resolve_assets_path(const std::string& custom_path = "");

public:
    TextureManager() = default;
    ~TextureManager();

    // Non-copyable, movable
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager(TextureManager&& other) noexcept;
    TextureManager& operator=(TextureManager&& other) noexcept;

    bool load_all_textures(SDL_Renderer* renderer, const std::string& base_path = "");
    void destroy();

    [[nodiscard]] SDL_Texture* get_piece_texture(Piece piece) const;
    [[nodiscard]] SDL_Texture* get_result_texture(ResultType result) const;
};

} // namespace chess::ui
