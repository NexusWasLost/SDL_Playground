#include <iostream>
#include <SDL3/SDL.h>

#define WINDOW_MAX_WIDTH 640
#define WINDOW_MAX_HEIGHT 480

void displayRendererInfo(SDL_Renderer* renderer);
SDL_Texture* stageImage(SDL_Renderer* renderer);
void loadImage(SDL_Renderer* renderer, SDL_Texture* texture);

int main(int argc, char** argv){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "SDL could not be initialized: " << SDL_GetError();
        return 1;
    }
    else{
        std::cout << "SDL Video System Ready to go\n";
    }

    SDL_Window* window = nullptr;
    window = SDL_CreateWindow(
        "PhotoViewer",
        WINDOW_MAX_WIDTH,
        WINDOW_MAX_HEIGHT,
        0
    );
    if(!window){
        std::cout << "Failed to init window: " << SDL_GetError();
        return -1;
    }

    //create the renderer
    SDL_Renderer* renderer = nullptr;
    renderer = SDL_CreateRenderer(window, nullptr);
    //print Renderer Name
    displayRendererInfo(renderer);

    //stage image before main loop
    SDL_Texture* texture = stageImage(renderer);
    if(texture == nullptr) return 1;

    bool windowIsRunning = true;
    while(windowIsRunning){
        SDL_Event event;

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                windowIsRunning = false;
                break;
            }
        }

        loadImage(renderer, texture);
    }

    SDL_DestroyTexture(texture);
    //destroy the renderer
    SDL_DestroyRenderer(renderer);
    //destroy the window
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

SDL_Texture* stageImage(SDL_Renderer* renderer){
    SDL_Surface* image = nullptr;
    SDL_Texture* texture = nullptr;

    image = SDL_LoadBMP("example.bmp");
    if(image == nullptr){
        std::cout << "SDL Error creating Surface: " << SDL_GetError();
        return nullptr;
    }

    texture = SDL_CreateTextureFromSurface(renderer, image);
    if(texture == nullptr){
        std::cout << "SDL Error creating texture: " << SDL_GetError();
        SDL_DestroySurface(image);
        return nullptr;
    }

    SDL_DestroySurface(image);
    return texture;
}

void loadImage(SDL_Renderer* renderer, SDL_Texture* texture){
    //clear the previous frame
    SDL_RenderClear(renderer);

    bool success = SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    if(!success){
        std::cout << "SDL Error failed to render texture: " << SDL_GetError();
        return;
    }

    bool success2 = SDL_RenderPresent(renderer);
    if(!success2){
        std::cout << "SDL Error failed to render frame: " << SDL_GetError();
        return;
    }
}

void displayRendererInfo(SDL_Renderer* renderer){
    const char* rendererName = SDL_GetRendererName(renderer);
    if(rendererName == nullptr){
        std::cout << "SDL Error fetching Renderer Info: " << SDL_GetError();
        return;
    }

    std::cout << rendererName << "\n";
}
