.SILENT:
.DEFAULT_GOAL := help

.PHONY: help
help:
	$(info DK3D commands:)
	$(info -> build                   build game binary)
	$(info -> run                     starts game)

.PHONY: build
build:
	gcc -framework OpenGL -framework GLUT -o DK3D.o main.c -Wno-deprecated-declarations
	echo DK3D.o

.PHONY: run
run:
	./DK3D.o
