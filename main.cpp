#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <mutex>

#define WINDOW_DEFAULT_WIDTH 1280
#define WINDOW_DEFAULT_HEIGHT 720

const Uint64 TARGET_FRAMETIME_MS = 1000/5;

class renderContext{
    public:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    float textureWidth = 0.0f;
    float textureHeight = 0.0f;
    std::mutex renderLock; //used to prevent double render at the same time

    renderContext(){
        //create window
        this->window = SDL_CreateWindow(
            "PhotoViewer",
            WINDOW_DEFAULT_WIDTH,
            WINDOW_DEFAULT_HEIGHT,
            SDL_WINDOW_RESIZABLE
        );
        if(!this->window) return;

        //create rendering context
        this->renderer = SDL_CreateRenderer(this->window, nullptr);
        if(!this->renderer){
            SDL_DestroyWindow(this->window);
            this->window = nullptr;
            //renderer remains nullptr
        }
    }

    ~renderContext(){
        if(this->renderer != nullptr){
            SDL_DestroyRenderer(this->renderer);
            this->renderer = nullptr;
        }
        if(this->window != nullptr){
            SDL_DestroyWindow(this->window);
            this->window = nullptr;
        }
        if(this->texture != nullptr){
            SDL_DestroyTexture(texture);
            this->texture = nullptr;
        }
    }

    bool createTexture(const char* filepath){
        //0 means failure and 1 means success
        if(!renderer || !window) return 0;
        texture = IMG_LoadTexture(renderer, filepath);
        if(!texture || !SDL_GetTextureSize(texture, &textureWidth, &textureHeight)){
            if(texture){
                SDL_DestroyTexture(texture);
                texture = nullptr;
            }
            textureWidth = 0.0f, textureHeight = 0.0f;
            return 0;
        }

        return 1;
    }

};

void displayRendererInfo(SDL_Renderer* renderer);
void renderImage(renderContext& rdc);
void destroyWindowAndRenderer(SDL_Window* window, SDL_Renderer* renderer);
float selectFactor(float widthFactor, float heightFactor);

//callback
bool liveResize(void* userdata, SDL_Event* event){
    renderContext* r = static_cast<renderContext*>(userdata);

    r->renderLock.lock();
    if(event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED){
        renderImage(*r);
    }
    r->renderLock.unlock();

    return 1;
}

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

    renderContext* rdc = new renderContext();
    if(rdc->window == nullptr || rdc->renderer == nullptr){
        std::cout << "Failed to init window or failed to create renderer: " << SDL_GetError();
        delete rdc;
        SDL_Quit();
        return 1;
    }

    const bool textureSuccess = rdc->createTexture(argv[1]);
    if(!textureSuccess){
        std::cout << "Failed to create texture and get texture size: " << SDL_GetError();
        delete rdc;
        SDL_Quit();
        return 1;
    }

    //add event watcher for live resize
    const bool liveResizeWatcherSuccess = SDL_AddEventWatch(liveResize, rdc);
    if(!liveResizeWatcherSuccess){
        std::cout << "Failed to add Event Watcher for live resize; Will fallback to simple resize";
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

        rdc->renderLock.lock();
        renderImage(*rdc);
        rdc->renderLock.unlock();

        const Uint64 frametime = SDL_GetTicks() - startTicks;
        if(frametime < TARGET_FRAMETIME_MS){
            SDL_Delay(TARGET_FRAMETIME_MS - frametime);
        }
    }

    delete rdc;
    SDL_Quit();

    return 0;
}

void renderImage(renderContext& rdc){
    //clear the previous frame
    SDL_RenderClear(rdc.renderer);

    //get current window for each frame
    int windowWidth, windowHeight;
    if(!SDL_GetWindowSize(rdc.window, &windowWidth, &windowHeight)) return;

    //crop of the actual image (This decides the amount of the actual image content)
    SDL_FRect src = {0.0f, 0.0f, rdc.textureWidth, rdc.textureHeight};

    /*    Calculate the scaling factor    */
    float widthFactor = windowWidth / rdc.textureWidth;
    float heightFactor = windowHeight / rdc.textureHeight;
    float factor = selectFactor(widthFactor, heightFactor);

    float scaledImageWidth = rdc.textureWidth * factor;
    float scaledImageHeight = rdc.textureHeight * factor;

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

    bool success = SDL_RenderTexture(rdc.renderer, rdc.texture, &src, &dest);
    if(!success){
        std::cout << "SDL Error failed to render texture: " << SDL_GetError();
        return;
    }

    bool success2 = SDL_RenderPresent(rdc.renderer);
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
