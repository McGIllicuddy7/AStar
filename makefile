make: main.cpp AStar.cpp
	g++ main.cpp AStar.cpp -l raylib -std=c++17 -Wall
#dont do this on macos they don't like it for some reason
profile: main.cpp AStar.cpp
	g++ main.cpp AStar.cpp -l raylib -std=c++17 -Wall -g3 -pg -Dprofile
	./a.out
	>prof.txt
	gprof >prof.txt