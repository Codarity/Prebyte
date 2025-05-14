start:
	mkdir -p build
	clang++ -std=c++20 -Isrc/main/include -g -o build/main src/main/cpp/*.cpp

run:
	./build/main
