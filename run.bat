g++ -c main.cpp -o bin/main.o -Iinclude/ -Idependencies/
g++ -o bin/program.exe bin/*.o -lglfw3dll -lopengl32
.\bin\program.exe
