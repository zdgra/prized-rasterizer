#include "display.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "vector.h"

// declare an array of vectors/points
#define N_POINTS 9 * 9 * 9
vec3_t cube_points[N_POINTS]; // 9x9x9 cube, saved in a 1D array
vec2_t projected_points[N_POINTS];

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };

float fov_factor = 640;

bool is_running = false;

void setup(void)
{
	// allocate the required memory in bytes to hold the color buffer
	color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);

	// null checks - super important!
	if (color_buffer == NULL)
		printf("The allocation of the color buffer was unsuccessful.");

	// create an SDL texture that is used to display what is on the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,						// renderer
		SDL_PIXELFORMAT_ARGB8888,		// pixel format
		SDL_TEXTUREACCESS_STREAMING,	// texture access
		window_width,					// texture width
		window_height					// texture height
	);

	int point_count = 0;

	// start loading my array of vectors
	// from -1 to 1 (in this 9x9x9 cube)
	// we set each (x, y, z) to a new point,
	// and add that new point to the 1D cube_points array
	// after adding it, we increment point_count
	for (float x = -1; x <= 1; x += 0.25)
		for (float y = -1; y <= 1; y += 0.25)
			for (float z = -1; z <= 1; z += 0.25)
			{
				vec3_t new_point = { x, y, z };
				cube_points[point_count++] = new_point;
			}
}

void process_input(void)
{
	// &event is the memory address of event
	SDL_Event event;
	SDL_PollEvent(&event);

	// check the type of event
	switch (event.type)
	{
		case SDL_QUIT:									// X button for a window
			is_running = false;
			break;
		case SDL_KEYDOWN:								// this is a keyboard press
			if (event.key.keysym.sym == SDLK_ESCAPE)	// escape key
				is_running = false;
			break;
	}
}

vec2_t project(vec3_t point)
{
	// currently, this is orthographic projection
	// we take a 3D point, point, and orthographically project it
	// by simply returning (point.x, point.y), ignoring point.z
	// we scale each point by a fov_factor, 
	// since they're all bunched up between -1 and 1 with step size 0.5
	vec2_t projected_point = {
		.x = fov_factor * (point.x / point.z),
		.y = fov_factor * (point.y / point.z)
	};

	return projected_point;
}

void update(void)
{
	// we loop through each 3D point in cube_points
	// and (orthographically) project each point 
	// we then save these points into a projected_points array
	for (int i = 0; i < N_POINTS; i++)
	{
		vec3_t point = cube_points[i];

		// move the points away from the camera
		point.z -= camera_position.z;

		// project the current point
		vec2_t projected_point = project(point);

		// save the projected 2D vector in the array of projected points
		projected_points[i] = projected_point;
	}
}

void render(void)
{
	/* 
	** select the color to be used for drawing, and
	** clear the renderer with this color
	** might not be necessary since we are clearing the color buffer manually
	** we realize that we do not need these functions anymore because 
	** we are using the color buffer
	*/
	// SDL_SetRenderDrawColor(renderer, 120, 50, 75, 255);
	// SDL_RenderClear(renderer);

	// test functions
	// draw_grid();
	// draw_rectangle(500, 400, 500, 300, 0xFFFFC0CB);
	// draw_pixel(20, 20, 0xFFFFFF00);

	// loop through all projected points and render them
	// we render these projected points as rectangles
	for (int i = 0; i < N_POINTS; i++)
	{
		vec2_t projected_point = projected_points[i];

		// printf("(%f, %f)\n\n", projected_point.x, projected_point.y);

		draw_rectangle(
			projected_point.x + window_width / 2,	// x position
			projected_point.y + window_height / 2,	// y position
			4,										// width
			4,										// height
			0xFFFFFF00								// color
		);
	}

	// this is where we copy the color buffer onto the texture
	// and copy the texture onto the renderer
	render_color_buffer();

	// actually displays the contents of the renderer
	SDL_RenderPresent(renderer);

	// we want to clear the color buffer every frame so that 
	// we start with a new color buffer every frame
	// we then re-initialize the color buffer with the given color
	clear_color_buffer(0xFF000000); // yellow
}

int main(int argc, char* args[])
{
	// create an SDL window
	is_running = initialize_window();

	setup();

	while (is_running)
	{
		process_input();
		update();
		render();
	}

	

	destroy_window();

	return 0;
}