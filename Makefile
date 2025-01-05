NAME               := libftpp.a
CMAKE_TARGET       := libftpp
BUILD_DIRECTORY    := cmake-build-export
MAKEFILE_ARGUMENTS := -j 6 VERBOSE=1

all: $(NAME)
.PHONY : all

$(NAME): $(BUILD_DIRECTORY)/$(NAME)
	cp $(BUILD_DIRECTORY)/$(NAME) $(NAME)

$(BUILD_DIRECTORY)/$(NAME): init
	cmake --build $(BUILD_DIRECTORY) --target $(CMAKE_TARGET) -- $(MAKEFILE_ARGUMENTS)

init:
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=c++ -G "Unix Makefiles" -S . -B $(BUILD_DIRECTORY)
.PHONY : init

clean:
	cmake --build $(BUILD_DIRECTORY) --target clean -- $(MAKEFILE_ARGUMENTS)
.PHONY : clean

fclean:
	rm -rf $(BUILD_DIRECTORY)
	rm -f $(NAME)
.PHONY : fclean

re: fclean all
.PHONY : re
