NAME				:= libftpp.a
CMAKE				:= "cmake" # must keep quotes (1)
CMAKE_TARGET		:= libftpp
BUILD_DIRECTORY		:= cmake-build-export
MAKEFILE_ARGUMENTS	:= -j 6 VERBOSE=1
MAKEFILE_ARGUMENTS	:= VERBOSE=1

all: $(NAME)
.PHONY : all

$(NAME): $(BUILD_DIRECTORY)/$(NAME)
	cp $(BUILD_DIRECTORY)/$(NAME) $(NAME)

$(BUILD_DIRECTORY)/$(NAME): init
	$(CMAKE) --build $(BUILD_DIRECTORY) --target $(CMAKE_TARGET) -- $(MAKEFILE_ARGUMENTS)

init:
	$(CMAKE) -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=c++ -G "Unix Makefiles" -S . -B $(BUILD_DIRECTORY)
.PHONY : init

clean:
	$(CMAKE) --build $(BUILD_DIRECTORY) --target clean -- $(MAKEFILE_ARGUMENTS)
.PHONY : clean

fclean:
	rm -rf $(BUILD_DIRECTORY)
	rm -f $(NAME)
.PHONY : fclean

re: fclean all
.PHONY : re

# (1)
# Without quotes, it triggers this bug on my session (using GNU Make 4.3) https://savannah.gnu.org/bugs/?57962
# https://stackoverflow.com/questions/73908737/permission-denied-when-running-go-from-makefile
