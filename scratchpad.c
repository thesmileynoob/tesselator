#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
