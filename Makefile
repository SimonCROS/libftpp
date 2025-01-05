all: init
	cmake --build cmake-build-export --target libftpp -- -j 6 VERBOSE=1

tests: init
	cmake --build cmake-build-export --target all -- VERBOSE=1

init:
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=c++ -G "Unix Makefiles" -S . -B cmake-build-export
