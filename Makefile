.SILENT:
.DEFAULT_GOAL := help

.PHONY: help
help:
	$(info DK3D commands:)
	$(info -> build                   build game binary)
	$(info -> run                     starts game)

.PHONY: build
build:
	gcc -framework OpenGL -framework GLUT -o dk3d.o main.c

.PHONY: run
run:
	./dk3d.o
