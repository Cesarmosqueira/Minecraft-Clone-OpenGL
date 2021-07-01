CC  := gcc
CXX := g++
CP  := cp
RM  := rm -rf
MD  := mkdir -p

CXXFLAGS += -std=c++17 -pedantic -Wall -O3
LDFLAGS += -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp -lGLEW -limgui


IMGUIDIR = dependencies/imgui

CPPFLAGS += -I include/ -I dependencies/ -I$(IMGUIDIR)
#all: dirs bin/program

default: 
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o bin/main.o main.cpp -g
	$(CXX) -o bin/program bin/*.o $(LDFLAGS) -g

imgui:
	$(CXX) -g $(LDFLAGS) -c -o bin/imgui_impl_glfw.o 	 $(IMGUIDIR)/imgui_impl_glfw.cpp
	$(CXX) -g $(LDFLAGS) -c -o bin/imgui_impl_opengl3.o $(IMGUIDIR)/imgui_impl_opengl3.cpp


stbi:
	$(CC) -c src/stb_image.c -I dependencies/ -o bin/stbi.o

znoise: 
	$(CXX) -o bin/FBM.o 	  	-c src/ZNOISE_SRC/FBM.cpp  				 -I dependencies/ZNOISE_INCLUDE/
	$(CXX) -o bin/HMF.o 		-c src/ZNOISE_SRC/HybridMultiFractal.cpp -I dependencies/ZNOISE_INCLUDE/
	$(CXX) -o bin/MixerBase.o 	-c src/ZNOISE_SRC/MixerBase.cpp 		 -I dependencies/ZNOISE_INCLUDE/
	$(CXX) -o bin/NoiseBase.o 	-c -c src/ZNOISE_SRC/NoiseBase.cpp 		 -I dependencies/ZNOISE_INCLUDE/
	$(CXX) -o bin/NoiseTools.o -c -c src/ZNOISE_SRC/NoiseTools.cpp 	 -I dependencies/ZNOISE_INCLUDE/
	$(CXX) -o bin/Perlin.o 	-c -c src/ZNOISE_SRC/Perlin.cpp 		 -I dependencies/ZNOISE_INCLUDE/
	$(CXX) -o bin/Simplex.o 	-c -c src/ZNOISE_SRC/Simplex.cpp 		 -I dependencies/ZNOISE_INCLUDE/
	$(CXX) -o bin/Worley.o 	-c -c src/ZNOISE_SRC/Worley.cpp 		 -I dependencies/ZNOISE_INCLUDE/


clean: 
	rm -rf bin/*
