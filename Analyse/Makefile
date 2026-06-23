exec:
	rm -f Analysis
	g++ -o Analysis Analysis.cpp `root-config --cflags` `root-config --glibs` -g

debug:
	rm -f Analysis
	g++ -o Analysis Analysis.cpp `root-config --cflags` `root-config --glibs` -g -DDEBUG

opt:
	rm -f Analysis
	g++ -o Analysis Analysis.cpp `root-config --cflags` `root-config --glibs` -O3