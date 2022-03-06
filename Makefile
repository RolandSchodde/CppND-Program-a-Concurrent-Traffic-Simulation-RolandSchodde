.PHONY: all
all: format build

.PHONY: format
format:
	clang-format src/* -i

#.PHONY: test
#test:
	
.PHONY: run
run: format build
	./build/membot
	
.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make

.PHONY: rebuild
rebuild: clean build

.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug .. && \
	make

.PHONY: clean
clean:
	rm -rf build
