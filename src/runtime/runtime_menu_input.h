#pragma once

#include <SDL.h>

inline bool runtime_menu_owns_event(bool open, const SDL_Event& event) {
    return open ||
        (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) ||
        (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE);
}
