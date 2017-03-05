//
// Self-Driving Cars Workshop
//
// Implementation by Arpad Goretity (H2CO3)
// See LICENSE for details
//

#include "lane_detection.hh"


namespace {

void print_lines(const char *title, const sdc::LineArray &lines) {
    std::printf("%s:\n", title);

    for (auto &line : lines) {
        std::printf(
            "(%d, %d)\t-> (%d, %d)\n",
            line[0], line[1], line[2], line[3]
        );
    }
}

}

int main(int argc, char *argv[]) {
    const char *in_filename = argv[1];

    sdc::Config config = {
        5,         /* n_lines              */
        0.85,      /* cluster_thresh_ratio */
        0.5,       /* y_crop_min           */
        0.9,       /* y_crop_max           */
        8,         /* blur_radius          */
        60,        /* canny_min_thresh     */
        190,       /* canny_max_thresh     */
        5,         /* canny_kern_size      */
        2,         /* hough_dist_resol     */
        M_PI / 90, /* hough_angle_resol    */
        20,        /* hough_vote_thresh    */
        8,         /* hough_min_length     */
        0,         /* hough_max_gap        */
    };

    auto in_image = cv::imread(in_filename, CV_LOAD_IMAGE_COLOR);
    auto context = sdc::make_context(config, std::move(in_image));
    auto lines = sdc::detect_lanes(context).value;

    print_lines("left lines",  std::get<0>(lines));
    print_lines("right lines", std::get<1>(lines));

    return 0;
}
