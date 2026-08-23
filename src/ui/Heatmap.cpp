#include "ui/Heatmap.hpp"
#include <algorithm>
#include <climits>
#include <filesystem>
#include <iostream>
#include <string>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat chess::ui::createHeatmap(const int board[8][8], int baseValue) {
  constexpr int boardSize = 800;
  constexpr int tileSize = boardSize / 8; // 100x100 per square
  cv::Mat img(boardSize, boardSize, CV_8UC4, cv::Scalar(0, 0, 0, 255));

  int maxVal = INT_MIN;
  int minVal = INT_MAX;
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      int val = board[row][col];
      maxVal = std::max(maxVal, val);
      minVal = std::min(minVal, val);
    }
  }

  // Deduce piece base value if not explicitly provided
  if (baseValue <= 0) {
    if (maxVal >= 15000) {
      baseValue = 20000; // King
    } else if (maxVal >= 700) {
      baseValue = (maxVal > 920) ? 950 : 900; // Queen
    } else if (maxVal >= 450) {
      baseValue = (maxVal > 520) ? 530 : 500; // Rook
    } else if (maxVal >= 330) {
      baseValue = 330; // Bishop
    } else if (maxVal >= 230) {
      baseValue = (minVal < 260) ? 300 : 320; // Knight
    } else {
      baseValue = (maxVal > 160) ? 120 : 100; // Pawn
    }
  }

  // Deep Blue (lowest score penalty): RGB(30, 80, 220)
  // Neutral (exact base value):       RGB(240, 240, 245)
  // Deep Orange (highest score gain): RGB(255, 125, 0)
  constexpr double rBlue = 30.0,   gBlue = 80.0,   bBlue = 220.0;
  constexpr double rMid = 240.0,   gMid = 240.0,   bMid = 245.0;
  constexpr double rOrange = 255.0, gOrange = 125.0, bOrange = 0.0;

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      int val = board[row][col];
      int r = static_cast<int>(rMid);
      int g = static_cast<int>(gMid);
      int b = static_cast<int>(bMid);

      if (val < baseValue) {
        // Values under base value: Blue palette
        double t = (baseValue > minVal)
                       ? static_cast<double>(val - minVal) / (baseValue - minVal)
                       : 0.0;
        t = std::clamp(t, 0.0, 1.0);
        r = static_cast<int>(rBlue + t * (rMid - rBlue));
        g = static_cast<int>(gBlue + t * (gMid - gBlue));
        b = static_cast<int>(bBlue + t * (bMid - bBlue));
      } else if (val > baseValue) {
        // Values over base value: Orange palette
        double t = (maxVal > baseValue)
                       ? static_cast<double>(val - baseValue) / (maxVal - baseValue)
                       : 1.0;
        t = std::clamp(t, 0.0, 1.0);
        r = static_cast<int>(rMid + t * (rOrange - rMid));
        g = static_cast<int>(gMid + t * (gOrange - gMid));
        b = static_cast<int>(bMid + t * (bOrange - bMid));
      }

      cv::Rect rect(col * tileSize, row * tileSize, tileSize, tileSize);
      cv::rectangle(img, rect, cv::Scalar(b, g, r, 255), -1);

      // Subtle square border
      cv::rectangle(img, rect, cv::Scalar(30, 30, 30, 255), 1);

      // Centered text showing evaluation value
      std::string text = std::to_string(val);
      int fontFace = cv::FONT_HERSHEY_SIMPLEX;
      double fontScale = 0.75;
      int thickness = 2;
      int baseline = 0;
      cv::Size textSize =
          cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);
      cv::Point textOrg(col * tileSize + (tileSize - textSize.width) / 2,
                        row * tileSize + (tileSize + textSize.height) / 2);

      // Drop shadow / outline for readability
      cv::putText(img, text, textOrg, fontFace, fontScale,
                  cv::Scalar(0, 0, 0, 255), thickness + 2, cv::LINE_AA);
      cv::putText(img, text, textOrg, fontFace, fontScale,
                  cv::Scalar(255, 255, 255, 255), thickness, cv::LINE_AA);
    }
  }

  return img;
}

void chess::ui::saveHeatmap(const cv::Mat &heatmap,
                            const std::string &filename) {
  std::filesystem::path p(filename);
  if (p.has_parent_path()) {
    std::error_code ec;
    std::filesystem::create_directories(p.parent_path(), ec);
  }
  if (!cv::imwrite(filename, heatmap)) {
    std::cerr << "[Error] Failed to write heatmap image to: " << filename
              << std::endl;
  }
}