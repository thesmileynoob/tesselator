# TODO
    - fix level dimensions (easy)
    - background graphics (easy)
    - multiple-hit tiles (easy)
    - load and save gamestate (easy)
    - rename variables to match set conventions (easy)

    - dynamic combo, score, time-left text (med)
    - add struct particle_src? (med)
    - slow motion! (med?)
    - more levels (med)
    - add some powerups (med)

    - start screen and pause menu (hard)
    - sound (not for the faint of heart) (veteran)
    - realistic breaking animation (veteran)



# Ideas
    - slow motion break animation
    - some camera movement/screen shake on breaking a tile
    - android support ( after steam release ;) )


# High-level design
## Level
    ************
    * - - - - -*
    * - - - - -*    <- Tiles
    * - - - - -*
    * - -   - -*
    * -     - -*
    *          *
    *     o    *    <- Ball
    *   ----   *    <- Player
    ************

## Steps:
    1. initialize external libs (SDL_*)
    2. allocate memory for the gamestate
    3. main loop:
        - take user input and move player
        - move ball and tiles
        - check for collisions
        - update tiles and score
        - apply effects
        - draw BG, TILES, BALL, PLAYER, UI(SCORE)

## Style Conventions:
    - global vars are capitalized: Tiles, Player etc.
    - locals and params are snake_case
    - functions are snake_case

# BUGS:
    What are those?


# Setup
1. Install the following deps (Ubuntu 19.04)
```
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
sudo apt install cmake
```

2. Clone this repo
```
git clone git@github.com:thesmileynoob/tesselator.git
```

3. Setup build dir
```
cd tesselator
mkdir build
cd build
cmake ..
```

4. Compiling
```
cd build
make
```

5. Running
```
cd build
./debug # This is the executable
```


# Notes:
Final project structure should look something like this

```
tesselator
├── animations.c
├── assets
│   ├── bomber_font.png
│   ├── font_04b.ttf
│   ├── SFX_Pickup_01.wav
│   └── tiles.png
├── breakout.h
├── build
│   ├── debug    #### This is the executable
│   ├── Makefile
│   └── ...
├── CMakeLists.txt
├── main.c
├── README.md
├── scratchpad.c
└── ui.c
└── ...

```
