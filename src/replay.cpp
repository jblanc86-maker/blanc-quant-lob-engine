#include "order_book.hpp"
#include <fstream>
#include <iostream>
#include <chrono>

struct Order {
    uint64_t id;
    uint32_t qty;
    double px;
};

int main() {
    std::ifstream in("feed.bin", std::ios::binary);
    if (!in) {
        std::cerr << "Missing feed.bin (run gen_synth first)\n";
        return 1;
    }

    OrderBook book;
    book.reserve(10000);

    auto t0 = std::chrono::high_resolution_clock::now();

    Order o;
    while (in.read(reinterpret_cast<char*>(&o.id), sizeof(o.id))) {
        in.read(reinterpret_cast<char*>(&o.qty), sizeof(o.qty));
        in.read(reinterpret_cast<char*>(&o.px),  sizeof(o.px));
        book.add(o.id, o.qty, o.px);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    double us  = std::chrono::duration<double, std::micro>(t1 - t0).count();
    double mps = (book.size() / us) * 1'000'000.0;

    std::cout << "Replay: " << book.size() << " msgs in "
              << us << " us (" << mps << " msgs/sec)\n";

    return 0;
}
