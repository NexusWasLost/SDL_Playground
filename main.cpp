#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#define WINDOW_DEFAULT_WIDTH 1280
#define WINDOW_DEFAULT_HEIGHT 720

const Uint64 TARGET_FRAMETIME_MS = 1000/5;

struct imageTexture{
    SDL_Texture* texture;
    float width;
    float height;
};

void displayRendererInfo(SDL_Renderer* renderer);
void renderImage(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, float width, float height);
void destroyWindowAndRenderer(SDL_Window* window, SDL_Renderer* renderer);
float selectFactor(float widthFactor, float heightFactor);

int main(int argc, char** argv){
    if(argc != 2){
        std::cout << "Invalid number of parameters" << "\n";
        return 1;
    }
    const char* filepath = argv[1];

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "SDL could not be initialized: " << SDL_GetError();
        return 1;
    }
    else{
        std::cout << "SDL Video System Ready to go\n";
    }

    SDL_Window* window = SDL_CreateWindow(
        "PhotoViewer",
        WINDOW_DEFAULT_WIDTH,
        WINDOW_DEFAULT_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    if(!window){
        std::cout << "Failed to init window: " << SDL_GetError();
        return 1;
    }

    //create the renderer
    SDL_Renderer*  renderer = SDL_CreateRenderer(window, nullptr);
    if(renderer == nullptr){
        std::cout << "Failed to create renderer: " << SDL_GetError();
        SDL_DestroyWindow(window);
        return 1;
    }
    //print Renderer Name
    displayRendererInfo(renderer);

    //stage the image
    imageTexture img = { IMG_LoadTexture(renderer, argv[1]), 0.0f, 0.0f };
    if(!img.texture || !SDL_GetTextureSize(img.texture, &img.width, &img.height)){
        std::cout << "Failed to create texture and get texture size: " << SDL_GetError();
        if(img.texture) SDL_DestroyTexture(img.texture);
        destroyWindowAndRenderer(window, renderer);
        SDL_Quit();
        return 1;
    }

    bool windowIsRunning = true;
    while(windowIsRunning){
        SDL_Event event;
        Uint64 startTicks = SDL_GetTicks();

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                windowIsRunning = false;
                break;
            }
        }

        renderImage(window, renderer, img.texture, img.width, img.height);
        const Uint64 frametime = SDL_GetTicks() - startTicks;
        if(frametime < TARGET_FRAMETIME_MS){
            SDL_Delay(TARGET_FRAMETIME_MS - frametime);
        }
    }

    SDL_DestroyTexture(img.texture);
    destroyWindowAndRenderer(window, renderer);
    SDL_Quit();

    return 0;
}

void renderImage(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, float width, float height){
    //clear the previous frame
    SDL_RenderClear(renderer);

    //get current window for each frame
    int windowWidth, windowHeight;
    if(!SDL_GetWindowSize(window, &windowWidth, &windowHeight)) return;

    //crop of the actual image (This decides the amount of the actual image content)
    SDL_FRect src = {0.0f, 0.0f, width, height};

    /*    Calculate the scaling factor    */
    float widthFactor = windowWidth / width;
    float heightFactor = windowHeight / height;
    float factor = selectFactor(widthFactor, heightFactor);

    float scaledImageWidth = width * factor;
    float scaledImageHeight = height * factor;

    //calculate window center
    float windowWidthCenter = windowWidth / 2;
    float windowHeightCenter = windowHeight / 2;

    //calculate image starting point
    /*
    Image and window center must intersect
    From the center going half the image's width(X-axis) and half its height (Y-axis)
    will result in landing on the image starting point
    */
    float imageStartWidth = windowWidthCenter - (scaledImageWidth / 2);
    float imageStartHeight = windowHeightCenter - (scaledImageHeight / 2);

    SDL_FRect dest = {imageStartWidth, imageStartHeight, scaledImageWidth, scaledImageHeight};

    bool success = SDL_RenderTexture(renderer, texture, &src, &dest);
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

float selectFactor(float widthFactor, float heightFactor){
    if(widthFactor < heightFactor){
        return widthFactor;
    }
    return heightFactor;
}

void displayRendererInfo(SDL_Renderer* renderer){
    const char* rendererName = SDL_GetRendererName(renderer);
    if(rendererName == nullptr){
        std::cout << "SDL Error fetching Renderer Info: " << SDL_GetError();
        return;
    }

    std::cout << rendererName << "\n";
}

void destroyWindowAndRenderer(SDL_Window* window, SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    //pass by reference so it destroys the actual objects
}
