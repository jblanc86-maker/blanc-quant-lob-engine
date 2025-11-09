FROM ubuntu:24.04
RUN apt-get update && apt-get install -y cmake ninja-build g++ && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY . .
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
CMD ["bash","-lc","make verify && ./build/replay --input data/golden/itch_1m.bin"]
