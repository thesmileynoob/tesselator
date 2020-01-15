#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "object.h"

const int ScreenWidth   = 1280;
const int ScreenHeight  = 720;

const int GroundLevel   = (int) ScreenHeight * 3 / 4;
static int visual_debug = 1;

SDL_Window* window     = NULL;
SDL_Renderer* renderer = NULL;

static int init_sdl();
static int deinit_sdl();
static void handle_input(const Uint8* keys, object* player, unsigned int dt);

/**
 * returns: dt in millisecs
 */
static unsigned int get_dt()
{
        static unsigned int oldtime = 0;
        const unsigned int newtime  = SDL_GetTicks();
        const unsigned int dt       = newtime - oldtime;
        oldtime                     = newtime;
        return dt;
}


int main(int argc, char const* argv[])
{
        init_sdl();
        assert(window && renderer);

        // player init
        object Player = {};
        Player.Width  = 25;
        Player.Height = 50;
        Player.Xpos   = 0;
        Player.Ypos   = GroundLevel - Player.Height;
        Player.State  = STANDING;

        Player.Xspeed    = 20;
        Player.Yspeed    = 0;
        Player.JumpSpeed = 1.2 *Player.Height;
        Player.r         = 255;


        int running = 1;
        while (running) {
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                        SDL_Keycode sym = ev.key.keysym.sym;
                        if (sym == SDLK_q || ev.type == SDL_QUIT) { running = 0; }

                        if (ev.type == SDL_KEYUP) {
                                if (sym == SDLK_v) { visual_debug = !visual_debug; }
                        } else if (ev.type == SDL_KEYDOWN) {
                        }
                }

                unsigned int dt = get_dt();

                // input
                handle_input(SDL_GetKeyboardState(NULL), &Player, dt);

                // START DRAWING
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderClear(renderer);

                // level
                SDL_Rect GroundRect = {0, GroundLevel, ScreenWidth, 10};
                SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);
                SDL_RenderFillRect(renderer, &GroundRect);
                SDL_RenderDrawRect(renderer, &GroundRect);


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
        const int gravity = 5;
        Obj->Yspeed += gravity;
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
        if ((Obj->Ypos >= GroundLevel - Obj->Height)) {
                // puts("STANDING");
                // jump ends. now stand
                Obj->State  = STANDING;
                Obj->Ypos   = GroundLevel - Obj->Height;
                Obj->Yspeed = 0;
        }
}

static void handle_input(const Uint8* Keys, object* Player, unsigned int dt)
{

        // if (Keys[SDL_SCANCODE_W])
        //         Player->Ypos -= speedy;
        // else if (Keys[SDL_SCANCODE_S])
        //         Player->Ypos += speedy;

        // movement
        if (Keys[SDL_SCANCODE_A]) {
                if (Player->Xspeed > 0) Player->Xspeed *= -1;    // face left
                Player->Xpos += Player->Xspeed;
        } else if (Keys[SDL_SCANCODE_D]) {
                if (Player->Xspeed < 0) Player->Xspeed *= -1;// face right
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
        err = SDL_CreateWindowAndRenderer(ScreenWidth, ScreenHeight, 0, &window,
                                          &renderer);
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
