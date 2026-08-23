#pragma once

#include <string>
#include <opencv2/opencv.hpp>

namespace chess::ui {
    
    cv::Mat createHeatmap(const int board[8][8], int baseValue = -1);
    void saveHeatmap(const cv::Mat& heatmap, const std::string& filename);

} // namespace chess::ui
