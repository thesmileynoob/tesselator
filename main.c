#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "object.h"


const int ScreenWidth  = 1280;
const int ScreenHeight = 720;

const int GroundLevel         = (int) ScreenHeight * 2 / 4;
static int visual_debug       = 1;
static tile* highlighted_tile = NULL;

SDL_Window* window     = NULL;
SDL_Renderer* renderer = NULL;


tile Tiles[] = {
    // X, Y, W, H, r,g,b
    {100, GroundLevel, 200, 10, .Xspeed = 5, .Yspeed = .5, .Type = TILE, .r = 0, .g = 0,
     .b = 200},
    {300, GroundLevel + 200, 200, 10, .Type = TILE, .r = 0, .g = 0, .b = 200},
    {200, GroundLevel + 300, 500, 10, .Type = TILE, .r = 0, .g = 0, .b = 200},
    //     {500, GroundLevel - 200, 100, 200, /*rgb*/ 150, 50, 0},
    //     {300, GroundLevel - 100, 100, 100, /*rgb*/ 150, 50, 0},
};


static int init_sdl();
static int deinit_sdl();
static void handle_input(const Uint8* keys, object* player, unsigned int dt);
static void step_player(object* Obj, unsigned int dt);
static void step_tile(tile* Obj, unsigned int dt);
static unsigned int get_dt();
static SDL_Texture* load_texture(const char* path);

inline static int tile_count() { return sizeof(Tiles) / sizeof(tile); }


int main(int argc, char const* argv[])
{
        init_sdl();
        assert(window && renderer);

        // player init
        object Player;
        player_reset(&Player, 200, GroundLevel);
        Player.Texture = load_texture("../assets/dude.png");


        int running = 1;
        while (running) {
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                        SDL_Keycode sym = ev.key.keysym.sym;
                        if (sym == SDLK_q || ev.type == SDL_QUIT) { running = 0; }

                        if (ev.type == SDL_KEYUP) {
                                // on key release
                                if (sym == SDLK_v) { visual_debug = !visual_debug; }
                        } else if (ev.type == SDL_KEYDOWN) {
                                // on key press
                                // RESET
                                if (sym == SDLK_r) {
                                        puts("MANUAL RESET");
                                        player_reset(&Player, 200, GroundLevel);
                                }
                        }
                }

                unsigned int dt = get_dt();

                // input
                handle_input(SDL_GetKeyboardState(NULL), &Player, dt);
                step_player(&Player, dt);
                step_tile(&Tiles[0], dt);

                // START DRAWING
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderClear(renderer);

                // level
                {
                        const int tc = tile_count();
                        for (int i = 0; i < tc; ++i) {
                                tile* Tile = &Tiles[i];
                                SDL_SetRenderDrawColor(renderer, Tile->r, Tile->g,
                                                       Tile->b, 255);

                                if (highlighted_tile == Tile) {
                                        // paint it with player color
                                        SDL_SetRenderDrawColor(renderer, Player.r,
                                                               Player.g, Player.b, 255);
                                }
                                const SDL_Rect TileRect = RECT(Tile);
                                SDL_RenderFillRect(renderer, &TileRect);
                                SDL_RenderDrawRect(renderer, &TileRect);
                        }
                }


                // objects
                {
                        // player
                        SDL_Rect PlayerRect = RECT((&Player));
                        SDL_Rect TexRect    = {0, 0, 512 / 8, 576 / 9};
                        // SDL_RenderCopy(renderer, Player.Texture, &TexRect,
                        // &PlayerRect);
                        SDL_RenderCopyEx(renderer, Player.Texture, &TexRect, &PlayerRect,
                                         0, NULL, !Player.FaceRight);
                }

                // visual debug
                if (visual_debug) {
                        // draw green border around player
                        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
                        SDL_Rect PlayerRect = RECT((&Player));
                        SDL_RenderDrawRect(renderer, &PlayerRect);

                        // draw velocity vector
                        const int scale = 2;

                        const int x1 = Player.Xpos + (Player.Width / 2);
                        const int y1 = Player.Ypos + (Player.Height / 2);
                        const int x2 = x1 + Player.Xspeed * scale;
                        const int y2 = y1 + Player.Yspeed * scale;


                        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                }


                SDL_RenderPresent(renderer);
                // END DRAWING

                SDL_Delay(1000 / 60);    // fps
        }


        return 0;
}


void step_tile(tile* Obj, unsigned int dt)
{
        const int new_Xpos = Obj->Xpos + Obj->Xspeed;
        const int new_Ypos = Obj->Ypos + Obj->Yspeed;
        Obj->Xpos          = new_Xpos;
        Obj->Ypos          = new_Ypos;
}


void step_player(object* Obj, unsigned int dt)
{
        // check if there's a tile below object
        highlighted_tile = tile_below_object(Obj, Tiles, tile_count()); // NULLABLE

        // TODO: get_tile_{left, right, top}
        // Collect data required for the step_player
        const tile* TileBelow = highlighted_tile;    // NULLABLE
        const int gravity     = 2;

        // new values to be assigned to the boject
        int new_Xpos, new_Ypos;
        new_Xpos = Obj->Xpos + Obj->Xspeed;

        // apply gravity
        Obj->Yspeed += gravity;
        new_Ypos = Obj->Ypos + Obj->Yspeed;

        if (Obj->State == IDLE) {
                // printf("%d IDLE\n", dt);
        }

        if (Obj->State == JUMPING) {
                if (Obj->Yspeed < 0) {
                        // rising up
                        // puts("JUMPING: rising up");
                        int amt = abs(Obj->Yspeed);
                        new_Ypos -= amt;
                } else {
                        // falling down
                        // puts("JUMPING: falling down");
                        int amt = abs(Obj->Yspeed);
                        new_Ypos += amt;
                }
        }


        // COLLISION HANDLING
        // NOTE: Update positions *HERE* only
        {
                if (TileBelow) {
                        const int new_obj_bottom = new_Ypos + Obj->Height;
                        const int will_intersect_tile_below =
                            (new_obj_bottom > TOP(TileBelow));
                        if (will_intersect_tile_below) {
                                // stand on the tile.
                                // set proper new_Ypos
                                new_Ypos    = TOP(TileBelow) - Obj->Height;
                                Obj->Yspeed = 0;
                                Obj->State  = IDLE;
                        }
                }
        }

        // NOTE: *SET* Obj->values here
        Obj->Xpos = new_Xpos;
        Obj->Ypos = new_Ypos;


        // win/lose condition
        {
                const int obj_bottom = Obj->Ypos + Obj->Height;
                if (obj_bottom >= ScreenHeight) {
                        puts("AUTO-RESET");
                        if (Obj->Type == PLAYER) player_reset(Obj, 200, GroundLevel);
                }
        }
}

static void handle_input(const Uint8* Keys, object* Player, unsigned int dt)
{
        // NOTE:
        //      DO *NOT* UPDATE POSITIONS (*pos) IN THIS FUNCITON!
        //      *ONLY* UPDATE STATES and SPEEDS!

        // lateral movement
        if (Keys[SDL_SCANCODE_A]) {
                Player->Xspeed    = -15;    // go left
                Player->FaceRight = 0;      // face left
        } else if (Keys[SDL_SCANCODE_D]) {
                Player->Xspeed    = +15;    // go right
                Player->FaceRight = 1;      // face rigth
        } else {
                Player->Xspeed = 0;
        }

        // vertical movement
        if (Keys[SDL_SCANCODE_SPACE] && Player->State == IDLE) {
                Player->State  = JUMPING;
                Player->Yspeed = -Player->JumpSpeed;
        }
}

int init_sdl()
{
        // init SDL proper
        int err = 0;
        err     = SDL_Init(SDL_INIT_EVERYTHING);
        if (err) {
                printf("SDL2 Error\n");
                exit(1);
        }
        err =
            SDL_CreateWindowAndRenderer(ScreenWidth, ScreenHeight, 0, &window, &renderer);
        if (err) {
                printf("SDL2 Window Error\n");
                deinit_sdl();
        }

        // init sdl image
        const int flags   = IMG_INIT_JPG | IMG_INIT_PNG;
        const int initted = IMG_Init(flags);
        if ((initted & flags) != flags) {
                puts("Failed to init SDL_Image");
                deinit_sdl();
        }


        return 0;
}

int deinit_sdl()
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

static SDL_Texture* load_texture(const char* path)
{
        SDL_Surface* surf = IMG_Load(path);
        if (surf == NULL) {
                printf("Failed to load image: %s\n", path);
                deinit_sdl();
                exit(1);
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
        if (texture == NULL) {
                printf("Failed to convert SDL_Surface to SDL_Texture: %s\n", path);
                printf("ERROR: %s\n", SDL_GetError());
                SDL_FreeSurface(surf);
                deinit_sdl();
                exit(1);
        }
        SDL_FreeSurface(surf);
        return texture;
}
