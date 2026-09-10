#include <iostream>
#include <SDL3/SDL.h>
#include <mpv/client.h>

#define WINDOW_DEFAULT_WIDTH 1280
#define WINDOW_DEFAULT_HEIGHT 720

const Uint64 TARGET_FRAMETIME_MS = 1000/5;

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
    SDL_PropertiesID winprops = SDL_GetWindowProperties(window);
    if(!winprops){
        std::cout << "Failed to get window property";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    std::cout << "winprops: " << winprops << std::endl;
    void* prop = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    int64_t wid = reinterpret_cast<int64_t>(prop); //take the value at prop and literally cast it to a int 64

    mpv_handle* mpv = mpv_create();
    if(!mpv){
        std::cout << "Failed to create MPV handle";
        return 1;
    }

    int status = mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);
    int hwdec_status = mpv_set_option_string(mpv, "hwdec", "auto");
    if(status < 0){
        std::cout << "Failed to set options";
        mpv_destroy(mpv);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int success = mpv_initialize(mpv);
    if(success < 0){
        std::cout << "Failed to init mpv";
        mpv_destroy(mpv);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const char* cmd[] = {
        "loadfile",
        "./EP.1.v0.1729392308.1080p.mp4",
        nullptr
    };
    int cmd_success = mpv_command(mpv, cmd);
    if(cmd_success < 0){
        std::cout << "Failed to init mpv";
        mpv_destroy(mpv);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool hwdecChecked = false;
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

        const Uint64 frametime = SDL_GetTicks() - startTicks;
        if(frametime < TARGET_FRAMETIME_MS){
            SDL_Delay(TARGET_FRAMETIME_MS - frametime);
        }

        if(!hwdecChecked){
            char* hwdec = nullptr;
            mpv_get_property(mpv, "hwdec-current", MPV_FORMAT_STRING, &hwdec);
            if(hwdec){
                std::cout << "hwdec-current: " << hwdec << "\n";
                mpv_free(hwdec);
                hwdecChecked = true;
            }
        }
    }

    mpv_destroy(mpv);
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
