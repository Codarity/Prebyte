TOML_PATH = ${HOME}/.rqp/cpptoml/include
start:
	mkdir -p build
	clang++ -std=c++23 -I$(TOML_PATH) -Isrc/main/include -O3 -o build/prebyte src/main/cpp/Executer.cpp src/main/cpp/main.cpp src/main/cpp/datatypes/*.cpp src/main/cpp/parser/*.cpp src/main/cpp/processor/*.cpp -lpugixml -lyaml-cpp -lfmt

run:
	./build/prebyte

clean:
	rm -rf build

lib:
	mkdir -p build
	clang++ -std=c++23 -I$(TOML_PATH) -Isrc/main/include -O3 -shared -fPIC -o build/libprebyte.so src/main/cpp/Executer.cpp src/main/cpp/PrebyteEngine.cpp src/main/cpp/datatypes/*.cpp src/main/cpp/parser/*.cpp src/main/cpp/processor/*.cpp -lpugixml -lyaml-cpp -lfmt
