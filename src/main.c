#include "main.h"

#define DEV_MODE 1 // Set to 1 to enable dev mode features (e.g., using local asset/config paths)

#if DEV_MODE
    #define ASSET_PATH "src/assets/"
    #define CONFIG_PATH "src/config/"
#else 
    #define ASSET_PATH "usr/local/share/sshot"
    #define CONFIG_PATH "~/.config/sshot" 
#endif

#define TITLE "SSHot" // Window title(may be changed later)
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_BUTTON_SIZE 20

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

SDL_FRect current_rect = {0, 0, 0, 0}; // To store the current selection rectangle

Button *save_button;
Button *copy_button;
Button *fullscreen_button;
int fscreen_button_spacing = 20; // Spacing between buttons



bool is_drawing_selection_rect = false;
bool is_dragging_selection_rect = false;
bool is_mouse_over_buttons = false;
float start_x, start_y;

void func1(ButtonType type) {
    // Placeholder for a function that will be called when a button is clicked
    switch (type)
    {
    case SAVE:
        printf("Save button clicked!\n");
        break;
    case COPY:
        printf("Copy button clicked!\n");
        break;
    case FULLSCREEN:
        printf("Fullscreen button clicked!\n");
        break;
    default:
        printf("Button of type %d clicked!\n", type);
        break;
    }
}

int main(void) {
    if (!initialize_window()) {
        return 1; 
    }
    bool is_running = true;
    SDL_Event event;    

    // Create buttons
    save_button = create_button(renderer, SAVE, ASSET_PATH "save_icon.svg", 0, 0, DEFAULT_BUTTON_SIZE);
    copy_button = create_button(renderer, COPY, ASSET_PATH "copy_icon.svg", 0, 0, DEFAULT_BUTTON_SIZE);
    fullscreen_button = create_button(renderer, FULLSCREEN, ASSET_PATH "fullscreen_icon.svg", 0, 0, DEFAULT_BUTTON_SIZE);

    // Bind buttons to functions
    bind_button_to_function(save_button, func1);
    bind_button_to_function(copy_button, func1);
    bind_button_to_function(fullscreen_button, func1);

    // Top right corner for fullscreen button
    fullscreen_button->rect.x = DEFAULT_WIDTH - DEFAULT_BUTTON_SIZE - fscreen_button_spacing;
    fullscreen_button->rect.y = fscreen_button_spacing;

    Button *buttons[] = {save_button, copy_button, fullscreen_button};

    // Main loop
    while (is_running) {
        is_running = process_input(&event, buttons);
        update();
        render();
    }
    QUIT();
    
    return 0;
}

bool initialize_window() {
    if (SDL_Init(SDL_INIT_VIDEO) != true) {
           fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
           return false;
    }
    
    if (SDL_CreateWindowAndRenderer(TITLE,DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer) != true) {
        fprintf(stderr, "Error creating window and renderer: %s\n", SDL_GetError());
        return false;
    }

    if (window == NULL || renderer == NULL) {
        fprintf(stderr, "Error creating window or renderer: %s\n", SDL_GetError());
        return false;
    }

    if (SDL_SetRenderVSync(renderer, 1) != true) {
        fprintf(stderr, "Error enabling VSync: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

bool process_input(SDL_Event *event, Button *buttons[]) {
    while (SDL_PollEvent(event))
        {
            switch (event->type)
            {
              case SDL_EVENT_QUIT:
                    return false;
                    break;
                case SDL_EVENT_KEY_UP:
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event->key.key == SDLK_ESCAPE || event->key.key == SDLK_Q) {
                        return false;
                    }   
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event->button.button == SDL_BUTTON_LEFT) {
                    mouse_left_button_down(event, &is_drawing_selection_rect, &is_dragging_selection_rect, &start_x, &start_y, &current_rect, buttons);
                }
                break;

            case SDL_EVENT_MOUSE_MOTION:
                if (is_drawing_selection_rect || is_dragging_selection_rect) {
                    // Calculate width/height based on current mouse pos
                    mouse_motion(event, &is_drawing_selection_rect, &is_dragging_selection_rect, &start_x, &start_y, &current_rect);
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event->button.button == SDL_BUTTON_LEFT) {
                    mouse_left_button_up(event, &is_drawing_selection_rect, &is_dragging_selection_rect, &start_x, &start_y, &current_rect, &save_button->rect, &copy_button->rect);
                }
                break;
            }
        }

    return true;
}

void update() {
    return;
}

void render() {
    // Clear the screen with a color (e.g., black)

    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
    SDL_RenderClear(renderer);

    if (is_drawing_selection_rect || is_dragging_selection_rect || (current_rect.w != 0 && current_rect.h != 0)) {
        // Draw the outline (Bright Blue)
        SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
        SDL_RenderRect(renderer, &current_rect);

        if (!is_drawing_selection_rect && !is_dragging_selection_rect) {
            // Draw the buttons only when not drawing
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            render_button(renderer, save_button);
            render_button(renderer, copy_button);
        }
    }

    //Render fullscreen button always
    render_button(renderer, fullscreen_button);

    // Present the rendered frame to the screen
    SDL_RenderPresent(renderer);
    return;
}

// load and create textures, initialize variables, etc.
bool load_assets() {
    // TODO
    return true;
}

void QUIT() { 
    // Destroy buttons
    destroy_button(save_button);
    destroy_button(copy_button);
    destroy_button(fullscreen_button);
    
    // Destroy renderer, window and quit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}