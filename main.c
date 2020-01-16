#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "object.h"


const int ScreenWidth  = 1280;
const int ScreenHeight = 720;

const int GroundLevel        = (int) ScreenHeight * 3.5 / 4;
static int visual_debug      = 1;
static int highlight_tile_id = -1;    // -1 == no highlight

SDL_Window* window     = NULL;
SDL_Renderer* renderer = NULL;


tile Tiles[] = {
    // X, Y, W, H, r,g,b
    {200, GroundLevel, ScreenWidth - 400, 10, /*rgb*/ 0, 0, 200},
    {500, GroundLevel - 200, 100, 200, /*rgb*/ 150, 50, 0},
    {300, GroundLevel - 100, 100, 100, /*rgb*/ 150, 50, 0},
};

static int init_sdl();
static int deinit_sdl();
static void handle_input(const Uint8* keys, object* player, unsigned int dt);
static unsigned int get_dt();
static void reset_player(object* Player)
{
        object result = {0};
        result.Width  = 25;
        result.Height = 50;
        result.Xpos   = 200;
        result.Ypos   = GroundLevel - result.Height;
        result.State  = STANDING;

        result.Xspeed    = 15;
        result.Yspeed    = 0;
        result.JumpSpeed = 1.2 * result.Height;
        result.r         = 255;

        *Player = result;
}

inline static int tile_count() { return sizeof(Tiles) / sizeof(tile); }
inline static SDL_Rect tile_rect(const tile* Tile)
{
        return (SDL_Rect){Tile->Xpos, Tile->Ypos, Tile->Width, Tile->Height};
}


int main(int argc, char const* argv[])
{
        init_sdl();
        assert(window && renderer);

        // player init
        object Player;
        reset_player(&Player);


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
                                        reset_player(&Player);
                                }
                        }
                }

                unsigned int dt = get_dt();

                // input
                handle_input(SDL_GetKeyboardState(NULL), &Player, dt);

                // START DRAWING
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderClear(renderer);

                // level
                {
                        const int tc = tile_count();
                        for (int i = 0; i < tc; ++i) {
                                tile* Tile = &Tiles[i];
                                // SDL_Rect TileRect = {Tile->Xpos, Tile->Ypos,
                                // Tile->Width,
                                //                      Tile->Height};
                                SDL_Rect TileRect = tile_rect(Tile);
                                SDL_SetRenderDrawColor(renderer, Tile->r, Tile->g,
                                                       Tile->b, 255);
                                if (highlight_tile_id == i) {
                                        // paint it black
                                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                                }
                                SDL_RenderFillRect(renderer, &TileRect);
                                SDL_RenderDrawRect(renderer, &TileRect);
                        }
                }


                // objects
                {
                        // player
                        SDL_SetRenderDrawColor(renderer, Player.r, Player.g, Player.b,
                                               255);
                        SDL_Rect PlayerRect = {Player.Xpos, Player.Ypos, Player.Width,
                                               Player.Height};
                        SDL_RenderFillRect(renderer, &PlayerRect);
                        SDL_RenderDrawRect(renderer, &PlayerRect);
                }

                // visual debug
                if (visual_debug) {
                        // printf("VDB: %d\n", visual_debug);
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                        SDL_Rect PlayerRect = {Player.Xpos, Player.Ypos, Player.Width,
                                               Player.Height};
                        SDL_RenderFillRect(renderer, &PlayerRect);
                        SDL_RenderDrawRect(renderer, &PlayerRect);

                        // draw velocity vector
                        const int scale = 2;

                        const int x1 = Player.Xpos + (Player.Width / 2);
                        const int y1 = Player.Ypos + (Player.Height / 2);
                        const int x2 = x1 + Player.Xspeed * scale;
                        const int y2 = y1 + Player.Yspeed * scale;

                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                }


                SDL_RenderPresent(renderer);
                // END DRAWING

                SDL_Delay(1000 / 60);
        }


        return 0;
}


void step(object* Obj, unsigned int dt)
{
        // printf("dt: %d\n", dt);

        // GRAVITY
        const int gravity = 4;
        Obj->Yspeed += gravity;    // always push downward

        // OTHER FORCES
        if (Obj->State == JUMPING) {
                // printf("JUMPING: ");
                if (Obj->Yspeed < 0) {
                        // going up
                        // puts("going up");
                        int amt = abs(Obj->Yspeed);
                        Obj->Ypos -= amt;
                } else if (Obj->Yspeed == 0) {
                        // at the peak
                        // puts("peak PEAK PEAK PEAK");
                } else if (Obj->Yspeed > 0) {
                        // going down
                        // puts("going down");
                        int amt = abs(Obj->Yspeed);
                        Obj->Ypos += amt;
                }
        }

        // COLLISION HANDLING
        // if touching the ground
        const tile* Tile     = &Tiles[0];
        const int obj_bottom = Obj->Ypos + Obj->Height;
        const int obj_left   = Obj->Xpos;
        const int obj_right  = Obj->Xpos + Obj->Width;

        const int tile_top   = Tile->Ypos;
        const int tile_left  = Tile->Xpos;
        const int tile_right = Tile->Xpos + Tile->Width;

        // obj is within tile bound ie [tile.x, tile.x + width] bounds
        const int in_xrange = (obj_left >= tile_left && obj_left <= tile_right) ||
                              (obj_right >= tile_left && obj_right <= tile_right);
        const int above_tile_top = (obj_bottom <= tile_top);

        // print the nearest tile below player
        {
                if (in_xrange && above_tile_top) {
                        highlight_tile_id = 0;
                } else {
                        highlight_tile_id = -1;
                }
                // printf("on tile: %d\n", highlight_tile_id);
        }

        // const int touching_tile = (Obj->Ypos >= tile_top - Obj->Height) ;
        const int touching_tile = (obj_bottom == tile_top) && in_xrange;
        if (touching_tile) {
                // puts("STANDING");
                // jump ends. now stand
                Obj->State  = STANDING;
                Obj->Ypos   = tile_top - Obj->Height;
                Obj->Yspeed = 0;
        } else {
                printf("%d IN AIR\n", dt);
                // // continue falling
                // Obj->Ypos += Obj->Yspeed;
        }

        if (obj_bottom >= ScreenHeight) {
                puts("RESET");
                reset_player(Obj);
        }
}

static void handle_input(const Uint8* Keys, object* Player, unsigned int dt)
{

        // if (Keys[SDL_SCANCODE_W])
        //         Player->Ypos += Player->Yspeed;
        // else if (Keys[SDL_SCANCODE_S])
        //         Player->Ypos -= Player->Yspeed;

        // movement
        if (Keys[SDL_SCANCODE_A]) {
                if (Player->Xspeed > 0) Player->Xspeed = -Player->Xspeed;    // face left
                Player->Xpos += Player->Xspeed;
        } else if (Keys[SDL_SCANCODE_D]) {
                if (Player->Xspeed < 0) Player->Xspeed = -Player->Xspeed;    // face right
                Player->Xpos += Player->Xspeed;
        }

        if (Keys[SDL_SCANCODE_SPACE] && Player->State == STANDING) {
                Player->State  = JUMPING;
                Player->Yspeed = -Player->JumpSpeed;
        }

        step(Player, dt);
}

int init_sdl()
{
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

        return 0;
}

int deinit_sdl()
{
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
