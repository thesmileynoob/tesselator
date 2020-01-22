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
// TODO: Move init to a LoadLevel() function
static gamestate gs;

/** the FIRST function to be called */
static int _init_sdl(int Width, int Height, SDL_Window** outWin,
                     SDL_Renderer** outRenderer);
static int _deinit_sdl();
static void update_state(const Uint8* Keys, unsigned int dt);
static int save_game_state();
static int load_game_state();
static SDL_Rect texture_rect(unsigned int col, unsigned int row);


/** here we go! */
int main(int argc, char const* argv[])
{
        _init_sdl(SCREENWIDTH, SCREENHEIGHT, &_window, &_renderer);
        assert(_window && _renderer);

        // TODO remove magic numbers
        // init gamestate
        gs.Running = 1;
        gs.Level   = load_level(0);

        // player
        gs.Player.X      = SCREENWIDTH / 2;
        gs.Player.Y      = gs.Level.PlayerLine;
        gs.Player.Width  = 155;
        gs.Player.Height = 35;

        // ball
        gs.Ball.X      = SCREENWIDTH / 2;
        gs.Ball.Y      = 50;
        gs.Ball.Width  = 25;
        gs.Ball.Height = 25;


        unsigned int dt = get_dt();
        while (gs.Running) {
                dt = get_dt();

                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                        SDL_Keycode sym = ev.key.keysym.sym;
                        if (sym == SDLK_q || ev.type == SDL_QUIT) { gs.Running = 0; }

                        if (ev.type == SDL_KEYUP) {
                                if (sym == SDLK_k) { save_game_state(); }
                                if (sym == SDLK_l) { load_game_state(); }
                        } else if (ev.type == SDL_KEYDOWN) {
                                if (sym == SDLK_r) { puts("MANUAL RESET"); }
                        }
                }

                // handle input and update state
                update_state(SDL_GetKeyboardState(NULL), dt);

                // aliases
                const level* Level = &gs.Level;     // alias
                player* Player     = &gs.Player;    // alias
                ball* Ball         = &gs.Ball;      // alias

                // START DRAWING
                SDL_SetRenderDrawColor(_renderer, 50, 50, 50, 255);
                SDL_RenderClear(_renderer);

                // grid
                {

                        SDL_SetRenderDrawColor(_renderer, 250, 250, 250, 255);
                        // vertical lines
                        for (int i = 0; i < Level->Cols; ++i) {
                                // printf("");
                                int x1 = i * Level->TileWidth;
                                int y1 = 0;
                                int x2 = x1;
                                int y2 = y1 + SCREENHEIGHT;
                                SDL_RenderDrawLine(_renderer, x1, y1, x2, y2);
                        }
                        // horizontal lines
                        for (int i = 0; i < Level->Rows; ++i) {
                                int x1 = 0;
                                int y1 = i * Level->TileHeight;
                                int x2 = x1 + SCREENWIDTH;
                                int y2 = y1;
                                SDL_RenderDrawLine(_renderer, x1, y1, x2, y2);
                                int y = i * Level->TileHeight;
                        }
                }

                // level
                {
                        // tiles
                        SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
                        SDL_Rect TexRect = texture_rect(0, 0);    // TODO

                        for (int row = 0; row < Level->Rows; ++row) {
                                for (int col = 0; col < Level->Cols; ++col) {
                                        const int x       = col * Level->TileWidth;
                                        const int y       = row * Level->TileHeight;
                                        SDL_Rect TileRect = {x, y, Level->TileWidth,
                                                             Level->TileHeight};
                                        SDL_RenderCopy(_renderer, Level->Texture,
                                                       &TexRect, &TileRect);
                                }
                        }
                }


                // objects
                {
                        // player
                        SDL_Rect PlayerRect = RECT(Player);
                        SDL_Rect TexRect    = texture_rect(0, 1);    // TODO
                        SDL_RenderCopy(_renderer, Level->Texture, &TexRect, &PlayerRect);

                        // ball
                        SDL_Rect BallRect = RECT(Ball);
                        SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
                        SDL_RenderFillRect(_renderer, &BallRect);
                }


                SDL_RenderPresent(_renderer);
                // END DRAWING

                SDL_Delay(1000 / 60);    // fps
        }


        return 0;
}


void update_state(const Uint8* Keys, unsigned int dt)
{
        // update player
        player* Player        = &gs.Player;
        const int playerspeed = 15;

        // player movement
        if (Keys[SDL_SCANCODE_A]) {
                Player->X -= playerspeed;    // move left
        }
        if (Keys[SDL_SCANCODE_D]) {
                Player->X += playerspeed;    // move right
        }

        // player actions
        if (Keys[SDL_SCANCODE_SPACE]) { puts("launch ball!"); }

        // player-level collision detection
        if (LEFT(Player) < 0) { Player->X = 0; }
        if (RIGHT(Player) > SCREENWIDTH) { Player->X = SCREENWIDTH - Player->Width; }


        // ball update
        ball* Ball        = &gs.Ball;
        static int xspeed = 2;
        static int yspeed = 7;
        Ball->X += xspeed;
        Ball->Y += yspeed;


        // ball-level collision
        if (LEFT(Ball) < 0) {
                Ball->X = 0;
                xspeed  = -xspeed;
        } else if (RIGHT(Ball) > SCREENWIDTH) {
                Ball->X = SCREENWIDTH - Ball->Width;
                xspeed  = -xspeed;
        }
        if (TOP(Ball) < 0) {
                Ball->Y = 0;
                yspeed  = -yspeed;
        } else if (BOTTOM(Ball) > SCREENHEIGHT) {
                Ball->Y = SCREENHEIGHT - Ball->Height;
                yspeed  = -yspeed;
        }

        // Ball-Player check
        {
                SDL_Rect ballRect   = RECT(Ball);
                SDL_Rect playerRect = RECT(Player);
                if (SDL_HasIntersection(&ballRect, &playerRect) == SDL_TRUE) {

                        const int ball_center   = ballRect.x + (ballRect.w / 2);
                        const int player_center = playerRect.x + (playerRect.w / 2);
                        const int offset =
                            ball_center - player_center;    // -ve means left
                        float scale = 1.0;
                        if (abs(offset) < (Player->Width / 3)) {
                                scale = 1.0;
                        } else {
                                scale = 0.7;
                        }
                        int newXspeed       = scale * offset;
                        const int maxxspeed = 7;
                        if (newXspeed > maxxspeed) newXspeed = maxxspeed;      // clip max
                        if (newXspeed < -maxxspeed) newXspeed = -maxxspeed;    // clip max

                        xspeed = newXspeed;    // based on offset
                        yspeed = -yspeed;      // just changes direction in Y

                        printf("%d -> %d\n", offset, xspeed);
                }
        }
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

// return SDL_Rect of sprite at (col, row)
SDL_Rect texture_rect(unsigned int col, unsigned int row)
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
