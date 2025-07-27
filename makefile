start:
	mkdir -p build
	clang++ -std=c++23 -I/home/leodora/.rqp/cpptoml/include -Isrc/main/include -g -o build/prebyte src/main/cpp/Executer.cpp src/main/cpp/main.cpp src/main/cpp/datatypes/*.cpp src/main/cpp/parser/*.cpp src/main/cpp/processor/*.cpp -lpugixml -lyaml-cpp -lfmt

run:
	./build/main

clean:
	rm -rf build

lib:
	mkdir -p build
	clang++ -std=c++23 -I/home/leodora/.rqp/cpptoml/include -Isrc/main/include -g -shared -fPIC -o build/libprebyte.so src/main/cpp/Executer.cpp src/main/cpp/PrebyteEngine.cpp src/main/cpp/datatypes/*.cpp src/main/cpp/parser/*.cpp src/main/cpp/processor/*.cpp -lpugixml -lyaml-cpp -lfmt

test:
	./build/main ~/Documents/Dev/Codarity/Prebyte/src/test/testfile/test1.cpp ~/Documents/Dev/Codarity/Prebyte/build/test.cpp -D__TIMESTAMP__=123456 -DmethodName=testMethod -Ddependencie=iostream
