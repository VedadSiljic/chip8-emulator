#include "chip-8.h"
#include <SDL3/SDL.h>

# define GAME_FILENAME "2-ibm-logo.ch8"

struct SDLApplication {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    bool isRunning = true;

    SDLApplication() {
        if (!SDL_Init(SDL_INIT_VIDEO)) SDL_Log("Init Error");

        window = SDL_CreateWindow("Chip-8", 800, 600, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, nullptr);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 64, 32);

        SDL_SetRenderLogicalPresentation(renderer, 64, 32, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

        // Init emulator
        startChip8(GAME_FILENAME);
    }

    void Tick() {
        Input();
        Update();
        Render();
    }

    void Input() {
        //TODO Add Input
    }

    void Update() {
        fetchDecodeExecuteInstruction();
    }

    void Render() const {
        const bool* display = getDisplay();
        uint32_t pixels[64 * 32];

        for (int i = 0; i < 64 * 32; i++) pixels[i] = display[i] ? 0xFFFFFFFF : 0xFF000000; // set the pixel white or black
        SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    void mainLoop() {
        while (isRunning) {
            SDL_Delay(1000 / 60);
            Tick();
        }
    }

    ~SDLApplication() {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

};

int main() {
    SDLApplication sdl_app;
    sdl_app.mainLoop();
    return 0;
}