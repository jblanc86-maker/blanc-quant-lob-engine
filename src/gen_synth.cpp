#include <fstream>
#include <random>
#include <iostream>
#include <cstdint>

static constexpr size_t N = 10000;

int main() {
    std::ofstream out("feed.bin", std::ios::binary);
    if (!out) {
        std::cerr << "Cannot open feed.bin for writing\n";
        return 1;
    }

    std::mt19937_64 rng(123456);
    for (size_t i = 0; i < N; ++i) {
        uint64_t id  = rng();
        uint32_t qty = static_cast<uint32_t>(rng() % 1000 + 1);
        double px    = static_cast<double>(rng() % 100000) * 0.01;

        out.write(reinterpret_cast<const char*>(&id),  sizeof(id));
        out.write(reinterpret_cast<const char*>(&qty), sizeof(qty));
        out.write(reinterpret_cast<const char*>(&px),  sizeof(px));
    }

    std::cout << "Generated " << N << " synthetic orders -> feed.bin\n";
    return 0;
}
