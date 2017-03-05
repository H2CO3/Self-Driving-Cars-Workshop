//
// Self-Driving Cars Workshop
//
// Implementation by Arpad Goretity (H2CO3)
// See LICENSE for details
//

#include <utility>
#include <vector>
#include <opencv2/opencv.hpp>

#include "context.hh"


namespace sdc {

using LineArray = std::vector<cv::Vec4i>;

auto detect_lanes(const Context<cv::Mat> &in_ctx)
    -> Context<std::tuple<LineArray, LineArray>>;

}
