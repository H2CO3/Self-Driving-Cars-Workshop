#include <cstdio>
#include <vector>
#include <utility>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <type_traits>
#include <opencv2/opencv.hpp>


namespace sdc {

using LineArray = std::vector<cv::Vec4i>;

template<typename T>
struct reverse_view {
    using const_iterator = std::reverse_iterator<typename T::const_iterator>;
    using const_reference = typename T::const_reference;

    const_iterator b;
    const_iterator e;

    reverse_view(const_iterator b_, const_iterator e_):
        b(b_), e(e_) {}

    const_iterator begin() const { return b; }
    const_iterator end()   const { return e; }

    const_reference front() const { return *b; }
    const_reference back()  const { return *(e - 1); }
};

template<typename T>
reverse_view<T> reversed(const T &c) {
    return reverse_view<T>(c.rbegin(), c.rend());
}

template<typename Arg, typename Fn>
auto operator|(const Arg &arg, Fn &&fn)
    -> typename std::result_of<Fn(const Arg &)>::type {

    return fn(arg);
}

void print_lines(const LineArray &lines) {
    for (auto &line : lines) {
        std::printf(
            "(%d, %d)\t-> (%d, %d)\n",
            line[0], line[1], line[2], line[3]
        );
    }
}

const std::size_t n_lines = 5;
const double extreme_cluster_threshold = 0.85;

double slope(const cv::Vec4i &v) {
    return double(v[3] - v[1]) / (v[2] - v[0]);
}

cv::Mat mask(const cv::Mat &in_image) {
    auto size = in_image.size();
    int x = 0;
    int y = size.height * 0.5;
    int w = size.width;
    int h = size.height * 0.4;
    return in_image(cv::Rect(x, y, w, h));
}

cv::Mat rgb_to_grayscale(const cv::Mat &in_image) {
    cv::Mat out_image;
    cv::cvtColor(in_image, out_image, CV_RGB2GRAY);
    return out_image;
}

cv::Mat blur(const cv::Mat &in_image) {
    cv::Mat out_image;
    cv::GaussianBlur(in_image, out_image, { 17, 17 }, 0, 0);
    return out_image;
}

cv::Mat detect_edges(const cv::Mat &in_image) {
    cv::Mat out_image;
    cv::Canny(in_image, out_image, 60, 190, 5);
    return out_image;
}

auto hough_transform(const cv::Mat &in_image)
    -> std::tuple<LineArray, cv::Size> {

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(
        in_image,
        lines,
        2,
        M_PI / 90,
        20,
        8
    );

    return { lines, in_image.size() };
}

auto filter_lane_marks(
    const std::tuple<LineArray, cv::Size> &input
) -> std::tuple<LineArray, LineArray> {
    auto lines = std::get<0>(input);
    auto size = std::get<1>(input);

    if (lines.size() < 2 * n_lines) {
        return std::make_tuple(LineArray(), LineArray());
    }

    std::sort(
        lines.begin(),
        lines.end(),
        [](auto &l1, auto &l2) { return slope(l1) < slope(l2); }
    );

    double avg_slope = std::accumulate(
        lines.begin(),
        lines.end(),
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

            if (rel_slope < rel_pivot_slope * extreme_cluster_threshold) {
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

auto pipeline(const cv::Mat &in_image) -> std::tuple<LineArray, LineArray> {
    return in_image | mask | rgb_to_grayscale | blur | detect_edges | hough_transform | filter_lane_marks;
}

}

int main(int argc, char *argv[]) {
    const char *in_filename = argv[1];

    auto in_image = cv::imread(in_filename, CV_LOAD_IMAGE_COLOR);
    auto lines = sdc::pipeline(in_image);

    std::printf("left lines:\n");
    sdc::print_lines(std::get<0>(lines));

    std::printf("right lines:\n");
    sdc::print_lines(std::get<1>(lines));

    return 0;
}
