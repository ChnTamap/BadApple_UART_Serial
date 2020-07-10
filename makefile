CMD_CLANG := clang++ -g -Wall -static-libgcc -fcolor-diagnostics --target=x86_64-w64-mingw -std=c++11
# CMD_CLANG := g++ -g
obj/Serial.exe:output/main.o
	$(CMD_CLANG) -o $@ $^
output/main.o:src/main.cpp
	$(CMD_CLANG) -o $@ -c $^
clean:
	del -rf output/*.o