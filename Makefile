HEADERS=read5052.cpp HitSiPM.hpp Reader5052.hpp LibCo/*

exec: $(HEADERS)
	# rm exec
	g++ -o exec read5052.cpp `root-config --cflags` `root-config --glibs` -g

debug: $(HEADERS)
	rm exec
	g++ -o exec read5052.cpp `root-config --cflags` `root-config --glibs` -g -DDEBUG

opt: $(HEADERS)
	# rm exec
	g++ -o exec read5052.cpp `root-config --cflags` `root-config --glibs` -O2