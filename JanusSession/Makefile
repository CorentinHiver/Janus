# LIB=-I/home/corentin/Installation/Janus_5202_4.1.0_20250228_linux/ferslib/include
LIB=-I/home/corentin/Installation/Janus_5202_4.1.0_20250228_linux/ferslib/local/include 
LIB_PATH=-L/home/corentin/Installation/Janus_5202_4.1.0_20250228_linux/ferslib/local/lib
HEADERS=JanusSession.hpp

exec: $(HEADERS)
	g++ -o exec communicator.cpp -g $(LIB) $(LIB_PATH) -lcaenferslib

debug: $(HEADERS)
	g++ -o exec communicator.cpp -g -DDEBUG $(LIB) $(LIB_PATH) -lcaenferslib

opt: $(HEADERS)
	g++ -o exec communicator.cpp -O2 $(LIB) $(LIB_PATH) -lcaenferslib