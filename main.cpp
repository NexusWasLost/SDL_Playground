#include <iostream>
#include <SDL3/SDL.h>

#define WINDOW_DEFAULT_WIDTH 1280
#define WINDOW_DEFAULT_HEIGHT 720

int main(int argc, char** argv){
    // if(argc != 2){
    //     std::cout << "Invalid number of parameters" << "\n";
    //     return 1;
    // }
    // const char* filepath = argv[1];

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "SDL could not be initialized: " << SDL_GetError();
        return 1;
    }
    else{
        std::cout << "SDL Video System Ready to go\n";
    }

    SDL_Window* window = SDL_CreateWindow(
        "VidPlayer",
        WINDOW_DEFAULT_WIDTH,
        WINDOW_DEFAULT_HEIGHT,
        0
    );
    if(!window){
        std::cout << "Failed to init window: " << SDL_GetError();
        return 1;
    }

    bool windowIsRunning = true;
    while(windowIsRunning){
        SDL_Event event;

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                windowIsRunning = false;
                break;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// void displayRendererInfo(SDL_Renderer* renderer){
//     const char* rendererName = SDL_GetRendererName(renderer);
//     if(rendererName == nullptr){
//         std::cout << "SDL Error fetching Renderer Info: " << SDL_GetError();
//         return;
//     }

//     std::cout << rendererName << "\n";
// }
