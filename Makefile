SRC = final.o geography.o
EXE = final

LIB = -lGL -lglut -lGLEW -lGLU
WARN = -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic

final: $(SRC)
	g++ $(SRC) $(LIB) -o $(EXE) $(WARN)

final.o: final.cpp
	gcc -c ./final.cpp

geography.o: geography.cpp geography.h
	gcc -c ./geography.cpp

clean:
	rm -f *.o $(EXE)

