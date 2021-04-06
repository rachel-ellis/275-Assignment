all: server client
	gnome-terminal --./client/client
	gnome-terminal --./server/server
	gnome-terminal --./client/plotter

server: ./server/server.o ./server/digraph.o ./server/dijkstra.o
	g++ -o server ./server/server.o ./server/digraph.o ./server/dijkstra.o

server.o: ./server/server.cpp
	g++ -c ./server/server.cpp 

digraph.o: ./server/digraph.cpp
	g++ -c ./server/digraph.cpp

dijkstra.o: ./server/dijkstra.cpp
	g++ -c ./server/dijkstra.cpp

client: ./client/client.o
	g++ -o client ./client/client.o

client.o: ./client/client.cpp
	g++ -c ./client/client.cpp

clean:
	rm inpipe outpipe

