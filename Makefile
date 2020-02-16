Sokoban: main.o
	g++ -std=c++17 -Wall -pthread -o Sokoban main.o

main.o: main.cpp plateau.hpp
	g++ -std=c++17 -Wall -pthread -o main.o -c main.cpp

clean:
	rm -v -f *.o *~core
mrproper: clean
	rm -v -f Puzzle
