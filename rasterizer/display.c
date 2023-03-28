#include "display.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// color buffer holds the pixel colors in a temporary place in memory
// we copy the color buffer onto a texture to be rendered onto the screen

/* we have chosen to manually create a color buffer and copy it onto the texture
*  so that we can understand the color buffer concept directly
*  we could have otherwise used SDL functions to modify the SDL renderer directly,
*  but this would hide what is really happening
*/

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

int window_width = 800;
int window_height = 600;

bool initialize_window(void)
{
	// initialize everything
	// if there was an error, return an error statement
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	// use SDL to query what fullscreen max width and height are
	// SDL_GetCurrentDisplayMode calculates the display characteristics
	// and populates display_mode with these characteristics
	// displayIndex = 0 gives the main display
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);

	// set the dimensions of the window to the dimensions of the display
	// this is actually "fake fullscreen" - it only gives a window with
	// no borders or decorations, so it appears to be fullscreen
	window_width = display_mode.w;
	window_height = display_mode.h;

	// create an SDL window
	window = SDL_CreateWindow(
		"TITLE",					// title
		SDL_WINDOWPOS_CENTERED,		// posX
		SDL_WINDOWPOS_CENTERED,		// posY
		window_width,				// width
		window_height,				// height
		SDL_WINDOW_BORDERLESS		// flags
	);

	// check if window is NULL
	if (window == NULL)
	{
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	// we set the window full screen
	// alternatively, we could use the SDL_WINDOW_FULLSCREEN flag 
	// inside SDL_CreateWindow()
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	// create an SDL renderer
	renderer = SDL_CreateRenderer(
		window,						// window that the renderer belongs to
		-1,							// default display device, -1 means the first good one
		0							// flags
	);

	// check if renderer is NULL
	if (renderer == NULL)
	{
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}

	// window and renderer are successfully created
	return true;
}

void draw_grid(void)
{
	for (int y = 0; y < window_height; y++)
		for (int x = 0; x < window_width; x++)
			if (x % 10 == 0 || y % 10 == 0)
				color_buffer[(window_width * y) + x] = 0xFFFFFFFF;
}

void draw_pixel(int x, int y, uint32_t color)
{
	if (x >= 0 && x < window_width && y >= 0 && y < window_height)
		color_buffer[(window_width * y) + x] = color;
}

void draw_rectangle(int x, int y, int width, int height, uint32_t color)
{
	for (int x_i = x; x_i < x + width; x_i++)
		for (int y_i = y; y_i < y + height; y_i++)
			// color_buffer[(window_width * y_i) + x_i] = color;
			draw_pixel(x_i, y_i, color);
}

void render_color_buffer()
{
	// copy the color buffer onto the texture
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);

	// copy the texture to the renderer
	SDL_RenderCopy(
		renderer,
		color_buffer_texture,
		NULL,
		NULL
	);
}

void clear_color_buffer(uint32_t color)
{
	// loop through the color buffer and clear it with a specified color
	// y loops through the rows, x loops through the columns
	// window_width * y gives the current row in color_buffer
	// + x gives the current column in color_buffer
	for (int y = 0; y < window_height; y++)
		for (int x = 0; x < window_width; x++)
			color_buffer[(window_width * y) + x] = color;
}

void destroy_window(void)
{
	// destroy in reverse order of creation
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}