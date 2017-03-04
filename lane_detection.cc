#include <vector>
#include <cstdio>
#include <opencv2/opencv.hpp>


namespace {

template<typename Arg, typename Fn>
auto operator|(const Arg &arg, Fn &&fn)
    -> typename std::result_of<Fn(const Arg &)>::type {

    return fn(arg);
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

cv::Mat hough_transform(const cv::Mat &in_image) {
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(
        in_image,
        lines,
        1,
        M_PI / 180,
        20,
        8
    );

    auto size = in_image.size();
    cv::Mat out_image = cv::Mat::zeros(
        size.height,
        size.width,
        CV_8UC3
    );

    for (auto &line : lines) {
        cv::line(
            out_image,
            cv::Point(line[0], line[1]),
            cv::Point(line[2], line[3]),
            cv::Scalar(0, 255, 0)
        );
    }

    return out_image;
}

cv::Mat pipeline(const cv::Mat &in_image) {
    return in_image | mask | rgb_to_grayscale | blur | detect_edges | hough_transform;
}

}

int main(int argc, char *argv[]) {
    const char *in_filename = argv[1];
    const char *out_filename = argv[2];

    auto in_image = cv::imread(in_filename, CV_LOAD_IMAGE_COLOR);
    auto out_image = pipeline(in_image);
    cv::imwrite(out_filename, out_image);

    return 0;
}
