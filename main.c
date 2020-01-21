#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "object.h"


static SDL_Window* _window     = NULL;
static SDL_Renderer* _renderer = NULL;
static gamestate gs;


/** the FIRST function to be called */
static int _init_sdl(int Width, int Height, SDL_Window** outWin,
                     SDL_Renderer** outRenderer);
static int _deinit_sdl();
static void update_state(unsigned int dt);
static int save_game_state();
static int load_game_state();


// return SDL_Rect of sprite at (col, row)
static SDL_Rect texture_rect(unsigned int col, unsigned int row)
{
        const int texwidth   = 80;
        const int texheight  = 40;
        const int rowcount   = 5;
        const int colcount   = 5;
        const int rectwidth  = texwidth / colcount;
        const int rectheight = texheight / rowcount;

        const int valid_index = (col < colcount) && (row < rowcount);
        if (!valid_index) { col = row = 0; }    // default value

        const int x = (col * rectwidth);
        const int y = (row * rectheight);

        SDL_Rect Result = {x, y, rectwidth, rectheight};
        return Result;
}

/** here we go! */
int main(int argc, char const* argv[])
{
#define SW 780
#define SH 800
        _init_sdl(SW, SH, &_window, &_renderer);
        assert(_window && _renderer);

        // gamestate init
        gs.ScreenWidth  = SW;
        gs.ScreenHeight = SH;
        gs.Running      = 1;

        // level
        gs.GroundLevel = (int) gs.ScreenHeight * 3.3 / 4;
        // TODO remove magic numbers
        // (16,8) is (width,height) of a tile texture
        gs.TileWidth  = 10 * 16;
        gs.TileHeight = 10 * 8;
        gs.TileXgap   = 1 * 16;
        gs.TileYgap   = 1 * 8;
#undef SW
#undef SH

        // player init
        gs.Player = calloc(1, sizeof(*gs.Player));
        assert(gs.Player);
        object* Player  = gs.Player;    // alias
        Player->Width   = 155;
        Player->Height  = 35;
        Player->Xpos    = gs.ScreenWidth / 2;
        Player->Ypos    = gs.GroundLevel;
        Player->Texture = load_texture("../assets/tiles.png");

// temp helper macro
#define TILE(x, y) {x, y, 100, 30, .Type = TILE, .State = IDLE, .Texture = NULL}
        tile _Tiles[] = {
            // X, Y, W, H, r,g,b
            TILE(0, 0),   TILE(110, 0),   TILE(220, 0),   TILE(330, 0),   TILE(440, 0),
            TILE(0, 40),  TILE(110, 40),  TILE(220, 40),  TILE(330, 40),  TILE(440, 40),
            TILE(0, 80),  TILE(110, 80),  TILE(220, 80),  TILE(330, 80),  TILE(440, 80),
            TILE(0, 120), TILE(110, 120), TILE(220, 120), TILE(330, 120), TILE(440, 120),
        };
#undef TILE    // Cannot be used after this

        gs.Tiles            = _Tiles;
        gs.TileCount        = 5 * 4;    // TODO: magic number
        gs.visual_debug     = 1;
        gs.highlighted_tile = NULL;

        // ball init
        object* Ball = calloc(1, sizeof(object));
        Ball->Width  = 25;
        Ball->Height = 25;
        Ball->Type   = BALL;
        Ball->State  = MOVING;


        while (gs.Running) {
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                        SDL_Keycode sym = ev.key.keysym.sym;
                        if (sym == SDLK_q || ev.type == SDL_QUIT) { gs.Running = 0; }

                        if (ev.type == SDL_KEYUP) {
                                // on key release
                                if (sym == SDLK_v) { gs.visual_debug = !gs.visual_debug; }
                                if (sym == SDLK_k) { save_game_state(); }
                                if (sym == SDLK_l) { load_game_state(); }
                        } else if (ev.type == SDL_KEYDOWN) {
                                // on key press
                                // RESET
                                if (sym == SDLK_r) {
                                        puts("MANUAL RESET");
                                        player_reset(Player, 200, gs.GroundLevel);
                                }
                        }
                }

                unsigned int dt = get_dt();

                // handle input and update state
                update_state(dt);


                // START DRAWING
                SDL_SetRenderDrawColor(_renderer, 50, 50, 50, 255);
                SDL_RenderClear(_renderer);

                // level
                {
                        for (int i = 0; i < gs.TileCount; ++i) {
                                const tile* Tile = &gs.Tiles[i];
                                SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
                                // if (gs.highlighted_tile == Tile) {
                                //         // paint it with player color
                                //         SDL_SetRenderDrawColor(_renderer, Player->r,
                                //                                Player->g, Player->b,
                                //                                255);
                                // }
                                SDL_Rect TileRect = RECT(Tile);
                                SDL_Rect TexRect  = texture_rect(0, 0);
                                SDL_RenderCopy(_renderer, Player->Texture, &TexRect,
                                               &TileRect);
                        }
                }


                // objects
                {
                        // player
                        SDL_Rect PlayerRect = RECT(Player);
                        SDL_Rect TexRect    = texture_rect(0, 1);
                        SDL_RenderCopy(_renderer, Player->Texture, &TexRect, &PlayerRect);

                        // ball
                        SDL_Rect BallRect = RECT(Ball);
                        SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
                        SDL_RenderFillRect(_renderer, &BallRect);
                }

                // visual debug
                if (gs.visual_debug) {
                        // draw green border around player
                        // SDL_SetRenderDrawColor(_renderer, 0, 200, 0, 255);
                        // SDL_Rect PlayerRect = RECT(Player);
                        // SDL_RenderDrawRect(_renderer, &PlayerRect);

                        // // draw velocity vector
                        // const int scale = 2;

                        // const int x1 = Player->Xpos + (Player->Width / 2);
                        // const int y1 = Player->Ypos + (Player->Height / 2);
                        // const int x2 = x1 + Player->Xspeed * scale;
                        // const int y2 = y1 + Player->Yspeed * scale;


                        // SDL_SetRenderDrawColor(_renderer, 255, 0, 255, 255);
                        // SDL_RenderDrawLine(_renderer, x1, y1, x2, y2);
                }


                SDL_RenderPresent(_renderer);
                // END DRAWING

                SDL_Delay(1000 / 60);    // fps
        }


        return 0;
}


static void update_state(unsigned int dt)
{
        const Uint8* Keys = SDL_GetKeyboardState(NULL);
        // START HANDLE INPUT
        // lateral movement
        object* Player = gs.Player;
        const int playerspeed = 15;
        if (Keys[SDL_SCANCODE_A]) {
                Player->Xpos -= playerspeed;    // move left
        }
        if (Keys[SDL_SCANCODE_D]) {
                Player->Xpos += playerspeed;    // move right
        }


        // actions
        if (Keys[SDL_SCANCODE_SPACE] && Player->State == IDLE) { puts("launch ball!"); }

        // collision detection
        if (LEFT(Player) < 0) { Player->Xpos = 0; }

        if (RIGHT(Player) > gs.ScreenWidth) {
                Player->Xpos = gs.ScreenWidth - Player->Width;
        }

        // END HANDLE INPUT

        // START STEP
        {
                object* Ball = &gs.Ball;
                // step ball
                static int xspeed = 10;
                static int yspeed = 9;
                Ball->Xpos += xspeed;
                Ball->Ypos += yspeed;
                if (LEFT(Ball) < 0) {
                        Ball->Xpos = 0;
                        xspeed     = -xspeed;
                }
                if (RIGHT(Ball) > gs.ScreenWidth) {
                        Ball->Xpos = gs.ScreenWidth - Ball->Width;
                        xspeed     = -xspeed;
                }
                if (TOP(Ball) < 0) {
                        Ball->Ypos = 0;
                        yspeed     = -yspeed;
                }
                if (BOTTOM(Ball) > gs.ScreenHeight) {
                        Ball->Ypos = gs.ScreenHeight - Ball->Height;
                        yspeed     = -yspeed;
                }
        }
        // END STEP
}

int _init_sdl(int Width, int Height, SDL_Window** outWin, SDL_Renderer** outRenderer)
{
        // init SDL proper
        int err = 0;
        err     = SDL_Init(SDL_INIT_EVERYTHING);
        if (err) {
                printf("SDL2 Error\n");
                exit(1);
        }
        err = SDL_CreateWindowAndRenderer(Width, Height, 0, outWin, outRenderer);
        if (err) {
                printf("SDL2 Window Error\n");
                _deinit_sdl();
        }

        // init sdl image
        const int flags   = IMG_INIT_JPG | IMG_INIT_PNG;
        const int initted = IMG_Init(flags);
        if ((initted & flags) != flags) {
                puts("Failed to init SDL_Image");
                _deinit_sdl();
        }

        return 0;
}

int _deinit_sdl()
{
        IMG_Quit();
        SDL_Quit();
        return 0;
}

/**
 * returns: dt in millisecs
 */
unsigned int get_dt()
{
        static unsigned int oldtime = 0;
        const unsigned int newtime  = SDL_GetTicks();
        const unsigned int dt       = newtime - oldtime;
        oldtime                     = newtime;
        return dt;
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


#define SAVEFILENAME "gamestate.bin"
// savefile format: [gamestate, object Player, object[TileCount] Tiles]
int save_game_state()
{
        // remove(SAVEFILENAME);
        // FILE* f = fopen(SAVEFILENAME, "wb");

        // // write gs
        // size_t size = fwrite(&gs, sizeof(gamestate), 1, f);
        // if (size != 1) { printf("save error1\n"); }

        // // write Player
        // size = fwrite(gs.Player, sizeof(*gs.Player), 1, f);
        // if (size != 1) { printf("save error2\n"); }

        // // write Tiles
        // size = fwrite(gs.Tiles, sizeof(tile), gs.TileCount, f);
        // if (size != gs.TileCount) { printf("save error3\n"); }

        // fclose(f);
        puts("GAME SAVED!");

        return 0;
}


int load_game_state()
{
        // FILE* f = fopen(SAVEFILENAME, "rb");
        // if (f == NULL) {
        //         // no save file
        //         return 1;
        // }

        // // read gamestate
        // size_t size = fread(&gs, sizeof(gamestate), 1, f);
        // if (size != 1) { printf("load error1\n"); }

        // // read Player
        // size = fread(gs.Player, sizeof(*gs.Player), 1, f);
        // if (size != 1) { printf("load error2\n"); }

        // // read Tiles
        // size = fread(gs.Tiles, sizeof(tile), gs.TileCount, f);
        // if (size != gs.TileCount) { printf("load error3\n"); }

        // fclose(f);
        puts("GAME LOADED!");
        return 0;
}
