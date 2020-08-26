#include <stdio.h>
#include <string>
#include <SDL.h>

int main( int argc, char **argv ) {
    // == init video context =======================================================================
    if ( SDL_Init( SDL_INIT_VIDEO ) != NULL ) {
        const char *error = SDL_GetError();
        printf( error );

        SDL_Quit();
        return 1;
    }

    // == init window ==============================================================================
    SDL_Window *window = SDL_CreateWindow(
        "haha", // title
        SDL_WINDOWPOS_UNDEFINED, // x
        SDL_WINDOWPOS_UNDEFINED, // y
        640, // w
        480, // h
        0 // flags
    );

    if ( window == NULL ) {
        const char *error = SDL_GetError();
        printf( error );

        SDL_Quit();
        return 1;
    }

    // == init renderer ============================================================================
    SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

    if ( renderer == NULL ) {
        const char *error = SDL_GetError();
        printf( error );

        SDL_DestroyWindow( window );
        SDL_Quit();
        return 1;
    }

    // == init texture =============================================================================
    std::string basePath = SDL_GetBasePath(); // guaranteed to end with a path separator
    std::string imagePath = basePath + "image.bmp";

    SDL_Surface* image = SDL_LoadBMP( imagePath.c_str() );

    if ( image == NULL ) {
        const char* error = SDL_GetError();
        printf( error );

        SDL_DestroyRenderer( renderer );
        SDL_DestroyWindow( window );
        SDL_Quit();
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface( renderer, image );
    SDL_FreeSurface( image );

    if ( texture == NULL ) {
        const char* error = SDL_GetError();
        printf( error );

        SDL_DestroyRenderer( renderer );
        SDL_DestroyWindow( window );
        SDL_Quit();
        return 1;
    }

    int textureWidth;
    int textureHeight;
    SDL_QueryTexture( texture, NULL, NULL, &textureWidth, &textureHeight );

    // == loop =====================================================================================
    SDL_Event event;
    bool shouldQuit = false;

    while ( !shouldQuit ) {
        while ( SDL_PollEvent( &event ) ) {
            if ( event.type == SDL_QUIT ) {
                shouldQuit = true;
            }

            if ( event.type == SDL_KEYDOWN ) {
                shouldQuit = true;
            }
        }

        SDL_Rect dest;
        SDL_GetMouseState( &dest.x, &dest.y );
        dest.w = textureWidth;
        dest.h = textureHeight;

        SDL_RenderClear( renderer );
        SDL_RenderCopy( renderer, texture, NULL, &dest );
        SDL_RenderPresent( renderer );
    }

    // == quit =====================================================================================
    SDL_DestroyTexture( texture );
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
