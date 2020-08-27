#include <stdio.h>
#include <string>
#include <SDL.h>
#include <sync.h>

#include "cleanup.h"

static struct sync_device *device;
#if !defined( SYNC_PLAYER )
static struct sync_cb cb;
#endif

template <typename T>
constexpr auto sizeof_array( const T& array ) {
    return (int) ( sizeof( array ) / sizeof( array[ 0 ] ) );
}

const char *SYNC_HOST_ADDRESS = "localhost";
const uint16_t SYNC_HOST_PORT = SYNC_DEFAULT_PORT;

const float BPM = 140.0f;
const float RPB = 8.0f;
const float RPS = BPM / 60.0f * RPB;
bool isPlaying = true;
int curtimeMs = 0;

static int row_to_ms_round( int row, float rps ) {
    const float newtime = ( (float) ( row ) ) / rps;
    return (int) ( floor( newtime * 1000.0f + 0.5f ) );
}

static float ms_to_row_f( int time_ms, float rps ) {
    const float row = rps * ( (float) time_ms ) * 1.0f / 1000.0f;
    return row;
}

static int ms_to_row_round( int time_ms, float rps ) {
    const float r = ms_to_row_f( time_ms, rps );
    return (int) ( floor( r + 0.5f ) );
}

#if !defined( SYNC_PLAYER )

static void xpause( void* data, int flag ) {
    (void) data;
    isPlaying = flag == 1;
}

static void xset_row( void* data, int row ) {
    int newtime_ms = row_to_ms_round( row, RPS );
    curtimeMs = newtime_ms;
    (void) data;
}

static int xis_playing( void* data )
{
    (void) data;
    return isPlaying ? 1 : 0;
}

#endif //!SYNC_PLAYER

int rocket_init( const char* prefix ) {
    device = sync_create_device( prefix );
    if ( !device ) {
        printf("Unable to create rocketDevice\n");
        return 0;
    }

#if !defined( SYNC_PLAYER )
    cb.is_playing = xis_playing;
    cb.pause = xpause;
    cb.set_row = xset_row;

    if ( sync_tcp_connect( device, SYNC_HOST_ADDRESS, SYNC_HOST_PORT ) ) {
        printf( "Rocket failed to connect\n" );
        return 0;
    }
#endif

    printf("Rocket connected.\n");
    return 1;
}

static int rocket_update() {
    int row = 0;

    if ( isPlaying ) {
        curtimeMs += 16; // 60hz or gtfo
    }

#if !defined( SYNC_PLAYER )
    row = ms_to_row_round( curtimeMs, RPS );
    if ( sync_update( device, row, &cb, 0 ) ) {
        sync_tcp_connect( device, SYNC_HOST_ADDRESS, SYNC_HOST_PORT );
    }
#endif

    return 1;
}

static const char* sTrackNames[] = {
    "internet#w",
    "internet#h",
};

static const struct sync_track* s_tracks[ sizeof_array( sTrackNames ) ];

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
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if ( renderer == NULL ) {
        const char *error = SDL_GetError();
        printf( error );

        cleanup( window );
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

        cleanup( renderer, window );
        SDL_Quit();
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface( renderer, image );
    cleanup( image );

    if ( texture == NULL ) {
        const char* error = SDL_GetError();
        printf( error );

        cleanup( renderer, window );
        SDL_Quit();
        return 1;
    }

    int textureWidth;
    int textureHeight;
    SDL_QueryTexture( texture, NULL, NULL, &textureWidth, &textureHeight );

    // == init rocket ==============================================================================
    if ( !rocket_init( "data/sync" ) ) {
        return -1;
    }

    for ( int i = 0; i < sizeof_array( sTrackNames ); i ++ ) {
        s_tracks[ i ] = sync_get_track( device, sTrackNames[ i ] );
    }

    // == loop =====================================================================================
    SDL_Event event;
    bool shouldQuit = false;

    while ( !shouldQuit ) {
        SDL_Keycode keyPressed = SDLK_UNKNOWN;

        while ( SDL_PollEvent( &event ) ) {
            if ( event.type == SDL_QUIT ) {
                shouldQuit = true;
            }

            if ( event.type == SDL_KEYDOWN ) {
                keyPressed = event.key.keysym.sym;
            }
        }

        if ( keyPressed == SDLK_ESCAPE ) {
            shouldQuit = true;
        }

        rocket_update();
        float row_f = ms_to_row_f( curtimeMs, RPS );

        SDL_Rect dest;
        SDL_GetMouseState( &dest.x, &dest.y );
        dest.w = (int) ( textureWidth * sync_get_val( s_tracks[ 0 ], row_f ) );
        dest.h = (int) ( textureHeight * sync_get_val( s_tracks[ 1 ], row_f ) );

        SDL_RenderClear( renderer );
        SDL_RenderCopy( renderer, texture, NULL, &dest );
        SDL_RenderPresent( renderer );
    }

    // == quit =====================================================================================
    cleanup( texture, renderer, window );
    SDL_Quit();

    return 0;
}
