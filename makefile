all:
	g++ main.cpp -o main.exe -I"SDL3/x86_64/include" -I"SDL3_image/x86_64/include" -L"SDL3/x86_64/lib" -L"SDL3_image/x86_64/lib" -lmingw32 -lSDL3 -lSDL3_image
