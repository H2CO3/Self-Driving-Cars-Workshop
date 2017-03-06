//
// Self-Driving Cars Workshop
//
// Implementation by Arpad Goretity (H2CO3)
// See LICENSE for details
//

#include <utility>


namespace sdc {

struct Config {
    unsigned n_lines;
    double   cluster_thresh_ratio;
    double   y_crop_min;
    double   y_crop_max;
    unsigned blur_radius;
    unsigned canny_min_thresh;
    unsigned canny_max_thresh;
    unsigned canny_kern_size;
    unsigned hough_dist_resol;
    double   hough_angle_resol;
    unsigned hough_vote_thresh;
    unsigned hough_min_length;
    unsigned hough_max_gap;
};

template<typename T>
struct Context {
    const Config *config;
    T value;
};

template<typename T>
Context<T> make_context(const Config &config, T value) {
    return Context<T> { &config, std::move(value) };
}

// Make pipelines look pretty
template<typename Arg, typename Fn>
auto operator|(const Arg &arg, Fn &&fn) {
    return make_context(*arg.config, fn(*arg.config, arg.value));
}

}
