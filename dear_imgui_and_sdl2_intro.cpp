
// imgui headers
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

// general headers
#include <iostream>
#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

using namespace std;

const static int SCREEN_WIDTH = 800;
const static int SCREEN_HEIGHT = 600;
Uint32* buffer;

void setup_imgui_context(SDL_Window* sdl_window, SDL_Renderer* sdl_renderer)
{
	// Setup Dear ImGui context
	// IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(sdl_window, sdl_renderer);
	ImGui_ImplSDLRenderer2_Init(sdl_renderer);

	// Start the Dear ImGui frame
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();

}

void setPixel(int x, int y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {

	if ((x >= SCREEN_WIDTH) or (y >= SCREEN_HEIGHT)) {
		// if the pixel position is out of box don't do anything
	}
	else if ((x < 0) or (y < 0))
	{
		// if the pixel position is negative then don't do anything
	}
	else {

		Uint32 color = 0;

		color += static_cast<Uint32>(red)   << 24 | 
				 static_cast<Uint32>(green) << 16 | 
				 static_cast<Uint32>(blue)  << 8  | 
				 static_cast<Uint32>(alpha);

		buffer[(y * SCREEN_WIDTH) + x] = color;
	}

}

void rect(int pos_x, int pos_y, int scale_x, int scale_y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			if (((x > pos_x) && (x < pos_x + scale_x)) && ((y > pos_y) && (y < pos_y + scale_y)))
			{
				setPixel(x, y, red, green, blue, alpha);
			}

		}
	}

}

int main(int argc, char* argv[]) {

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;

	// allocating memory
	buffer = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT];
	// Seting initial values in the memory buffer
	memset(buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));

	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}
	cout << "SDL Init succeeded." << endl;

	// Get the number of displays
	int numDisplays = SDL_GetNumVideoDisplays();

	// Check if there are at least two displays
	if (numDisplays < 2) {
		// Handle error (not enough displays)
		SDL_Quit();
		return 1;
	}

	// Create window with SDL_Renderer graphics context
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
	window = SDL_CreateWindow(
		"My Window",
		SDL_WINDOWPOS_CENTERED_DISPLAY(1),
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		window_flags);
	if (window == NULL) {
		SDL_Quit();
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

	texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STATIC,
		SCREEN_WIDTH,
		SCREEN_HEIGHT);

	if (renderer == NULL) {
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return false;
	}

	if (texture == NULL) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyTexture(texture);
		SDL_Quit();
		return false;
	}

	setup_imgui_context(window, renderer);

	// rectangle position properties
	int x_pos = 200;
	int y_pos = 500;

	// rectangle scale properties
	int x_scale = 60;
	int y_scale = 50;

	// rectangle color properties
	ImVec4 rect_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (true)
	{

		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
			{
				cout << "Quit window event called. Closing the window." << endl;
				return false;
			}

		}

		// Start the Dear ImGui frame
		ImGui::NewFrame();

		ImGui::Begin("test");
		{
			ImGui::Text("Color");
			ImGui::ColorEdit4("rect color", (float*)&rect_color);
			ImGui::Text("position controls");
			ImGui::SliderInt("x_pos", &x_pos, 0, SCREEN_WIDTH);
			ImGui::SliderInt("y_pos", &y_pos, 0, SCREEN_HEIGHT);
			ImGui::Text("scale controls");
			ImGui::SliderInt("x_scale", &x_scale, 0, 500);
			ImGui::SliderInt("y_scale", &y_scale, 0, 500);
			ImGui::End();

			ImGui::Begin("rect");
			ImGui::Image((void*)texture, ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT));
			ImGui::End();
		}

		// Update the buffer
		//setPixel(x_pos, y_pos, 255, 255, 255, 255);
		rect(
			x_pos, y_pos,
			x_scale, y_scale,
			(Uint8)(rect_color.x * 255),
			(Uint8)(rect_color.y * 255),
			(Uint8)(rect_color.z * 255),
			(Uint8)(rect_color.w * 255)
		);

		SDL_UpdateTexture(texture, NULL, buffer, SCREEN_WIDTH * sizeof(Uint32));

		// Rendering
		ImGui::Render();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		//SDL_RenderCopy(renderer, texture, NULL, NULL);

		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

		SDL_RenderPresent(renderer);

		// clean the buffer
		memset(buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
		SDL_RenderClear(renderer);
	}

	// Cleanup
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	delete[] buffer;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	SDL_DestroyWindow(window);
	SDL_Quit();

}