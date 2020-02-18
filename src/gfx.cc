#include "gfx.h"
#include "breakout.h"

#include <ctime>

namespace gfx
{
// env
SDL_Window* _window;
SDL_Renderer* _renderer;

// window
int _SCR_WIDTH;
int _SCR_HEIGHT;

// textures
SDL_Texture* TileTexture;
SDL_Texture* BgTexture;
int BgTexture_width;
int BgTexture_height;

// fonts
TTF_Font* UIFont;


// sdl
static int _init_sdl();
static int _deinit_sdl();


static int gen_random_num(int upto)
{
    // seed
    srand(time(0));
    int i = rand() % upto;
    return i;
}

void init()
{
    _init_sdl();
    assert(_window && _renderer);

    // load textures

    // choose a random tileset from "assets/tilesets/"
    {
        int i = gen_random_num(TILESET_COUNT);
        if (i == 0) i = 1;  // "Breakout-000-C.png" doesn't exist
        assert(i > 0);
        assert(i < TILESET_COUNT);

        // create file name string
        const int max_len = 256;
        char ts_filename[max_len];  // eg: "Breakout-007-C.png"
        snprintf(ts_filename, max_len, "Breakout-%03d-C.png", i);

        // create path string
        char ts_path[(2 * max_len)];
        snprintf(ts_path, (2 * max_len), "../assets/tilesets/%s", ts_filename);

        TileTexture = gfx::load_texture(ts_path);
        assert(TileTexture);

        printf("loaded tileset texture: %s (%d x %d)...\n", ts_filename, TILE_WIDTH,
               TILE_HEIGHT);
    }

    // choose a random background from "assets/bg/"
    {
        // i = [1, 16]
        srand(time(0));
        int i = rand() % BACKGROUND_TILE_COUNT;
        if (i == 0) i = 1;  // "0.png" doesn't exist
        assert(i > 0);
        assert(i < BACKGROUND_TILE_COUNT);

        // create file name string
        const int max_len = 256;
        char bg_filename[max_len];  // eg: "12.png"
        snprintf(bg_filename, max_len, "%d.png", i);

        // create path string
        char bg_path[(2 * max_len)];
        snprintf(bg_path, (2 * max_len), "../assets/bg/%s", bg_filename);

        // load background texture
        BgTexture = gfx::load_texture(bg_path);
        assert(BgTexture);

        // load its width and height
        SDL_QueryTexture(BgTexture, NULL, NULL, &BgTexture_width, &BgTexture_height);

        printf("loaded background texture: %s (%d x %d)...\n", bg_filename,
               BgTexture_width, BgTexture_height);
    }

    // load fonts
    UIFont = TTF_OpenFont("../assets/font_04b.ttf", 25);
    assert(UIFont);
}

void deinit() { _deinit_sdl(); }

void draw_texture(SDL_Texture* t, SDL_Rect* src, SDL_Rect* dst)
{
    SDL_RenderCopy(_renderer, t, src, dst);
}


void draw_crosshair(vec2 center, int width, int height)
{
    const int half_width  = width / 2;
    const int half_height = height / 2;

    // horiz line
    SDL_RenderDrawLine(_renderer, center.X - half_width, center.Y, center.X + half_width,
                       center.Y);
    // vertical line
    SDL_RenderDrawLine(_renderer, center.X, center.Y - half_height, center.X,
                       center.Y + half_height);
}

SDL_Texture* load_texture(const char* path)
{
    SDL_Surface* surf = IMG_Load(path);
    if (surf == NULL) {
        printf("Failed to load image: %s\n", path);
        _deinit_sdl();
        exit(1);
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surf);
    if (texture == NULL) {
        printf("Failed to convert SDL_Surface to SDL_Texture: %s\n", path);
        printf("ERROR: %s\n", SDL_GetError());
        SDL_FreeSurface(surf);
        _deinit_sdl();
        exit(1);
    }
    SDL_FreeSurface(surf);
    return texture;
}


// return SDL_Rect of sprite at (col, row)
SDL_Rect texture_rect(unsigned int col, unsigned int row)
{
    /// TODO: Move these to breakout.h
    const int texwidth   = 320;
    const int texheight  = 160;
    const int rowcount   = 5;
    const int colcount   = 5;
    const int rectwidth  = texwidth / colcount;
    const int rectheight = texheight / rowcount;

    const int valid_index = (col < colcount) && (row < rowcount);
    if (!valid_index) { col = row = 0; }  // default value

    const int x = (col * rectwidth);
    const int y = (row * rectheight);

    SDL_Rect Result = {x, y, rectwidth, rectheight};
    return Result;
}

// create SDL_Window and SDL_Renderer
int _init_sdl()
{
    // SDL proper
    int err = 0;
    err     = SDL_Init(SDL_INIT_EVERYTHING);
    if (err) {
        printf("SDL2 Error\n");
        exit(1);
    }
    _window =
        SDL_CreateWindow("Breakout Clone", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    assert(_window);

    _renderer = SDL_CreateRenderer(_window, -1, 0);
    assert(_renderer);

    // set SCR_WIDTH and SCR_HEIGHT
    SDL_GetWindowSize(_window, &_SCR_WIDTH, &_SCR_HEIGHT);


    // sdl image
    const int flags   = IMG_INIT_JPG | IMG_INIT_PNG;
    const int initted = IMG_Init(flags);
    if ((initted & flags) != flags) {
        puts("Failed to init SDL_Image");
        _deinit_sdl();
        exit(1);
    }

    // sdl ttf

    err = TTF_Init();
    if (err) {
        puts("Failed to init SDL_ttf");
        _deinit_sdl();
        exit(1);
    }


    return 0;
}

int _deinit_sdl()
{
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
    return 0;
}

}  // namespace gfx
