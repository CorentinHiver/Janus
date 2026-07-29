HEADERS=janus2root.cpp HitSiPM.hpp Reader5052.hpp RootHitSiPM.hpp

exec: $(HEADERS)
	g++ -o janus2root janus2root.cpp `root-config --cflags` `root-config --glibs` -g

debug: $(HEADERS)
	g++ -o janus2root janus2root.cpp `root-config --cflags` `root-config --glibs` -g -DDEBUG

opt: $(HEADERS)
	g++ -o janus2root janus2root.cpp `root-config --cflags` `root-config --glibs` -O3 -march=native