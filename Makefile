flags 		= -c -Wall -g -O3
ldFlags 	= 
objDir 		= obj/
sourceDir 	= src/
executable  = tonemap

SYS := $(shell gcc -dumpmachine)
ifneq (, $(findstring linux, $(SYS)))
	ldFlags += -lglfw3 -lGLEW
else ifneq (, $(findstring mingw, $(SYS)))
	ldFlags += -lglfw3 -lglew32 -lopengl32
else ifneq (, $(findstring darwin, $(SYS)))
	flags 	+= -I/usr/local/include
	ldFlags += -L/usr/local/lib
	ldFlags += -framework Cocoa -framework OpenGL -lglfw3 -lGLEW
endif

all: $(executable)

$(executable): obj/main.o obj/Framebuffer2D.o obj/HDRImage.o obj/Shader.o obj/Texture2D.o obj/rgbe.o
	g++ obj/main.o obj/Framebuffer2D.o obj/HDRImage.o obj/Shader.o obj/Texture2D.o obj/rgbe.o $(ldFlags) -o $(executable)

obj/main.o: src/main.cpp
	g++ $(flags) -c src/main.cpp -o obj/main.o

obj/Framebuffer2D.o: src/Framebuffer2D.cpp
	g++ $(flags) -c src/Framebuffer2D.cpp -o obj/Framebuffer2D.o

obj/HDRImage.o: src/HDRImage.cpp
	g++ $(flags) -c src/HDRImage.cpp -o obj/HDRImage.o

obj/Shader.o: src/Shader.cpp
	g++ $(flags) -c src/Shader.cpp -o obj/Shader.o

obj/Texture2D.o: src/Texture2D.cpp
	g++ $(flags) -c src/Texture2D.cpp -o obj/Texture2D.o

obj/rgbe.o: src/rgbe.c
	gcc $(flags) -c src/rgbe.c -o obj/rgbe.o

clean:
	$(RM) $(executable) obj/main.o obj/Framebuffer2D.o obj/HDRImage.o obj/Shader.o obj/Texture2D.o obj/rgbe.o