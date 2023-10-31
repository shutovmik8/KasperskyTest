.PHONY: build test
build: zip unzip

test:
	bash test.sh	

zip unzip: main.c
	gcc main.c -o zip -lzip
	cp zip unzip
