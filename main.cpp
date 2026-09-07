#include <iostream>
#include <SDL3/SDL.h>

#define WINDOW_MAX_WIDTH 640
#define WINDOW_MAX_HEIGHT 480

void displayRendererInfo(SDL_Renderer* renderer);

int main(int argc, char** argv){
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        std::cout << "SDL could not be initialized: " << SDL_GetError();
    else
        std::cout << "SDL Video System Ready to go\n";

    SDL_Window* window = nullptr;
    window = SDL_CreateWindow(
        "PhotoViewer",
        WINDOW_MAX_WIDTH,
        WINDOW_MAX_HEIGHT,
        0
    );

    //create the renderer
    SDL_Renderer* renderer = nullptr;
    renderer = SDL_CreateRenderer(window, nullptr);
    //print Renderer Name
    displayRendererInfo(renderer);

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

    //destroy the renderer
    SDL_DestroyRenderer(renderer);
    //destroy the window
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void displayRendererInfo(SDL_Renderer* renderer){
    const char* rendererName = SDL_GetRendererName(renderer);
    if(rendererName == nullptr){
        std::cout << "SDL Error fetching Renderer Info: " << SDL_GetError();
        return;
    }

    std::cout << rendererName << "\n";
}
