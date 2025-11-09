SHELL:=/bin/bash
all: build
build:
	cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
	cmake --build build -j
golden:
	./build/gen_synth --count 1000000 --out data/golden/itch_1m.bin
	shasum -a 256 data/golden/itch_1m.bin | awk '{print $$1}' > data/golden/itch_1m.digest
verify:
	scripts/verify_golden.sh
bench: build
	scripts/bench.sh 9
release: build verify
	rm -rf artifacts/release && mkdir -p artifacts/release
	cp -r build/replay build/gen_synth data/golden scripts/verify_golden.sh artifacts/release/
	tar -C artifacts -czf artifacts/blanc-lob-engine-rc.tar.gz release
	@echo "Release bundle at artifacts/blanc-lob-engine-rc.tar.gz"
