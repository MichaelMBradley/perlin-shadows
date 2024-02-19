SRC = final.o geography.o grid.o math.o vec.o
EXE = final

LIB = -lGL -lglut -lGLEW -lGLU
WARN = -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic

final: $(SRC)
	g++ $(SRC) $(LIB) -o $(EXE) $(WARN)

final.o: final.cpp
	gcc -c final.cpp $(WARN)

geography.o: geography.cpp geography.h
	gcc -c geography.cpp $(WARN)

grid.o: grid.h grid.cpp
	gcc -c grid.cpp $(WARN)

math.o: math.h math.cpp
	gcc -c math.cpp $(WARN)

vec.o: vec.h vec.cpp
	gcc -c vec.cpp $(WARN)

clean:
	rm -f *.o *.gch $(EXE)

