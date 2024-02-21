SRC = constants.o final.cpp geography.o grid.o math.o random.o vec.o
EXE = final

LIB = -lGL -lglut -lGLEW -lGLU
WARN = -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic

final: $(SRC)
	g++ $(SRC) $(LIB) -o $(EXE) $(WARN)

constants.o: constants.h constants.cpp
	g++ -c constants.cpp $(WARN)

geography.o: geography.cpp geography.h
	g++ -c geography.cpp $(WARN)

grid.o: grid.h grid.cpp
	g++ -c grid.cpp $(WARN)

math.o: math.h math.cpp
	g++ -c math.cpp $(WARN)

random.o: random.h random.cpp
	g++ -c random.cpp $(WARN)

vec.o: vec.h vec.cpp
	g++ -c vec.cpp $(WARN)

clean:
	rm -f *.o *.gch $(EXE)

