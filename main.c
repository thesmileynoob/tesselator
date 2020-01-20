#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "object.h"


SDL_Window* window     = NULL;
SDL_Renderer* renderer = NULL;

static gamestate gs;


int init_sdl(int Width, int Height, SDL_Window** outWin, SDL_Renderer** outRenderer);
static int deinit_sdl();
static void handle_input(const Uint8* keys, object* player, unsigned int dt);
static void step_player(object* Obj, unsigned int dt);
static void step_tile(tile* Obj, unsigned int dt);
static unsigned int get_dt();
static SDL_Texture* load_texture(const char* path);


// use this to change velocities
inline static void apply_force(object* Obj, int fx, int fy)
{
        Obj->Xspeed += fx;
        Obj->Yspeed += fy;
}

// use this to set positions
inline static void set_pos(object* Obj, int x, int y)
{
        Obj->Xpos = x;
        Obj->Ypos = y;
}


int main(int argc, char const* argv[])
{
        init_sdl(1280, 720, &window, &renderer);
        assert(window && renderer);

        // gamestate
        gs.ScreenWidth  = 1280;
        gs.ScreenHeight = 720;
        gs.GroundLevel  = (int) gs.ScreenHeight * 2 / 4;
        gs.Gravity      = 2;
        gs.Running      = 1;

        // player init
        gs.Player      = calloc(1, sizeof(*gs.Player));
        object* Player = gs.Player;    // alias
        player_reset(Player, /*x*/ 200, /*y*/ gs.GroundLevel);
        Player->Texture = load_texture("../assets/dude.png");

// temp helper macro
#define TILE(x, y, w, xs, ys) {x, y, w, 15, xs, ys, .Type = TILE, .b = 200}

        tile _Tiles[] = {
            // X, Y, W, H, r,g,b
            TILE(100, gs.GroundLevel - 200, 500, 4, 2),
            TILE(300, gs.GroundLevel - 100, 300, 1, -1),
            TILE(200, gs.GroundLevel, 600, -2, 0),
            TILE(200, gs.GroundLevel + 130, 200, -2, 0),
            TILE(200, gs.GroundLevel + 210, 100, -2, 0),
        };
#undef TILE    // Cannot be used after this

        gs.Tiles            = _Tiles;
        gs.TileCount        = 5;
        gs.visual_debug     = 1;
        gs.highlighted_tile = NULL;


        while (gs.Running) {
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                        SDL_Keycode sym = ev.key.keysym.sym;
                        if (sym == SDLK_q || ev.type == SDL_QUIT) { gs.Running = 0; }

                        if (ev.type == SDL_KEYUP) {
                                // on key release
                                if (sym == SDLK_v) { gs.visual_debug = !gs.visual_debug; }
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

                // input
                handle_input(SDL_GetKeyboardState(NULL), Player, dt);

                // step
                {
                        step_player(Player, dt);
                        for (int i = 0; i < gs.TileCount; i++) {
                                step_tile(&gs.Tiles[i], dt);
                        }
                }


                // START DRAWING
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderClear(renderer);

                // level
                {
                        for (int i = 0; i < gs.TileCount; ++i) {
                                tile* Tile = &gs.Tiles[i];
                                SDL_SetRenderDrawColor(renderer, Tile->r, Tile->g,
                                                       Tile->b, 255);

                                if (gs.highlighted_tile == Tile) {
                                        // paint it with player color
                                        SDL_SetRenderDrawColor(renderer, Player->r,
                                                               Player->g, Player->b, 255);
                                }
                                const SDL_Rect TileRect = RECT(Tile);
                                SDL_RenderFillRect(renderer, &TileRect);
                                SDL_RenderDrawRect(renderer, &TileRect);
                        }
                }


                // objects
                {
                        // player
                        SDL_Rect PlayerRect = RECT(Player);
                        SDL_Rect TexRect    = {0, 0, 512 / 8, 576 / 9};
                        SDL_RenderCopyEx(renderer, Player->Texture, &TexRect, &PlayerRect,
                                         0, NULL, !Player->FaceRight);
                }

                // visual debug
                if (gs.visual_debug) {
                        // draw green border around player
                        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
                        SDL_Rect PlayerRect = RECT(Player);
                        SDL_RenderDrawRect(renderer, &PlayerRect);

                        // draw velocity vector
                        const int scale = 2;

                        const int x1 = Player->Xpos + (Player->Width / 2);
                        const int y1 = Player->Ypos + (Player->Height / 2);
                        const int x2 = x1 + Player->Xspeed * scale;
                        const int y2 = y1 + Player->Yspeed * scale;


                        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                }


                SDL_RenderPresent(renderer);
                // END DRAWING

                SDL_Delay(1000 / 60);    // fps
        }


        return 0;
}


void step_tile(tile* Tile, unsigned int dt)
{
        int new_Xpos = Tile->Xpos + Tile->Xspeed;
        int new_Ypos = Tile->Ypos + Tile->Yspeed;

        const int new_right_edge  = new_Xpos + Tile->Width;
        const int new_left_edge   = new_Xpos;
        const int new_top_edge    = new_Ypos;
        const int new_bottom_edge = new_Ypos + Tile->Height;

        // collision
        if (new_right_edge >= gs.ScreenWidth) {
                Tile->Xspeed = -Tile->Xspeed;
                new_Xpos     = new_right_edge - Tile->Width;
        }
        if (new_left_edge < 0) {
                Tile->Xspeed = -Tile->Xspeed;
                new_Xpos     = 0;
        }

        // TODO: define magic numbers(30 etc.) SOMEWHERE!
        if (new_top_edge < 30) {
                Tile->Yspeed = -Tile->Yspeed;
                new_Ypos     = 30;
        }
        if (new_bottom_edge >= gs.ScreenHeight - 30) {
                Tile->Yspeed = -Tile->Yspeed;
                new_Ypos     = gs.ScreenHeight - 30 - Tile->Height;
        }

        // update position
        set_pos(Tile, new_Xpos, new_Ypos);
}


void step_player(object* Obj, unsigned int dt)
{
        // check if there's a tile below object
        gs.highlighted_tile =
            tile_below_object(Obj, gs.Tiles, gs.TileCount);    // NULLABLE

        // Collect data required for the step_player
        const tile* TileBelow = gs.highlighted_tile;    // NULLABLE

        // new values to be assigned to the boject
        int new_Xpos, new_Ypos;
        new_Xpos = Obj->Xpos + Obj->Xspeed;

        // apply gravity
        apply_force(Obj, 0, gs.Gravity);
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

        // update positons
        set_pos(Obj, new_Xpos, new_Ypos);

        // win/lose condition
        {
                if (BOTTOM(Obj) >= gs.ScreenHeight) {
                        puts("AUTO-RESET");
                        if (Obj->Type == PLAYER) player_reset(Obj, 200, gs.GroundLevel);
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

int init_sdl(int Width, int Height, SDL_Window** outWin, SDL_Renderer** outRenderer)
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
