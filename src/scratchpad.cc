#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0

char* read_file(const char* path)
{
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        printf("Failed to open file: %s\n", path);
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    size_t nmemb = ftell(f);
    rewind(f);

    char* response = malloc((nmemb + 1) * sizeof(*response));
    fread(response, sizeof(char), nmemb, f);
    fclose(f);
    response[nmemb] = '\0';
    // puts(response);
    return response;
}
// TILES
SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
SDL_Rect TexRect = texture_rect(1, 4);  // TODO

const int pad = 12;
for (int row = 0; row < Rows; ++row) {
    for (int col = 0; col < Cols; ++col) {
        const int x       = pad + col * TILE_WIDTH;
        const int y       = pad + row * TILE_HEIGHT;
        SDL_Rect TileRect = {x, y, TILE_WIDTH - 2 * pad, TILE_HEIGHT - 2 * pad};
        SDL_RenderCopy(_renderer, TileTexture, &TexRect, &TileRect);
    }
}

#endif
