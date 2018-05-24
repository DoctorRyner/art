build:
	g++ -std=gnu++2a -o s main.cpp; ./s; rm s

run:
	g++ -std=gnu++2a -o art art.cpp; ./art; rm art

a:
	g++ -std=gnu++2a -o s main.cpp; ./s; g++ -std=gnu++2a -o art art.cpp; ./art; rm s art