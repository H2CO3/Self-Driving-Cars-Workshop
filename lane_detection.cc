//
// Self-Driving Cars Workshop
//
// Implementation by Arpad Goretity (H2CO3)
// See LICENSE for details
//

#include <cstdio>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <type_traits>

#include "lane_detection.hh"
#include "util.hh"


namespace sdc {

double slope(const cv::Vec4i &v) {
    return double(v[3] - v[1]) / (v[2] - v[0]);
}

// Actual functions of the pipeline

cv::Mat crop(const Config &config, const cv::Mat &in_image) {
    auto size = in_image.size();
    int x = 0;
    int y = size.height * config.y_crop_min;
    int w = size.width;
    int h = size.height * (config.y_crop_max - config.y_crop_min);
    return in_image(cv::Rect(x, y, w, h));
}

cv::Mat rgb_to_grayscale(const Config &config, const cv::Mat &in_image) {
    cv::Mat out_image;
    cv::cvtColor(in_image, out_image, CV_RGB2GRAY);
    return out_image;
}

cv::Mat blur(const Config &config, const cv::Mat &in_image) {
    cv::Mat out_image;
    int d = 2 * config.blur_radius + 1;
    cv::GaussianBlur(in_image, out_image, { d, d }, 0, 0);
    return out_image;
}

cv::Mat detect_edges(const Config &config, const cv::Mat &in_image) {
    cv::Mat out_image;
    cv::Canny(
        in_image,
        out_image,
        config.canny_min_thresh,
        config.canny_max_thresh,
        config.canny_kern_size
    );
    return out_image;
}

LineArray hough_transform(const Config &config, const cv::Mat &in_image) {
    std::vector<cv::Vec4i> lines;

    cv::HoughLinesP(
        in_image,
        lines,
        config.hough_dist_resol,
        config.hough_angle_resol,
        config.hough_vote_thresh,
        config.hough_min_length,
        config.hough_max_gap
    );

    return lines;
}

auto filter_lane_marks(const Config &config, LineArray lines)
    -> std::tuple<LineArray, LineArray> {

    if (lines.size() < 2 * config.n_lines) {
        return std::make_tuple(LineArray(), LineArray());
    }

    sort(
        lines,
        [](auto &l1, auto &l2) { return slope(l1) < slope(l2); }
    );

    double avg_slope = accumulate(
        lines,
        0.0,
        [](auto tmp, auto &line) { return tmp + slope(line); }
    );
    avg_slope /= lines.size();

    // selecting extreme clusters
    auto cluster = [=](const auto &range) {
        LineArray result;
        auto &pivot = range.front();

        for (auto &line : range) {
            double rel_slope = std::abs(slope(line) - avg_slope);
            double rel_pivot_slope = std::abs(slope(pivot) - avg_slope);

            if (rel_slope < rel_pivot_slope * config.cluster_thresh_ratio) {
                break;
            }

            result.push_back(line);
        }

        return result;
    };

    return {
        cluster(lines),
        cluster(reversed(lines))
    };
}

auto detect_lanes(const Context<cv::Mat> &in_ctx)
    -> Context<std::tuple<LineArray, LineArray>> {

    return in_ctx | crop | rgb_to_grayscale | blur | detect_edges | hough_transform | filter_lane_marks;
}

}
