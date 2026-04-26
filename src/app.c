#include "app.h"
#include "buttons.h"
#include "colors.h"
#include "mouse.h"
#include "undo.h"
#include "wayland/take_ss.h"

#define DEV_MODE 1 // Set to 1 to enable dev mode features (e.g., using local asset/config paths)

#if DEV_MODE
    #define ASSET_PATH "src/assets/"
    #define CONFIG_PATH "src/config/"
#else 
    #define ASSET_PATH "usr/local/share/sshot"
    #define CONFIG_PATH "~/.config/sshot" 
#endif

#define TITLE "SSHot" // Window title(may be changed later)
#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600
#define DEFAULT_BUTTON_SIZE 20

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
int window_width = 0;
int window_height = 0;
bool is_running = true;
SDL_Event event;

SDL_FRect current_rect = {0, 0, 0, 0}; // To store the current selection rectangle

Button *save_button;
Button *copy_button;
Button *fullscreen_button;
int fscreen_button_spacing = 20; // Spacing between buttons

SDL_Texture *image_tex = NULL;
SDL_FRect image_rect = {0, 0, 0, 0}; // To store the position and size of the loaded image
float image_tex_width, image_tex_height; 
float zoom_sens = 0.05; // Sensitivity for zooming in/out the image


bool is_drawing_selection_rect = false;
bool is_dragging_selection_rect = false;
bool is_mouse_over_buttons = false;
float start_x, start_y;

// Local function prototypes
bool initialize_window();
bool process_input(SDL_Event *event, Button *buttons[]);
void update();
void render();
bool load_assets();
void func1(ButtonType type); // Placeholder for button click functions
void on_fullscreen_button_click(ButtonType type);
void handle_window_resize();
void zoomin_image();
void zoomout_image();
void cut_image();

// Global functions (ALL CAPS)
int APP_INIT(void){
    // Take screenshot first
    char *ss_filepath = take_ss_wayland();
    if (!initialize_window()) {
        return APP_ERROR_INIT; 
    }

    SDL_PumpEvents();
    SDL_SyncWindow(window);  // Wait for Wayland compositor to configure the window
    // get window size
    SDL_GetWindowSizeInPixels(window, &window_width, &window_height);

    SDL_Surface* image_surface = IMG_Load(ss_filepath);

    if (image_surface == NULL) {
        fprintf(stderr, "Error loading image: %s\n", SDL_GetError());
        return APP_ERROR_INIT;
    }

    image_tex = SDL_CreateTextureFromSurface(renderer, image_surface);

    if (image_tex == NULL) {
        fprintf(stderr, "Error creating texture from surface: %s\n", SDL_GetError());
        SDL_DestroySurface(image_surface);
        return APP_ERROR_INIT;
    }

    image_rect.w = (float)image_surface->w; // Set initial width of the image
    image_rect.h = (float)image_surface->h; // Set initial height of the image

    SDL_GetTextureSize(image_tex, &image_tex_width, &image_tex_height);
    
    SDL_DestroySurface(image_surface);
    SDL_SetTextureScaleMode(image_tex, SDL_SCALEMODE_LINEAR);
    
    return APP_SUCCESS;

    
}

int APP_RUN(void) {

    // Create buttons
    save_button = create_button(renderer, SAVE, ASSET_PATH "save_icon.svg", 0, 0, DEFAULT_BUTTON_SIZE);
    copy_button = create_button(renderer, COPY, ASSET_PATH "copy_icon.svg", 0, 0, DEFAULT_BUTTON_SIZE);
    fullscreen_button = create_button(renderer, FULLSCREEN, ASSET_PATH "fullscreen_icon.svg", 0, 0, DEFAULT_BUTTON_SIZE);

    // Bind buttons to functions
    bind_button_to_function(save_button, func1);
    bind_button_to_function(copy_button, func1);
    bind_button_to_function(fullscreen_button, on_fullscreen_button_click);

    // Top right corner for fullscreen button
    fullscreen_button->rect.x = window_width - DEFAULT_BUTTON_SIZE - fscreen_button_spacing;
    fullscreen_button->rect.y = fscreen_button_spacing;

    Button *buttons[] = {save_button, copy_button, fullscreen_button};

    // Main loop
    while (is_running) {
        is_running = process_input(&event, buttons);
        update();
        render();
    }
    return APP_SUCCESS;
}

void APP_QUIT() { 
    // Destroy buttons
    destroy_button(save_button);
    destroy_button(copy_button);
    destroy_button(fullscreen_button);

    // Free loaded image
    if (image_tex) {
        SDL_DestroyTexture(image_tex);
        image_tex = NULL;
    }

    // Free undo stack
    free_undo_stack();
    
    // Destroy renderer, window and quit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

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
    // case FULLSCREEN:
    //     printf("Fullscreen button clicked!\n");
    //     break;
    default:
        printf("Button of type %d clicked!\n", type);
        break;
    }
}

void on_fullscreen_button_click(ButtonType type) {
    // Make the current rect as the same as the image rect
    current_rect = image_rect; 
}

bool initialize_window() {
    if (SDL_Init(SDL_INIT_VIDEO) != true) {
           fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
           return false;
    }
    
    if (SDL_CreateWindowAndRenderer(TITLE, 1920, 1080, SDL_WINDOW_FULLSCREEN, &window, &renderer) != true) {
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
                    SDL_Keymod mod = event->key.mod;
                    SDL_Keycode key = event->key.key;

                    bool ctrl = (mod & SDL_KMOD_CTRL) != 0;

                    if (key == SDLK_ESCAPE || key == SDLK_Q) {
                        return false;
                    }
                    if (ctrl && key == SDLK_Z) {
                        undo(renderer, &image_tex, &image_rect, &image_tex_width, &image_tex_height);
                    }
                    if (ctrl && key == SDLK_C){
                        //TODO: Implement copy functionality
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
                if (event->button.button == SDL_BUTTON_RIGHT) {
                    // for now
                    cut_image();
                }
                break;
            
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                window_width = event->window.data1;
                window_height = event->window.data2;

                // Update fullscreen button position
                fullscreen_button->rect.x = window_width - DEFAULT_BUTTON_SIZE - fscreen_button_spacing;
                fullscreen_button->rect.y = fscreen_button_spacing;

                // Re-center the image
                image_rect.x = (window_width - image_rect.w) / 2; // Center the image
                image_rect.y = (window_height - image_rect.h) / 2;

                break;
                
            case SDL_EVENT_MOUSE_WHEEL:
                if (event->wheel.y > 0) {
                    // Zoom in
                    image_rect.w = image_rect.w * zoom_sens + image_rect.w; // Increase width by a percentage
                    image_rect.h = image_rect.h * zoom_sens + image_rect.h;

                    zoom_sens = (zoom_sens < 0.30) ? zoom_sens + 0.02 : 0.30;
                } else if (event->wheel.y < 0) {
                    // Zoom out, but prevent the image from becoming too small
                    if (image_rect.w > zoom_sens && image_rect.h > zoom_sens) {
                        image_rect.w = image_rect.w - image_rect.w * zoom_sens; // Decrease width by a percentage
                        image_rect.h = image_rect.h - image_rect.h * zoom_sens; 

                        zoom_sens = (zoom_sens > 0.10) ? zoom_sens - 0.02 : 0.10;
                    }
                }
                // Re-center the image after zooming
                image_rect.x = (window_width - image_rect.w) / 2; // Center the image
                image_rect.y = (window_height - image_rect.h) / 2;
                break;
            
            }
        }

    return true;
}

void update() {
    return;
}

void render() {
    SDL_SetRenderDrawColorStruct(renderer, BACKGROUND_COLOR);
    SDL_RenderClear(renderer);
    
    // Render the loaded image
    if (image_tex) {
        SDL_RenderTexture(renderer, image_tex, NULL, &image_rect);
    }

    // Draw selection rectangle
    if (is_drawing_selection_rect || is_dragging_selection_rect || (current_rect.w != 0 && current_rect.h != 0)) {
        // Draw the outline
        SDL_SetRenderDrawColorStruct(renderer, COLOR_SEMI_TRANSPARENT_BLUE);
        SDL_RenderRect(renderer, &current_rect);
    }

    // Calculate button positions based on current_rect
    if (current_rect.w != 0 && current_rect.h != 0) {
        // Position the buttons near the selection rectangle
        copy_button->rect.x = current_rect.x + current_rect.w - copy_button->rect.w; 
        copy_button->rect.y = current_rect.y + current_rect.h + 5; // 5 pixels below the rectangle
        save_button->rect.x = current_rect.x + current_rect.w - save_button->rect.w - copy_button->rect.w - 5; // 5 pixels to the left of the copy button
        save_button->rect.y = current_rect.y + current_rect.h + 5; // 5 pixels below the rectangle
    } else {
        // If there is no selection rectangle yet, position the buttons at the bottom right corner
        copy_button->rect.x = window_width - copy_button->rect.w;
        copy_button->rect.y = window_height - copy_button->rect.h - 5;
        save_button->rect.x = window_width - save_button->rect.w - copy_button->rect.w - 5;
        save_button->rect.y = window_height - save_button->rect.h - 5;

    }
    // Draw buttons
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    render_button(renderer, save_button);
    render_button(renderer, copy_button);

    //Render fullscreen button always
    render_button(renderer, fullscreen_button);

    // Present the rendered frame to the screen
    SDL_RenderPresent(renderer);
    return;
}

void cut_image() {
    if (current_rect.w <= 0 || current_rect.h <= 0) return;

    // If the current_rect is partially outside the image_rect, we need to adjust it and also calculate the corresponding source rect for cropping
    if (current_rect.x < image_rect.x) {
        current_rect.w -= (image_rect.x - current_rect.x);
        current_rect.x = image_rect.x;
    }

    if (current_rect.y < image_rect.y) {
        current_rect.h -= (image_rect.y - current_rect.y);
        current_rect.y = image_rect.y;
    }

    if (current_rect.x + current_rect.w > image_rect.x + image_rect.w) {
        current_rect.w = (image_rect.x + image_rect.w) - current_rect.x;
    }

    if (current_rect.y + current_rect.h > image_rect.y + image_rect.h) {
        current_rect.h = (image_rect.y + image_rect.h) - current_rect.y;
    }

    // Convert current_rect from screen coords to texture coords
    float scale_x = image_tex_width / image_rect.w;
    float scale_y = image_tex_height / image_rect.h;

    SDL_FRect src = {
        (current_rect.x - image_rect.x) * scale_x,
        (current_rect.y - image_rect.y) * scale_y,
        current_rect.w * scale_x,
        current_rect.h * scale_y
    };

    SDL_Texture* cropped_tex = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        (int)src.w,
        (int)src.h
    );

    if (!cropped_tex) {
        printf("Failed to create cropped texture: %s\n", SDL_GetError());
        return;
    }

    SDL_Texture* prev_target = SDL_GetRenderTarget(renderer);

    SDL_SetRenderTarget(renderer, cropped_tex);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_FRect dst = { 0, 0, src.w, src.h };
    SDL_RenderTexture(renderer, image_tex, &src, &dst);  // <-- src, not current_rect

    SDL_SetRenderTarget(renderer, prev_target);

    push_undo_state(renderer, image_tex, image_rect, image_tex_width, image_tex_height);
    SDL_DestroyTexture(image_tex);
    image_tex = cropped_tex;

    image_tex_width = src.w;
    image_tex_height = src.h;

    image_rect.w = current_rect.w;
    image_rect.h = current_rect.h;
    image_rect.x = (window_width - image_rect.w) / 2;
    image_rect.y = (window_height - image_rect.h) / 2;

    // current_rect = (SDL_FRect){ 0, 0, 0, 0 };
    current_rect = image_rect; // Set current_rect to the new image_rect after cutting
}

// load and create textures, initialize variables, etc.
bool load_assets() {
    // TODO
    return true;
}

