#include "tobii/tobii_streams.h"
#include <stdio.h>
#include <assert.h>
#include <cstring>
#include <iostream>
using namespace std;

void gaze_origin_callback( tobii_gaze_origin_t const* gaze_origin, void* user_data )
{
    cout << gaze_origin->left_validity << endl;
    cout << gaze_origin->right_validity << endl;
    if( gaze_origin->left_validity == TOBII_VALIDITY_VALID )
        printf( "Left: %f, %f, %f ",
            gaze_origin->left_xyz[ 0 ],
            gaze_origin->left_xyz[ 1 ],
            gaze_origin->left_xyz[ 2 ] );

    if( gaze_origin->right_validity == TOBII_VALIDITY_VALID )
        printf( "Right: %f, %f, %f ",
            gaze_origin->right_xyz[ 0 ],
            gaze_origin->right_xyz[ 1 ],
            gaze_origin->right_xyz[ 2 ] );

    printf( "\n" );
}

static void url_receiver( char const* url, void* user_data )
{
    char* buffer = (char*)user_data;
    if( *buffer != '\0' ) return; // only keep first value

    if( strlen( url ) < 256 )
        strcpy( buffer, url );
}

int main()
{
    tobii_api_t* api;
    tobii_error_t error = tobii_api_create( &api, NULL, NULL );
    assert( error == TOBII_ERROR_NO_ERROR );

    char url[ 256 ] = { 0 };
    error = tobii_enumerate_local_device_urls( api, url_receiver, url );
    assert( error == TOBII_ERROR_NO_ERROR && *url != '\0' );

    tobii_device_t* device;
    error = tobii_device_create( api, url, &device );
    assert( error == TOBII_ERROR_NO_ERROR );

    error = tobii_gaze_origin_subscribe( device, gaze_origin_callback, 0 );
    assert( error == TOBII_ERROR_NO_ERROR );

    int is_running = 1000; // in this sample, exit after some iterations
    while( --is_running > 0 )
    {
        error = tobii_wait_for_callbacks( nullptr, 1, &device  );
        assert( error == TOBII_ERROR_NO_ERROR || error == TOBII_ERROR_TIMED_OUT );

        error = tobii_device_process_callbacks( device );
        assert( error == TOBII_ERROR_NO_ERROR );
    }

    error = tobii_gaze_origin_unsubscribe( device );
    assert( error == TOBII_ERROR_NO_ERROR );

    error = tobii_device_destroy( device );
    assert( error == TOBII_ERROR_NO_ERROR );

    error = tobii_api_destroy( api );
    assert( error == TOBII_ERROR_NO_ERROR );
    return 0;
}