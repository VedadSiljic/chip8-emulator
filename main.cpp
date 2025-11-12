#include "chip-8.h"
#include <SDL3/SDL.h>

# define GAME_FILENAME "5-quirks.ch8"
# define IS_DEBUG true

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
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
                continue;
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.scancode) {
                    case 30: // 1
                        setInput(0x1);
                        if constexpr (IS_DEBUG) SDL_Log("1 was pressed.");
                        break;
                    case 31: // 2
                        setInput(0x2);
                        if constexpr (IS_DEBUG) SDL_Log("2 was pressed.");
                        break;
                    case 32: // 3
                        setInput(0x3);
                        if constexpr (IS_DEBUG) SDL_Log("3 was pressed.");
                        break;
                    case 33: // 4
                        setInput(0xC);
                        if constexpr (IS_DEBUG) SDL_Log("4 was pressed.");
                        break;
                    case 20: // Q
                        setInput(0x4);
                        if constexpr (IS_DEBUG) SDL_Log("Q was pressed.");
                        break;
                    case 26: // W
                        setInput(0x5);
                        if constexpr (IS_DEBUG) SDL_Log("W was pressed.");
                        break;
                    case 8: // E
                        setInput(0x6);
                        if constexpr (IS_DEBUG) SDL_Log("E was pressed.");
                        break;
                    case 21: // R
                        setInput(0xD);
                        if constexpr (IS_DEBUG) SDL_Log("R was pressed.");
                        break;
                    case 4: // A
                        setInput(0x7);
                        if constexpr (IS_DEBUG) SDL_Log("A was pressed.");
                        break;
                    case 22: // S
                        setInput(0x8);
                        if constexpr (IS_DEBUG) SDL_Log("S was pressed.");
                        break;
                    case 7: // D
                        setInput(0x9);
                        if constexpr (IS_DEBUG) SDL_Log("D was pressed.");
                        break;
                    case 9: // F
                        setInput(0xE);
                        if constexpr (IS_DEBUG) SDL_Log("F was pressed.");
                        break;
                    case 29: // Z
                        setInput(0xA);
                        if constexpr (IS_DEBUG) SDL_Log("Z was pressed.");
                        break;
                    case 27: // X
                        setInput(0x0);
                        if constexpr (IS_DEBUG) SDL_Log("X was pressed.");
                        break;
                    case 6: // C
                        setInput(0xB);
                        if constexpr (IS_DEBUG) SDL_Log("C was pressed.");
                        break;
                    case 25: // V
                        setInput(0xF);
                        if constexpr (IS_DEBUG) SDL_Log("V was pressed.");
                        break;
                    default:
                        if constexpr (IS_DEBUG) SDL_Log("A key was pressed : %d", event.key.scancode);
                }

                continue;
            }

            if (event.type == SDL_EVENT_KEY_UP) {
                setInput(0xFF);
            }
        }
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
            //SDL_Delay(1000 / 60);
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