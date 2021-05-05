CC  := gcc
CXX := g++
CP  := cp
RM  := rm -rf
MD  := mkdir -p

CXXFLAGS += -std=c++17 -pedantic -Wall -O3
LDFLAGS += -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lGLEW

CPPFLAGS += -I include/
#all: dirs bin/program

default: 
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o bin/main.o main.cpp -g
	$(CXX) -o bin/program bin/*.o $(LDFLAGS) -g

imgui:
	$(CXX) include/gui/imgui.cpp -o bin/imgui.o -c $(LDFLAGS)
	$(CXX) include/gui/imgui_demo.cpp -o bin/imgui_demo.o -c $(LDFLAGS) 
	$(CXX) include/gui/imgui_draw.cpp -o bin/imgui_draw.o -c $(LDFLAGS) 
	$(CXX) include/gui/imgui_impl_glfw.cpp -o bin/imgui_impl_glfw.o -c $(LDFLAGS)
	$(CXX) include/gui/imgui_impl_opengl3.cpp -o bin/imgui_impl_opengl3.o -c $(LDFLAGS)
	$(CXX) include/gui/imgui_tables.cpp -o bin/imgui_tables.o -c $(LDFLAGS)
	$(CXX) include/gui/imgui_widgets.cpp -o bin/imgui_widgets.o -c $(LDFLAGS)

stbi:
	$(CC) -c src/stb_image.c -I include/ -o bin/stbi.o

glad:
	$(CC) -c src/glad.c -I include/ -o bin/glad.o

clean: 
	rm -rf bin/*
