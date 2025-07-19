start:
	mkdir -p build
	clang++ -std=c++20 -Isrc/main/include -g -o build/main src/main/cpp/*.cpp src/main/cpp/datatypes/*.cpp src/main/cpp/parser/*.cpp src/main/cpp/processor/*.cpp

run:
	./build/main

test:
	./build/main ~/Documents/Dev/Codarity/Prebyte/src/test/testfile/test1.cpp ~/Documents/Dev/Codarity/Prebyte/build/test.cpp -D__TIMESTAMP__=123456 -DmethodName=testMethod -Ddependencie=iostream
