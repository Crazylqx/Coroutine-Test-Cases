#include <cstdint>
#include <cstddef>

void test(size_t n, size_t data[]);
void test_a(size_t n, size_t data[]);
void init(size_t n, size_t data[]);


int main() {
    constexpr size_t N = 1000 * 1000;
    size_t *data = new size_t[N];
    init(N, data);
    for (size_t i = 0; i < 3; i++) {
        test(N, data);
        test_a(N, data);
    }
    return 0;
}