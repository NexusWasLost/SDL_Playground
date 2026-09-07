all:
	g++ main.cpp -o main.exe -I"SDL3/x86_64/include" -L"SDL3/x86_64/lib" -lmingw32 -lSDL3
