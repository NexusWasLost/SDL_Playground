all:
	g++ main.cpp -o main.exe -I"SDL3/x86_64/include" -I"MPV/include" -L"MPV" -L"SDL3/x86_64/lib" -lmingw32 -lSDL3 -lmpv
