make: main.cpp AStar.cpp
	g++ main.cpp AStar.cpp -l raylib -std=c++17 -Wall -d3
#dont do this on macos they don't like it for some reason
profile_linux: main.cpp AStar.cpp
	g++ main.cpp AStar.cpp -l raylib -std=c++17 -Wall -g3 -pg -Dprofile
	./a.out
	>prof.txt
	gprof >prof.txt
	make
profile_macos: main.cpp AStar.cpp
	g++ main.cpp AStar.cpp -l raylib -std=c++17 -Wall -g3 -Dprofile
	./a.out
	>prof.txt
	make