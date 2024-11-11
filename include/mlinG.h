#ifndef MLIN_GRAPHICS
#define MLIN_GRAPHICS

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t r, g, b, a;
} Color;

typedef struct 
{
    int width, height, channels;
    uint8_t *data;
} Image;

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

typedef struct {
    Vec3 position;
    float radius;
    Color color;
    float reflectivity; // 0 to 1 for reflectiveness
    float refractivity; // 0 to 1 for transparency
} Sphere;

typedef struct {
    Vec3 position;
    Vec3 normal;
    Color color;
    float reflectivity;
} Plane;

Color trace_ray(Ray ray, Sphere *spheres, int num_spheres, int depth);
int intersect_ray_sphere(Ray ray, Sphere sphere, float *t);
Color calculate_lighting(Vec3 point, Vec3 normal, Vec3 view, Color object_color);
Vec3 reflect_ray(Vec3 ray_dir, Vec3 normal);

Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_scale(Vec3 v, float scale);
float vec3_dot(Vec3 a, Vec3 b);
Vec3 vec3_normalize(Vec3 v);

Image * create_image(int width, int height, int channels);
void freeImage(Image * img);

void draw_pixel(Image *img, int x, int y, Color color);
Color get_pixel(Image *img, int x, int y);
void draw_line(Image *img, int x0, int y0, int x1, int y1, Color color);
void draw_rect(Image *img, int x, int y, int width, int height, Color color, int filled);
void draw_circle(Image *img, int cx, int cy, int radius, Color color, int filled);

void clear_image(Image *img, Color color);

Image *load_image(const char *filepath);
void save_image_png(Image * img, const char* filepath);


#ifdef MLIN_GRAPHICS_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb_image.h"
#include "../external/stb_image_write.h"


Image * create_image(int width, int height, int channels){
    Image * img = (Image*) malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->channels = channels;
    img->data = (uint8_t *)malloc(width * height * channels);
    memset(img->data, 0, width * height *channels);
    return img;
}


void freeImage(Image * img) {
    if (img){
        free(img->data);
        free(img);
    }
}

void draw_pixel(Image *img, int x, int y, Color color) {
    if (x < 0 || x >= img->width || y < 0 || y >= img->height) return;

    int index = (y * img->width + x) * img->channels;
    img->data[index] = color.r;
    img->data[index + 1] = color.g;
    img->data[index + 2] = color.b;
    if (img->channels == 4) img->data[index + 3] = color.a;
}

Color get_pixel(Image *img, int x, int y) {
    Color color = {0, 0, 0, 255};
    if (x < 0 || x >= img->width || y < 0 || y >= img->height) return color;
    int index = (y * img->width + x) * img->channels;
    color.r = img->data[index];
    color.g = img->data[index + 1];
    color.b = img->data[index + 2];
    if (img->channels == 4) color.a = img->data[index + 3];
    return color;
}


void clear_image(Image *img, Color color) {
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            draw_pixel(img, x, y, color);
        }
        
    }
}

Image *load_image(const char *filepath) {
    Image * img = (Image*) malloc(sizeof(Image));
    img->data = stbi_load(filepath, &img->width, &img->height, &img->channels, 0);
    if (!img->data){
        free(img);
        return NULL;
    }

    return img;
}
void save_image_png(Image * img, const char* filepath) {
    stbi_write_png(filepath, img->width, img->height, img->channels, img->data, img->width * img->channels);
}


void draw_line(Image *img, int x0, int y0, int x1, int y1, Color color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (1)
    {
        draw_pixel(img, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy){
            err = err + dy;
            x0 = x0 + sx;
        }
        if (e2 <= dx){
            err = err + dy;
            y0 = y0 + sy;
        }

    }
    
}

void draw_rect(Image *img, int x, int y, int width, int height, Color color, int filled) {
    if (filled) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                draw_pixel(img, x + j, y + i, color);
            }
        }
    } else {
        draw_line(img, x, y, x + width - 1, y, color);             // Top edge
        draw_line(img, x, y, x, y + height - 1, color);            // Left edge
        draw_line(img, x, y + height - 1, x + width - 1, y + height - 1, color); // Bottom edge
        draw_line(img, x + width - 1, y, x + width - 1, y + height - 1, color);  // Right edge
    }
}

void draw_circle(Image *img, int cx, int cy, int radius, Color color, int filled) {
    int x = radius, y = 0;
    int err = 0;

    while (x >= y) {
        if (filled) {
            for (int i = cx - x; i <= cx + x; i++) {
                draw_pixel(img, i, cy + y, color);
                draw_pixel(img, i, cy - y, color);
            }
            for (int i = cx - y; i <= cx + y; i++) {
                draw_pixel(img, i, cy + x, color);
                draw_pixel(img, i, cy - x, color);
            }
        } else {
            draw_pixel(img, cx + x, cy + y, color);
            draw_pixel(img, cx + y, cy + x, color);
            draw_pixel(img, cx - y, cy + x, color);
            draw_pixel(img, cx - x, cy + y, color);
            draw_pixel(img, cx - x, cy - y, color);
            draw_pixel(img, cx - y, cy - x, color);
            draw_pixel(img, cx + y, cy - x, color);
            draw_pixel(img, cx + x, cy - y, color);
        }

        y += 1;
        if (err <= 0) {
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
    Vec3 vec = { a.x + b.x, a.y + b.y, a.z + b.z };
    return vec;
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    Vec3 vec = {  a.x - b.x, a.y - b.y, a.z - b.z };
    return vec;
}

Vec3 vec3_scale(Vec3 v, float scale) {
    Vec3 vec = {v.x * scale, v.y * scale, v.z * scale};
    return vec;
}

float vec3_dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    Vec3 vec = {        
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return vec;
}

float vec3_length(Vec3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 vec3_normalize(Vec3 v) {
    float length = vec3_length(v);
    if (length == 0) {
        Vec3 vec = {0, 0, 0};
        return vec;
    }; 
    return vec3_scale(v, 1.0f / length);
}

Vec3 vec3_reflect(Vec3 v, Vec3 n) {
    return vec3_sub(v, vec3_scale(n, 2 * vec3_dot(v, n)));
}


int intersect_ray_sphere(Ray ray, Sphere sphere, float *t) {
    Vec3 oc = vec3_sub(ray.origin, sphere.position);
    float a = vec3_dot(ray.direction, ray.direction);
    float b = 2.0f * vec3_dot(oc, ray.direction);
    float c = vec3_dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return 0;
    *t = (-b - sqrt(discriminant)) / (2.0f * a);
    return 1;
}


Color calculate_lighting(Vec3 point, Vec3 normal, Vec3 view, Color object_color) {
    // Single light source for simplicity
    Vec3 light_pos = {5, 5, -10};
    Color light_color = {255, 255, 255, 255};

    Vec3 light_dir = vec3_normalize(vec3_sub(light_pos, point));
    float diffuse_intensity = fmax(0, vec3_dot(normal, light_dir));

    // Specular component
    Vec3 reflect_dir = reflect_ray(light_dir, normal);
    float specular_intensity = powf(fmax(0, vec3_dot(view, reflect_dir)), 32); // Shininess

    Color color;
    color.r = fmin(255, object_color.r * diffuse_intensity + light_color.r * specular_intensity);
    color.g = fmin(255, object_color.g * diffuse_intensity + light_color.g * specular_intensity);
    color.b = fmin(255, object_color.b * diffuse_intensity + light_color.b * specular_intensity);
    color.a = object_color.a;
    return color;
}


Vec3 reflect_ray(Vec3 ray_dir, Vec3 normal) {
    return vec3_sub(ray_dir, vec3_scale(normal, 2 * vec3_dot(ray_dir, normal)));
}


Color trace_ray(Ray ray, Sphere *spheres, int num_spheres, int depth) {
    float closest_t = INFINITY;
    Sphere *closest_sphere = NULL;

    // Find the closest sphere intersection
    for (int i = 0; i < num_spheres; i++) {
        float t;
        if (intersect_ray_sphere(ray, spheres[i], &t) && t < closest_t) {
            closest_t = t;
            closest_sphere = &spheres[i];
        }
    }

    if (!closest_sphere) {
        Color col = {135, 206, 235, 255};
        return col; // Sky blue
    }

    // Calculate intersection point and normal
    Vec3 hit_point = vec3_add(ray.origin, vec3_scale(ray.direction, closest_t));
    Vec3 normal = vec3_normalize(vec3_sub(hit_point, closest_sphere->position));
    Color local_color = calculate_lighting(hit_point, normal, vec3_scale(ray.direction, -1), closest_sphere->color);

    // Reflective shading (recursive)
    if (depth < 5 && closest_sphere->reflectivity > 0) {
        Vec3 reflect_dir = reflect_ray(ray.direction, normal);
        Ray reflect_ray = { hit_point, reflect_dir };
        Color reflect_color = trace_ray(reflect_ray, spheres, num_spheres, depth + 1);
        
        // Mix reflection color and local color based on reflectivity
        local_color.r = (uint8_t)(local_color.r * (1 - closest_sphere->reflectivity) + reflect_color.r * closest_sphere->reflectivity);
        local_color.g = (uint8_t)(local_color.g * (1 - closest_sphere->reflectivity) + reflect_color.g * closest_sphere->reflectivity);
        local_color.b = (uint8_t)(local_color.b * (1 - closest_sphere->reflectivity) + reflect_color.b * closest_sphere->reflectivity);
    }

    return local_color;
}


void render_scene(Image *img, Sphere *spheres, int num_spheres) {
    Vec3 camera_pos = {0, 0, -5};

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            float u = (2 * (x + 0.5) / (float)img->width - 1) * tan(M_PI / 6);
            float v = (1 - 2 * (y + 0.5) / (float)img->height) * tan(M_PI / 6);
            Vec3 normvec = {u, v, 1};
            Vec3 ray_dir = vec3_normalize(normvec);
            Ray ray = {camera_pos, ray_dir};

            Color color = trace_ray(ray, spheres, num_spheres, 0);
            draw_pixel(img, x, y, color);
        }
    }
}





#endif
#endif