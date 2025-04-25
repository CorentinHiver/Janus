LIB=-I/home/corentin/Installation/Janus_5202_4.1.0_20250228_linux/ferslib/include
HEADERS=read5052.cpp DataReader_5202.hpp MyBinaryData.hpp HitSiPM.hpp TreeSiPM.hpp Reader5052.hpp LibCo/*  JanusSession.hpp

exec: $(HEADERS)
	# rm exec
	g++ -o exec read5052.cpp `root-config --cflags` `root-config --glibs` -g $(LIB)

debug: $(HEADERS)
	rm exec
	g++ -o exec read5052.cpp `root-config --cflags` `root-config --glibs` -g -DDEBUG

opt: $(HEADERS)
	# rm exec
	g++ -o exec read5052.cpp `root-config --cflags` `root-config --glibs` -O2