.PHONY: main

main:
	g++ -O2 main.cpp -o main

makeptn:
	g++ -O2 makeptn.cpp -o makeptn

test:
	g++ -O2 test.cpp -o test

clean:
	rm test main makeptn