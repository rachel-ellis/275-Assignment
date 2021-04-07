all: server client
	gnome-terminal -- ./client/client 8888 127.0.0.1
	gnome-terminal -- ./server/server 8888
	gnome-terminal -- ./plotter

server: ./server/server.o ./server/digraph.o ./server/dijkstra.o
	g++ -o ./server/server ./server/server.o ./server/digraph.o ./server/dijkstra.o

server.o: ./server/server.cpp
	g++ -c ./server/server.cpp 

digraph.o: ./server/digraph.cpp
	g++ -c ./server/digraph.cpp

dijkstra.o: ./server/dijkstra.cpp
	g++ -c ./server/dijkstra.cpp

client: ./client/client.o
	g++ -o ./client/client ./client/client.o

client.o: ./client/client.cpp
	g++ -c ./client/client.cpp

clean:
	rm inpipe outpipe
	rm -rf ./client/client.o ./client/client
	rm -rf ./server/*.o ./server/server 

