//
// Self-Driving Cars Workshop
//
// Implementation by Arpad Goretity (H2CO3)
// See LICENSE for details
//

#include <utility>
#include <iterator>


namespace sdc {

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

// Algorithms

template<typename Container>
constexpr std::size_t container_size(const Container &c) {
    return c.size();
}

template<typename T, std::size_t N>
constexpr std::size_t container_size(const T (&)[N]) {
    return N;
}

template<typename Container, typename Item>
auto find(const Container &c, const Item &item)
    -> typename Container::const_iterator {

    return std::find(std::begin(c), std::end(c), item);
}

template<typename Container, typename Item>
bool contains(const Container &c, const Item &item) {
    return std::find(std::begin(c), std::end(c), item) != std::end(c);
}

template<typename Container, typename Pred>
auto find_if(const Container &c, Pred &&predicate)
    -> typename Container::const_iterator {

    return std::find_if(
        std::begin(c),
        std::end(c),
        std::forward<Pred>(predicate)
    );
}

template<typename Container, typename Pred>
bool all_of(const Container &c, Pred &&predicate) {
    return std::all_of(
        std::begin(c),
        std::end(c),
        std::forward<Pred>(predicate)
    );
}

template<typename Container, typename Pred>
bool any_of(const Container &c, Pred &&predicate) {
    return std::any_of(
        std::begin(c),
        std::end(c),
        std::forward<Pred>(predicate)
    );
}

template<typename Container, typename T, typename BinaryOp>
T accumulate(const Container &c, T &&init, BinaryOp &&binary_op) {
    return std::accumulate(
        std::begin(c),
        std::end(c),
        std::forward<T>(init),
        std::forward<BinaryOp>(binary_op)
    );
}

template<typename Container, typename Compare>
void sort(Container &c, Compare &&compare) {
    std::sort(
        std::begin(c),
        std::end(c),
        std::forward<Compare>(compare)
    );
}

template<typename Result, typename Container, typename Fn>
auto vector_map(const Container &c, Fn &&fn) -> std::vector<Result> {
    std::vector<Result> result;
    result.reserve(container_size(c));

    std::transform(
        std::begin(c),
        std::end(c),
        std::back_inserter(result),
        std::forward<Fn>(fn)
    );

    return result;
}

}
