#include "background.h"

#include "system/gui/gui.h"
#include "system/gui/vars/colors.h"
#include "system/drivers/video/driver.h"


#define BACKGROUND_COLOR 0xFFFFFF00


void draw_background()
{
    if(!fb)
        return;


    uint32_t* bb = get_backbuffer();
    size_t pitch = get_backbuffer_pitch();


    // żółte tło
    for(size_t y = 0; y < fb->height; y++)
    {
        for(size_t x = 0; x < fb->width; x++)
        {
            bb[y * pitch + x] = BACKGROUND_COLOR;
        }
    }



    /*
        Obliczenie skali

        wybieramy większy współczynnik,
        żeby ekran został całkowicie zakryty
    */


    uint32_t scale_x =
        (fb->width << 16) / background_width;

    uint32_t scale_y =
        (fb->height << 16) / background_height;


    uint32_t scale =
        (scale_x > scale_y) ? scale_x : scale_y;



    size_t scaled_width =
        (background_width * scale + 0xFFFF) >> 16;

    size_t scaled_height =
        (background_height * scale + 0xFFFF) >> 16;



    /*
        Lewy dół

        jeśli obraz większy:
        przycinamy górę/prawą stronę

        jeśli mniejszy:
        zostaje na dole
    */


    int offset_x = 0;

    int offset_y =
        fb->height - scaled_height;


    if(offset_y > 0)
        offset_y = 0;



    /*
        Skalowanie nearest-neighbor
    */


    for(size_t y = 0; y < scaled_height; y++)
    {
        for(size_t x = 0; x < scaled_width; x++)
        {

            int screen_x =
                offset_x + x;

            int screen_y =
                offset_y + y;


            if(screen_x < 0 ||
               screen_y < 0 ||
               screen_x >= (int)fb->width ||
               screen_y >= (int)fb->height)
                continue;



            size_t src_x =
                (x << 16) / scale;

            size_t src_y =
                (y << 16) / scale;



            bb[
                screen_y * pitch +
                screen_x
            ] =
                background_data[
                    src_y * background_width +
                    src_x
                ];
        }
    }
}