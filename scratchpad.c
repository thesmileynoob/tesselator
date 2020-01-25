#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/** https://gist.github.com/armornick/3447121 */
#include <SDL2/SDL.h>

#define MUS_PATH "../assets/SFX_Pickup_01.wav"

// variable declarations
static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play

// prototype for our audio callback
// see the implementation for more information
// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
	printf("%p, %hhn, %d\n", userdata, stream, len);
	printf("%p, %d\n", audio_pos, audio_len);

	if (audio_len ==0)
		return;

	len = ( len > audio_len ? audio_len : len );
	//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

	audio_pos += len;
	audio_len -= len;
}

int play(){

	// local variables
	static Uint32 wav_length; // length of our sample
	static Uint8 *wav_buffer; // buffer containing our audio file
	static SDL_AudioSpec wav_spec; // the specs of our piece of music


	/* Load the WAV */
	// the specs, length and buffer of our wav are filled
	if( SDL_LoadWAV(MUS_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL ){
		return 1;
	}
	// set the callback function
	wav_spec.callback = my_audio_callback;
	wav_spec.userdata = NULL;
	// set our global static variables
	audio_pos = wav_buffer; // copy sound buffer
	audio_len = wav_length; // copy file length

	/* Open the audio device */
	if ( SDL_OpenAudio(&wav_spec, NULL) < 0 ){
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}

	/* Start playing */
	SDL_PauseAudio(0);

	// wait until we're don't playing
	// while ( audio_len > 0 ) {
	// 	//SDL_Delay(1);
	// }
	SDL_Delay(1000 / 6);

	// shut everything down
	SDL_CloseAudio();
	SDL_FreeWAV(wav_buffer);

}


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
		SDL_RenderCopy(_renderer, Texture, &TexRect, &TileRect);
	}
}
#endif
