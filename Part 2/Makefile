all: server client
	gnome-terminal --./client
	gnome-terminal --./server
	gnome-terminal --./plotter

server: server.o
	g++ -o server server.o

server.o: server.cpp
	g++ -c server.cpp

client: client.o
	g++ -o client client.o

client.o: clientclient.cpp
	g++ -c client.cpp


