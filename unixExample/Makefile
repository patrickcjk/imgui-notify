#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 23.04 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# Linux:
#	Ubuntu, Linux Mint:
#   	apt-get install libglfw-dev
# 	Fedora:
#		dnf install glfw-devel
#		dnf install vulkan-validation-layers
# Mac OS X:
#   brew install glfw
# MSYS2:
#   pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
#

CXX = g++
#CXX = clang++

EXE = test
IMGUI_DIR = ./
SOURCES = main.cpp


SOURCES += $(IMGUI_DIR)/imgui/imgui.cpp $(IMGUI_DIR)/imgui/imgui_demo.cpp $(IMGUI_DIR)/imgui/imgui_draw.cpp $(IMGUI_DIR)/imgui/imgui_tables.cpp $(IMGUI_DIR)/imgui/imgui_widgets.cpp $(IMGUI_DIR)/imgui/implot.cpp $(IMGUI_DIR)/imgui/implot_items.cpp $(IMGUI_DIR)/imgui/implot_demo.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_vulkan.cpp 
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)
LINUX_VULKAN_LIBS = -lvulkan -lpthread -ldl -lX11 -lXrandr

CXXFLAGS = -std=c++23 -I$(IMGUI_DIR)/imgui -I$(IMGUI_DIR)/backends -I$(IMGUI_DIR)/src/sources -I$(IMGUI_DIR)/src/headers
CXXFLAGS += -g -Wall -Wformat
LIBS =  

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += $(LINUX_VULKAN_LIBS) `pkg-config --static --libs glfw3`

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:$(IMGUI_DIR)/src/sources/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f *.o
