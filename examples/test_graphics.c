#define MLIN_GRAPHICS_IMPLEMENTATION
#include "../include/mlinG.h"

#include <stdio.h>



int main(){
    Image *img = create_image(800, 600, 4);
    Sphere spheres[] = {
        {{0, -1, 3}, 1, {255, 0, 0, 255}, 0.5f, 0},
        {{2, 0, 4}, 1, {0, 0, 255, 255}, 0.3f, 0},
        {{-2, 0, 4}, 1, {0, 255, 0, 255}, 0.7f, 0}
    };
    int num_spheres = sizeof(spheres) / sizeof(Sphere);

    render_scene(img, spheres, num_spheres);
    save_image_png(img, "output.png");

    freeImage(img);

    printf("Image saved as png\n");

    return 0;
}