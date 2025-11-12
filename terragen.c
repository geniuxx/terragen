//==============================================================================
//
//   terragen.c    
//   A procedural terrain generator using the Diamond-Square algorithm with 
//   isometric 3D visualization, built with Raylib.
//
//   Version 1.0 
//
//   Author: Claudio Genio
//   Algorithm: Diamond-Square with isometric projection
//
//==============================================================================

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

/* Constants */
#define ROUGHNESS 1.20f
#define ITERATIONS 257              // 2^n+1 (65, 129, 257)
#define INITIAL_HEIGHT 50.0f
#define SCREEN_MARGIN 50            // Border margin (pixels)

/* Isometric projection angles (in degrees) */
#define ISO_ANGLE 30.0f
#define ROTATION_ANGLE 45.0f

/* Screen dimensions */
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 700
#define UI_HEIGHT 90                // Space for UI at the top

/* Function declarations */
void generate_terrain(void);
float calculate_noise(float amplitude);
void calculate_view_parameters(void);
void draw_terrain_3d(void);
void draw_reference_axes(void);
Vector2 isometric_projection(float x, float y, float z);
Color calculate_height_color(float height, float max_height, float min_height);
void calculate_min_max_height(void);
void reset_canvas_corners(void);

/* Global variables */
float terrain[ITERATIONS][ITERATIONS];
float min_height, max_height;

/* Dynamically calculated view parameters */
float render_scale;
float offset_x, offset_y;

/* Terrain colors */
const Color COLOR_WATER = {30, 90, 180, 255};
const Color COLOR_SAND = {210, 180, 140, 255};
const Color COLOR_GRASS = {50, 150, 50, 255};
const Color COLOR_ROCK = {120, 100, 80, 255};
const Color COLOR_SNOW = {240, 240, 255, 255}; 

/* ----------------------------------------------------------------------------
 * Main function
 * ---------------------------------------------------------------------------- */
int main(void)
{
    srand((unsigned int)time(NULL));
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3D World - Virtual Mountains");
    SetTargetFPS(60);
    
    /* Generate initial terrain and calculate view parameters */
    reset_canvas_corners();
    generate_terrain();
    calculate_min_max_height();
    calculate_view_parameters();
    
    while (!WindowShouldClose())
    {
        /* SPACE redraws the terrain */
        if (IsKeyPressed(KEY_SPACE))
        {
            reset_canvas_corners();
            generate_terrain();
            calculate_min_max_height();
            calculate_view_parameters();
        }
        
        /* Terrain rendering */
        BeginDrawing();
        ClearBackground(BLACK);
        
        draw_terrain_3d();
        draw_reference_axes();
        
        /* Display all information related to code generation */
        DrawText("SPACE: Regenerate terrain | ESC: Exit", 10, 10, 20, RAYWHITE);
        DrawText(TextFormat("Height min: %.1f  max: %.1f", min_height, max_height), 10, 40, 16, LIGHTGRAY);
        DrawText(TextFormat("Resolution: %dx%d - Scale: %.2f", ITERATIONS, ITERATIONS, render_scale), 10, 60, 16, LIGHTGRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

/* ----------------------------------------------------------------------------
 * Correct isometric projection (without scale)
 * This function transforms 3D coordinates (x, y, z) into 2D screen coordinates
 * using isometric projection, a common technique in graphics to represent
 * three-dimensional objects on two-dimensional surfaces while maintaining
 * axis proportions.
 * ---------------------------------------------------------------------------- */
Vector2 isometric_projection(float x, float y, float z)
{
    /* Convert angles to radians */
    float ang_rot = ROTATION_ANGLE * DEG2RAD;
    float ang_iso = ISO_ANGLE * DEG2RAD;
    
    /* Standard isometric projection */
    float x_rot = x * cosf(ang_rot) - y * sinf(ang_rot);
    float y_rot = x * sinf(ang_rot) + y * cosf(ang_rot);
    
    Vector2 projection;
    projection.x = x_rot;
    projection.y = y_rot * sinf(ang_iso) - z;
    
    return projection;
}

/* ----------------------------------------------------------------------------
 * Calculate parameters to automatically center and scale the terrain
 * ---------------------------------------------------------------------------- */
void calculate_view_parameters(void)
{
    float min_x = 1e9, max_x = -1e9;
    float min_y = 1e9, max_y = -1e9;
    
    /* First pass: find the limits of the projected terrain */
    for (int y = 0; y < ITERATIONS; y++)
    {
        for (int x = 0; x < ITERATIONS; x++)
        {
            Vector2 p = isometric_projection(x, y, terrain[x][y]);
            
            if (p.x < min_x) min_x = p.x;
            if (p.x > max_x) max_x = p.x;
            if (p.y < min_y) min_y = p.y;
            if (p.y > max_y) max_y = p.y;
        }
    }
    
    /* Calculate projected terrain dimensions */
    float terrain_width = max_x - min_x;
    float terrain_height = max_y - min_y;
    
    /* Calculate available space (with margins) */
    float available_space_x = SCREEN_WIDTH - (2 * SCREEN_MARGIN);
    float available_space_y = (SCREEN_HEIGHT - UI_HEIGHT) - (2 * SCREEN_MARGIN);
    
    /* Calculate scale factor to fit everything on screen */
    float scale_x = available_space_x / terrain_width;
    float scale_y = available_space_y / terrain_height;
    
    /* Use the smaller scale to maintain proportions */
    render_scale = fminf(scale_x, scale_y);
    
    /* Calculate offsets for centering */
    float scaled_width = terrain_width * render_scale;
    float scaled_height = terrain_height * render_scale;
    
    offset_x = (SCREEN_WIDTH - scaled_width) / 2.0f - (min_x * render_scale);
    offset_y = (SCREEN_HEIGHT - UI_HEIGHT - scaled_height) / 2.0f + UI_HEIGHT - (min_y * render_scale);
}

/* ----------------------------------------------------------------------------
 * Optimized 3D drawing with automatic centering
 * ---------------------------------------------------------------------------- */
void draw_terrain_3d(void)
{    
    /* Draw terrain grid */
    for (int y = 0; y < ITERATIONS - 1; y++)
    {
        for (int x = 0; x < ITERATIONS - 1; x++)
        {
            /* Coordinates of the 4 vertices of the cell (base projection) */
            Vector2 p1_base = isometric_projection(x, y, terrain[x][y]);
            Vector2 p2_base = isometric_projection(x + 1, y, terrain[x + 1][y]);
            Vector2 p3_base = isometric_projection(x, y + 1, terrain[x][y + 1]);
            Vector2 p4_base = isometric_projection(x + 1, y + 1, terrain[x + 1][y + 1]);
            
            /* Apply scale and offset for centering */
            Vector2 p1 = {p1_base.x * render_scale + offset_x,
                         p1_base.y * render_scale + offset_y};
            Vector2 p2 = {p2_base.x * render_scale + offset_x,
                         p2_base.y * render_scale + offset_y};
            Vector2 p3 = {p3_base.x * render_scale + offset_x,
                         p3_base.y * render_scale + offset_y};
            Vector2 p4 = {p4_base.x * render_scale + offset_x,
                         p4_base.y * render_scale + offset_y};
            
            /* Calculate average height for color */
            float avg_height = (terrain[x][y] + terrain[x + 1][y] + 
                               terrain[x][y + 1] + terrain[x + 1][y + 1]) / 4.0f;
            
            /* Set color based on height */
            Color color = calculate_height_color(avg_height, max_height, min_height);
            
            /* Draw grid lines (note: Y inverted for Raylib) */
            DrawLineV((Vector2){p1.x, SCREEN_HEIGHT - p1.y},
                     (Vector2){p2.x, SCREEN_HEIGHT - p2.y},
                     color);
            
            DrawLineV((Vector2){p1.x, SCREEN_HEIGHT - p1.y},
                     (Vector2){p3.x, SCREEN_HEIGHT - p3.y},
                     color);
            
            /* Close cells on borders */
            if (x == ITERATIONS - 2)
            {
                DrawLineV((Vector2){p2.x, SCREEN_HEIGHT - p2.y},
                         (Vector2){p4.x, SCREEN_HEIGHT - p4.y}, color);
            }
            if (y == ITERATIONS - 2)
            {
                DrawLineV((Vector2){p3.x, SCREEN_HEIGHT - p3.y},
                         (Vector2){p4.x, SCREEN_HEIGHT - p4.y}, color);
            }
        }
    }
}

/* ----------------------------------------------------------------------------
 * Draw X, Y, Z axes
 * ---------------------------------------------------------------------------- */
void draw_reference_axes(void)
{
    /* Draw reference axes */
    Vector2 origin_base = isometric_projection(0, 0, 0);
    Vector2 axis_x_base = isometric_projection(ITERATIONS * 0.25f, 0, 0);
    Vector2 axis_y_base = isometric_projection(0, ITERATIONS * 0.25f, 0);
    Vector2 axis_z_base = isometric_projection(0, 0, max_height * 0.5f);
    
    /* Apply scale and offset */
    Vector2 origin = {origin_base.x * render_scale + offset_x,
                     origin_base.y * render_scale + offset_y};
    Vector2 axis_x = {axis_x_base.x * render_scale + offset_x,
                     axis_x_base.y * render_scale + offset_y};
    Vector2 axis_y = {axis_y_base.x * render_scale + offset_x,
                     axis_y_base.y * render_scale + offset_y};
    Vector2 axis_z = {axis_z_base.x * render_scale + offset_x,
                     axis_z_base.y * render_scale + offset_y};
    
    DrawLineV((Vector2){origin.x, SCREEN_HEIGHT - origin.y},
             (Vector2){axis_x.x, SCREEN_HEIGHT - axis_x.y}, RED);
    DrawLineV((Vector2){origin.x, SCREEN_HEIGHT - origin.y},
             (Vector2){axis_y.x, SCREEN_HEIGHT - axis_y.y}, GREEN);
    DrawLineV((Vector2){origin.x, SCREEN_HEIGHT - origin.y},
             (Vector2){axis_z.x, SCREEN_HEIGHT - axis_z.y}, BLUE);
    
    DrawText("X", axis_x.x + 10, SCREEN_HEIGHT - axis_x.y, 14, RED);
    DrawText("Y", axis_y.x + 10, SCREEN_HEIGHT - axis_y.y, 14, GREEN);
    DrawText("Z", axis_z.x + 10, SCREEN_HEIGHT - axis_z.y, 14, BLUE);
}

/* ----------------------------------------------------------------------------
 * Calculate normalized height
 * ---------------------------------------------------------------------------- */
float calculate_normalized_height(float height, float max_alt, float min_alt)
{
    float range = max_alt - min_alt;
    float normalized = (height - min_alt) / range;
    
    return normalized;
}

/* ----------------------------------------------------------------------------
 * Calculate color based on height
 * ---------------------------------------------------------------------------- */
Color calculate_height_color(float height, float max_alt, float min_alt)
{
    float normalized = calculate_normalized_height(height, max_alt, min_alt);
    
    /* Height levels (inverted: snow at top, water at bottom) */
    if (normalized < 0.2f)  return COLOR_SNOW;
    if (normalized < 0.35f) return COLOR_ROCK;
    if (normalized < 0.65f) return COLOR_GRASS;
    if (normalized < 0.85f) return COLOR_SAND;
    
    return COLOR_WATER;
}

/* ----------------------------------------------------------------------------
 * Generate terrain using Diamond-Square algorithm
 * ---------------------------------------------------------------------------- */
void generate_terrain(void)
{
    int length = ITERATIONS - 1;
    float amplitude = INITIAL_HEIGHT;
    
    /* Reset min/max */
    min_height = 0.0f;
    max_height = 0.0f;
    
    while (length > 1)
    {
        int half = length / 2;
        
        /* SQUARE STEP */
        for (int x = 0; x < ITERATIONS - 1; x += length)
        {
            for (int y = 0; y < ITERATIONS - 1; y += length)
            {
                float average = (terrain[x][y] +
                               terrain[x + length][y] +
                               terrain[x][y + length] +
                               terrain[x + length][y + length]) / 4.0f;
                
                terrain[x + half][y + half] = average + calculate_noise(amplitude);
            }
        }
        
        /* DIAMOND STEP */
        for (int x = 0; x < ITERATIONS; x += half)
        {
            for (int y = (x + half) % length; y < ITERATIONS; y += length)
            {
                float sum = 0.0f;
                int count = 0;
                
                /* Check the 4 neighbors */
                if (x >= half)
                {
                    sum += terrain[x - half][y];
                    count++;
                }
                if (x + half < ITERATIONS)
                {
                    sum += terrain[x + half][y];
                    count++;
                }
                if (y >= half)
                {
                    sum += terrain[x][y - half];
                    count++;
                }
                if (y + half < ITERATIONS)
                {
                    sum += terrain[x][y + half];
                    count++;
                }
                
                terrain[x][y] = (sum / count) + calculate_noise(amplitude);
            }
        }
        
        length /= 2;
        amplitude *= powf(2.0f, -ROUGHNESS);
    }
}

/* ----------------------------------------------------------------------------
 * Calculate minimum and maximum heights
 * ---------------------------------------------------------------------------- */
void calculate_min_max_height(void)
{
    /* Calculate min and max heights */
    min_height = terrain[0][0];
    max_height = terrain[0][0];
    
    for (int x = 0; x < ITERATIONS; x++)
    {
        for (int y = 0; y < ITERATIONS; y++)
        {
            if (terrain[x][y] < min_height) min_height = terrain[x][y];
            if (terrain[x][y] > max_height) max_height = terrain[x][y];
        }
    }
}

/* ----------------------------------------------------------------------------
 * Calculate random noise
 * ---------------------------------------------------------------------------- */
float calculate_noise(float amplitude)
{
    return ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * amplitude;
}

/* ----------------------------------------------------------------------------
 * Reset the 4 corners of the terrain to 0
 * ---------------------------------------------------------------------------- */
void reset_canvas_corners(void)
{
    terrain[0][0] = 0.0f;
    terrain[0][ITERATIONS - 1] = 0.0f;
    terrain[ITERATIONS - 1][0] = 0.0f;
    terrain[ITERATIONS - 1][ITERATIONS - 1] = 0.0f;
}
