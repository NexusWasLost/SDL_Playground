#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#define WINDOW_MAX_WIDTH 1280
#define WINDOW_MAX_HEIGHT 720

void displayRendererInfo(SDL_Renderer* renderer);
SDL_Texture* stageImage(SDL_Renderer* renderer, const char* filepath);
void loadImage(SDL_Renderer* renderer, SDL_Texture* texture, float width, float height);
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
    SDL_Texture* texture = stageImage(renderer, filepath);
    if(texture == nullptr){
        SDL_DestroyTexture(texture);
        destroyWindowAndRenderer(window, renderer);
        SDL_Quit();
        return 1;
    }
    //get texture width and height
    float width = 0.0f; float height = 0.0f;
    bool textureInfoSuccess = SDL_GetTextureSize(texture, &width, &height);
    if(!textureInfoSuccess){
        SDL_DestroyTexture(texture);
        destroyWindowAndRenderer(window, renderer);
        SDL_Quit();
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

        loadImage(renderer, texture, width, height);
    }

    SDL_DestroyTexture(texture);
    destroyWindowAndRenderer(window, renderer);
    SDL_Quit();

    return 0;
}

SDL_Texture* stageImage(SDL_Renderer* renderer, const char* filepath){
    // SDL_Surface* image = nullptr;
    SDL_Texture* texture = nullptr;

    texture = IMG_LoadTexture(renderer, filepath);
    if(!texture){
        std::cout << "SDL Error creating texture: " << SDL_GetError();
        return nullptr;
    }
    return texture;
}

void loadImage(SDL_Renderer* renderer, SDL_Texture* texture, float width, float height){
    //clear the previous frame
    SDL_RenderClear(renderer);

    // float IMAGE_WIDTH = 736.0f;
    // float IMAGE_HEIGHT = 1594.0f;
    //crop of the actual image (This decides the amount of the actual image content)
    SDL_FRect src = {0.0f, 0.0f, width, height};
    /*
    crop of the bounding box in the main window in which the image will fit !
    The image will fill this box no matter its crop
    If the image's src crop is set as 1000px by 1000px it will fill the destination box,
    no matter the destination box's widht and height
    */

    /*
    Calculate the scaling factor
    */
    float widthFactor = WINDOW_MAX_WIDTH / width;
    float heightFactor = WINDOW_MAX_HEIGHT / height;
    float factor = selectFactor(widthFactor, heightFactor);

    float scaledImageWidth = width * factor;
    float scaledImageHeight = height * factor;

    //calculate window center
    float windowWidthCenter = WINDOW_MAX_WIDTH / 2;
    float windowHeightCenter = WINDOW_MAX_HEIGHT / 2;

    //calculate image starting point
    /*
    I would want my image to be centered
    That means my image's center and window's center would intersect !
    So if I go half way from the center in X axis I will cover half of the image horizontally and land on the edge of the image
    Doing so on the Y axis will make me land on the edge of the image vertically
    therefore moving both horizontally and vertically from the center will gimme the starting of the image !
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
