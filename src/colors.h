/*
 * Color definitions for the program
 */

#ifndef COLORS_H
#define COLORS_H
#include <SDL3/SDL.h>

// Define some common colors using SDL_Color struct
const SDL_Color COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color COLOR_RED = {255, 0, 0, 255};
const SDL_Color COLOR_GREEN = {0, 255, 0, 255};
const SDL_Color COLOR_BLUE = {0, 0, 255, 255};
const SDL_Color COLOR_LIGHT_GRAY = {200, 200, 200, 255};
const SDL_Color COLOR_DARK_GRAY = {50, 50, 50, 255};
const SDL_Color COLOR_LIGHT_BLACK = {30, 30, 30, 255}; 
const SDL_Color COLOR_SEMI_TRANSPARENT_BLUE = {0, 200, 255, 120};

const SDL_Color COLOR_BUTTON_HOVER = COLOR_SEMI_TRANSPARENT_BLUE; // White for default button color
const SDL_Color BACKGROUND_COLOR = COLOR_LIGHT_BLACK; // Light gray for background


#endif // COLORS_H