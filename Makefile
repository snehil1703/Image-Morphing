all: lab1 lab1_gray

lab1: lab1.cpp
	g++ lab1.cpp -o lab1 -lpng -I.

lab1_gray: lab1_gray.cpp
	g++ lab1_gray.cpp -o lab1_gray -lpng -I.

clean:
	rm lab1
	rm lab1_gray
