#ifndef UNDO_H
#define UNDO_H

#include "app.h"

void push_undo_state(SDL_Renderer* renderer, SDL_Surface *original_surface, SDL_Texture *image_tex, SDL_FRect image_rect, float image_tex_width, float image_tex_height);
void undo(SDL_Renderer* renderer, SDL_Surface **original_surface, SDL_Texture **image_tex, SDL_FRect *image_rect, float *image_tex_width, float *image_tex_height);
void free_undo_stack();



#endif // UNDO_H