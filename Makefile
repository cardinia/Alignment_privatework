DMRtrends: DMRtrends.C
	g++ -g -Wall -O3 $(shell root-config --cflags) -I$(shell root-config --incdir) -std=c++17 $^ $(shell root-config --libs) -lstdc++fs -o $@

clean:
	rm -f DMRtrends
