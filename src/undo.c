/*
    Undo functionality for sShot.
    This module will handle the undo stack and related operations for sShot.
*/

#include "app.h"
#include "undo.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct UndoState {
    SDL_Texture *image_tex;
    SDL_FRect image_rect;
    float image_tex_width;
    float image_tex_height;
} UndoState;

typedef struct UndoNode {
    UndoState *state;
    struct UndoNode *next;
} UndoNode;

SDL_Texture* copy_texture(SDL_Renderer* renderer,SDL_Texture* src, float w, float h) {
    SDL_Texture* copy = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        (int)w, (int)h
    );
    if (!copy) return NULL;

    SDL_Texture* prev_target = SDL_GetRenderTarget(renderer);

    SDL_SetRenderTarget(renderer, copy);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_FRect dst = { 0, 0, w, h };
    SDL_RenderTexture(renderer, src, NULL, &dst);

    SDL_SetRenderTarget(renderer, prev_target);

    return copy;
}

UndoNode *undo_stack = NULL;

UndoNode *push(UndoNode *stack, UndoState *state) {
    UndoNode *new_node = malloc(sizeof(UndoNode));
    if (!new_node) {
        printf("Failed to allocate memory for undo node\n");
        return stack; // Return existing stack if allocation fails
    }
    new_node->state = state;
    new_node->next = stack;
    stack = new_node;
    return stack;
}

void push_undo_state(SDL_Renderer* renderer, SDL_Texture *image_tex, SDL_FRect image_rect, float image_tex_width, float image_tex_height) {
    // Create a new undo state and push it onto the stack
    UndoState *state = malloc(sizeof(UndoState));
    if (!state) {
        printf("Failed to allocate memory for undo state\n");
        return;
    }

    // Copy current image texture and related info into the undo state
    state->image_tex = copy_texture(renderer, image_tex, image_tex_width, image_tex_height);
    state->image_rect = image_rect;
    state->image_tex_width = image_tex_width;
    state->image_tex_height = image_tex_height;
    undo_stack = push(undo_stack, state);
}

void undo(SDL_Renderer* renderer, SDL_Texture **image_tex, SDL_FRect *image_rect, float *image_tex_width, float *image_tex_height) {
    if (!undo_stack) {
        printf("Undo stack is empty, cannot undo\n");
        return;
    }

    // Pop the last state from the stack and restore it
    UndoNode *top = undo_stack;
    UndoState *state = top->state;

    // Restore the image texture and related info
    *image_tex = copy_texture(renderer, state->image_tex, state->image_tex_width, state->image_tex_height);
    *image_rect = state->image_rect;
    *image_tex_width = state->image_tex_width;
    *image_tex_height = state->image_tex_height;

    // Free the popped undo state and node
    free(state);
    undo_stack = top->next;
    free(top);
}

void free_undo_stack() {
    while (undo_stack) {
        UndoNode *temp = undo_stack;
        undo_stack = undo_stack->next;
        free(temp->state);
        free(temp);
    }
}



