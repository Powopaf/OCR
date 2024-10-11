//the median filter can be test in gaussian blur
#include <SDL2/SDL.h>

void insert_sort(Uint8 array[], int len) {
    for (int i = 1; i < len; i++) {
        int curr = array[i];
        int j = i - 1;
        while (j >= 0 && curr < array[j]) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = curr;
    }
}

void median(SDL_Surface* surface) {
    const int kernel_size = 3;
    SDL_LockSurface(surface);
    for (int i = 2; i < surface->h - 2; i++) {
        for (int j = 2; j < surface->w - 2; j++) {
            Uint8* pixel = surface->pixels + i * surface->pitch + j * surface->format->BytesPerPixel;
            Uint8 k_value[9] = { 0 };
            int p1 = -1; //change p1 and p2 in function of the kernel_size
            int p2 = 1;
            size_t index = 0;
            for (int ki = 0; ki < kernel_size; ki++) {
                for (int kj = 0; kj < kernel_size; kj++) {
                    k_value[index] = (pixel + p2 * surface->pitch + p1 * surface->format->BytesPerPixel)[0];
                    insert_sort(k_value, kernel_size);
                    p1++;
                    index++;
                }
                p2--;
            }
            pixel[0] = k_value[kernel_size / 2];
            pixel[1] = k_value[kernel_size / 2];
            pixel[2] = k_value[kernel_size / 2];
        }
    }
    SDL_UnlockSurface(surface);
}

void contrast(SDL_Surface* surf) {
    //image must be gray
    const Uint8 low = 85; //because 255/3=85
    const Uint8 high = 170; //because 255 - 85 = 175
    SDL_LockSurface(surf);
    for (int i = 0; i < surf->h; i++) {
        for (int j = 0; j < surf->w; j++) {
            Uint8* pixel = surf->pixels + i * surf->pitch + j * surf->format->BytesPerPixel;
            if (pixel[0] < low) {
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
            }
            else if (pixel[0] > high) {
                pixel[0] = 255;
                pixel[1] = 255;
                pixel[2] = 255;
            }
            else {
                Uint8 color = 255 * (pixel[0] - low) / (high - low);
                pixel[0] = color;
                pixel[1] = color;
                pixel[2] = color;
            }
        }
    }
    SDL_UnlockSurface(surf);
}

