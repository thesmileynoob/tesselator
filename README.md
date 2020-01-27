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
