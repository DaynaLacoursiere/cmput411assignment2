
OS := $(shell uname -s)

FLAGS = -I/usr/include -I/usr/include/eigen3 -I/opt/local/include/eigen3 -L/usr/lib -L/usr/lib/nvidia-340 -lglut -lGLEW -lGL -lGLU -lX11 -Wno-write-strings -std=c++11


all: motionViewer

motionViewer: motionViewer.cpp skeleton.* camera.h
	g++ motionViewer.cpp skeleton.cpp -o motionViewer $(FLAGS)

clean:
	rm motionViewer
