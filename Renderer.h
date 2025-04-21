#pragma once
#include <string>
#include "Helper.h"
#include "AudioHelper.h"
#include "SDL2_image/SDL_image.h"
#include "SDL2_ttf/SDL_ttf.h"
#include "Actor.h"
#include "glm/glm.hpp"
#include "queue"

struct RenderRequest {
	// General elements
	SDL_Texture* texture;
	SDL_FRect destRect;
	SDL_Color color;
	int rotation;
	glm::vec2 pivot;
	glm::vec2 scale;
	glm::vec2 position;
	SDL_RendererFlip flip;
	int sorting_order;
};
 
class Renderer
{
public:
	int x_resolution, y_resolution;
	int clear_color_r, clear_color_g, clear_color_b;
	std::string game_title;
	bool intro = true;

	void initialize(int num_actors);
	void update();
	void present();
	void clearScreen();

	// Lua versions of things
	static void TextDraw(std::string content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a);
	static void UIDraw(std::string image_name, float x, float y);
	static void UIDrawEX(std::string image_name, float x, float y, float r, float g, float b, float a, float sorting_order);
	static void ImageDraw(std::string image_name, float x, float y);
	static void ImageDrawEX(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order);
	static void PixelDraw(float x, float y, float r, float g, float b, float a);
	static void CamSetPos(float x, float y);
	static float CamGetPosX();
	static float CamGetPosY();
	static void CamSetZoom(float zoom_factor);
	static float CamGetZoom();
	static inline glm::vec2 camera_center;
	static inline float camera_zoom;
	static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fontMap;
	static inline std::unordered_map<std::string, SDL_Texture*> textureMap;
	static inline SDL_Renderer* rend;
	static inline SDL_Window* window;
	static inline std::queue<RenderRequest> renderQueueImages;
	static inline std::queue<RenderRequest> renderQueueUI;
	static inline std::queue<RenderRequest> renderQueueText;
	static inline std::queue<RenderRequest> renderQueuePixels;

	static void CreateDefaultParticleTextureWithName(const std::string& name);

	
	//glm::vec2 camera_offset;
	glm::vec2 player_movement;
	std::string bad_ending;
	std::string good_ending;
	int intro_frame = 0;
	float camera_ease_factor;
	//bool x_scale_actor_flipping_on_movement;
	
	Renderer();
	Renderer(int x_res, int y_res, int clear_r, int clear_g, int clear_b, std::string title, std::vector<std::string> intro_img,
		std::vector<std::string> intro_txt, std::string font_type, glm::vec2 camera, glm::vec2 cam_offset, float zoom_factor, float ease_factor, bool move_flip);

private:
	std::vector<std::string> intro_images;
	std::vector<std::string> intro_text;
	std::vector<SDL_Texture*> intro_textures;
	std::vector<SDL_Surface*> text_surfaces;
	std::vector<SDL_Texture*> text_textures;
	std::vector<SDL_Surface*> actor_text_surfaces_nearby;
	std::vector<SDL_Texture*> actor_text_textures_nearby;
	std::vector<SDL_Surface*> actor_text_surfaces_contact;
	std::vector<SDL_Texture*> actor_text_textures_contact;
	int score_reminder = -1;
	std::vector < std::vector <SDL_Texture*>> actor_textures;
	std::vector<Actor*> actor_references;
	std::string font;
	TTF_Font* font_ttf;
};

