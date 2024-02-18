SRC = final.o geography.o
EXE = final

LIB = -lGL -lglut -lGLEW -lGLU
WARN = -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic

final: $(SRC)
	g++ $(SRC) $(LIB) -o $(EXE) $(WARN)

final.o: final.cpp
	gcc -c final.cpp $(WARN)

geography.o: geography.cpp geography.h
	gcc -c geography.cpp $(WARN)

grid.o: grid.h
	gcc -c grid.h $(WARN)

clean:
	rm -f *.o $(EXE)

