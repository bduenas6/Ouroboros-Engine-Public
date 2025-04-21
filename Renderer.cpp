#include "Renderer.h"
#include "Helper.h"
#include "AudioHelper.h"
#include "Actor.h"
#include "Input.h"

inline static bool comp(RenderRequest a, RenderRequest b) {
	return a.sorting_order < b.sorting_order;
}

//-------------------------------------------------------------------------------------

void Renderer::initialize(int num_actors)
{
	window = Helper::SDL_CreateWindow(game_title.c_str(), 50, 50, x_resolution, y_resolution, 0);
	rend = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(rend, clear_color_r, clear_color_g, clear_color_b, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(rend);
	TTF_Init();
}

void Renderer::update()
{
	SDL_SetRenderDrawColor(rend, clear_color_r, clear_color_g, clear_color_b, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(rend);
	float scene_unit = 100.0f; // * camera_zoom

	// Image Rendering (Scene)
	std::vector<RenderRequest> ImageRequests;
	while (!Renderer::renderQueueImages.empty()) {
		RenderRequest request = Renderer::renderQueueImages.front();
		Renderer::renderQueueImages.pop();
		ImageRequests.push_back(request);
	}
	std::stable_sort(ImageRequests.begin(), ImageRequests.end(), comp);
	SDL_RenderSetScale(rend, Renderer::camera_zoom, Renderer::camera_zoom);
	for (auto request : ImageRequests) {
		SDL_FPoint pivot;
		pivot.x = request.pivot.x;
		pivot.y = request.pivot.y;
		request.destRect.x = (((request.destRect.x - camera_center.x) * scene_unit) + (x_resolution * 0.5f * (1.0f / camera_zoom)) - pivot.x);
		request.destRect.y = (((request.destRect.y - camera_center.y) * scene_unit) + (y_resolution * 0.5f * (1.0f / camera_zoom)) - pivot.y);
		request.destRect.w *= glm::abs(request.scale.x);
		request.destRect.h *= glm::abs(request.scale.y);
		SDL_SetTextureColorMod(request.texture, request.color.r, request.color.g, request.color.b);
		SDL_SetTextureAlphaMod(request.texture, request.color.a);
		// Debug render log because environment variables are annoying
		//std::cout << "dstrect: " << request.destRect.x << " " << request.destRect.y << " " << request.destRect.w << " " << request.destRect.h << " angle " << request.rotation << " center " << request.pivot.x << " " << request.pivot.y << " flip " << request.flip << " renderscale " << request.scale.x << " " << request.scale.y << std::endl;
		Helper::SDL_RenderCopyEx(0, "", rend, request.texture, NULL, &request.destRect, request.rotation, &pivot, request.flip);
	}
	SDL_RenderSetScale(rend, 1, 1);
	// UI Element Rendering (Screen)
	std::vector<RenderRequest> UIRequests;
	while (!Renderer::renderQueueUI.empty()) {
		RenderRequest request = Renderer::renderQueueUI.front();
		Renderer::renderQueueUI.pop();
		UIRequests.push_back(request);
	}
	std::stable_sort(UIRequests.begin(), UIRequests.end(), comp);
	for (auto request : UIRequests) {
		SDL_SetTextureColorMod(request.texture, request.color.r, request.color.g, request.color.b);
		SDL_SetTextureAlphaMod(request.texture, request.color.a);
		Helper::SDL_RenderCopyEx(0, "", rend, request.texture, NULL, &request.destRect, 0, NULL, SDL_FLIP_NONE);
	}
	// Text Rendering (Screen)
	while (!Renderer::renderQueueText.empty()) {
		RenderRequest request = Renderer::renderQueueText.front();
		Renderer::renderQueueText.pop();
		Helper::SDL_RenderCopyEx(0, "", rend, request.texture, NULL, &request.destRect, 0, NULL, SDL_FLIP_NONE);
	}
	// Pixel Rendering (Screen)
	while (!Renderer::renderQueuePixels.empty()) {
		RenderRequest request = Renderer::renderQueuePixels.front();
		Renderer::renderQueuePixels.pop();
		SDL_SetRenderDrawColor(rend, request.color.r, request.color.g, request.color.b, request.color.a);
		SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
		SDL_RenderDrawPoint(rend, request.position.x, request.position.y);
		SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
	}

	return;
}

void Renderer::present()
{
	Helper::SDL_RenderPresent(rend);
}


void Renderer::clearScreen()
{
	SDL_SetRenderDrawColor(rend, clear_color_r, clear_color_g, clear_color_b, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(rend);
}

void Renderer::TextDraw(std::string content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a)
{
	x = static_cast<int>(x);
	y = static_cast<int>(y);
	font_size = static_cast<int>(font_size);
	r = static_cast<int>(r);
	g = static_cast<int>(g);
	b = static_cast<int>(b);
	a = static_cast<int>(a);
	TTF_Font* new_font;
	if (fontMap.find(font_name) == fontMap.end() || fontMap[font_name].find(font_size) == fontMap[font_name].end()) {
		std::string font_location = "resources/fonts/" + font_name + ".ttf";
		new_font = TTF_OpenFont(font_location.c_str(), font_size);
		fontMap[font_name][font_size] = new_font;
	}
	else new_font = fontMap[font_name][font_size];
	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	SDL_Surface* text_surface = TTF_RenderText_Solid(fontMap[font_name][font_size], content.c_str(), color);
	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(rend, text_surface);
	SDL_FRect dest_rect;
	dest_rect.x = x;
	dest_rect.y = y;
	dest_rect.w = text_surface->w;
	dest_rect.h = text_surface->h;
	RenderRequest temp;
	temp.color = color;
	temp.texture = text_texture;
	temp.destRect = dest_rect;
	temp.position = glm::vec2(x, y);
	//std::cout << "Before: " << Renderer::renderQueueText.size() << std::endl;
	renderQueueText.push(temp);
	//std::cout << "After: " << Renderer::renderQueueText.size() << std::endl;
}

void Renderer::UIDraw(std::string image_name, float x, float y)
{
	x = static_cast<int>(x);
	y = static_cast<int>(y);
	SDL_Texture* UITexture;
	if (textureMap.find(image_name) == textureMap.end()) {
		std::string image_location = "resources/images/" + image_name + ".png";
		UITexture = IMG_LoadTexture(rend, image_location.c_str());
		textureMap[image_name] = UITexture;
	}
	else UITexture = textureMap[image_name];
	SDL_Color color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;
	SDL_FRect dest_rect;
	dest_rect.x = x;
	dest_rect.y = y;
	float w, h;
	Helper::SDL_QueryTexture(UITexture, &w, &h);
	dest_rect.w = w;
	dest_rect.h = h;
	RenderRequest temp;
	temp.color = color;
	temp.texture = UITexture;
	temp.position = glm::vec2(x, y);
	temp.sorting_order = 0;
	temp.destRect = dest_rect;
	renderQueueUI.push(temp);	
}

void Renderer::UIDrawEX(std::string image_name, float x, float y, float r, float g, float b, float a, float sorting_order)
{
	x = static_cast<int>(x);
	y = static_cast<int>(y);
	r = static_cast<int>(r);
	g = static_cast<int>(g);
	b = static_cast<int>(b);
	a = static_cast<int>(a);
	sorting_order = static_cast<int>(sorting_order);
	SDL_Texture* UITexture;
	if (textureMap.find(image_name) == textureMap.end()) {
		std::string image_location = "resources/images/" + image_name + ".png";
		UITexture = IMG_LoadTexture(rend, image_location.c_str());
		textureMap[image_name] = UITexture;
	}
	else UITexture = textureMap[image_name];
	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	SDL_FRect dest_rect;
	dest_rect.x = x;
	dest_rect.y = y;
	float w, h;
	Helper::SDL_QueryTexture(UITexture, &w, &h);
	dest_rect.w = w;
	dest_rect.h = h;
	RenderRequest temp;
	temp.color = color;
	temp.texture = UITexture;
	temp.position = glm::vec2(x, y);
	temp.sorting_order = sorting_order;
	temp.destRect = dest_rect;
	renderQueueUI.push(temp);
}

void Renderer::ImageDraw(std::string image_name, float x, float y)
{
	SDL_Texture* ImageTexture;
	if (textureMap.find(image_name) == textureMap.end()) {
		std::string image_location = "resources/images/" + image_name + ".png";
		ImageTexture = IMG_LoadTexture(rend, image_location.c_str());
		textureMap[image_name] = ImageTexture;
	}
	else ImageTexture = textureMap[image_name];
	SDL_Color color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;
	SDL_FRect dest_rect;
	dest_rect.x = x; //stays as float
	dest_rect.y = y; //stays as float
	float w, h;
	Helper::SDL_QueryTexture(ImageTexture, &w, &h);
	dest_rect.w = w;
	dest_rect.h = h;
	RenderRequest temp;
	temp.color = color;
	temp.texture = ImageTexture;
	temp.position = glm::vec2(x, y);
	temp.sorting_order = 0;
	float final_pivot_x = 0;
	float final_pivot_y = 0;
	final_pivot_x = (0.5f * dest_rect.w);
	final_pivot_y = (0.5f * dest_rect.h);
	temp.pivot = glm::vec2(final_pivot_x, final_pivot_y);
	temp.rotation = 0;
	temp.scale = glm::vec2(1, 1);
	temp.destRect = dest_rect;
	temp.flip = SDL_FLIP_NONE;
	renderQueueImages.push(temp);
}

void Renderer::ImageDrawEX(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order)
{
	rotation_degrees = static_cast<int>(rotation_degrees);
	r = static_cast<int>(r);
	g = static_cast<int>(g);
	b = static_cast<int>(b);
	a = static_cast<int>(a);
	sorting_order = static_cast<int>(sorting_order);
	SDL_Texture* ImageTexture;
	if (textureMap.find(image_name) == textureMap.end()) {
		std::string image_location = "resources/images/" + image_name + ".png";
		ImageTexture = IMG_LoadTexture(rend, image_location.c_str());
		textureMap[image_name] = ImageTexture;
	}
	else ImageTexture = textureMap[image_name];
	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	SDL_FRect dest_rect;
	dest_rect.x = x; //stays as float
	dest_rect.y = y; //stays as float
	float w, h;
	Helper::SDL_QueryTexture(ImageTexture, &w, &h);
	dest_rect.w = w;
	dest_rect.h = h;
	RenderRequest temp;
	temp.color = color;
	temp.texture = ImageTexture;
	temp.position = glm::vec2(x, y);
	temp.sorting_order = sorting_order;

	float final_pivot_x = 0;
	float final_pivot_y = 0;
	final_pivot_x = (pivot_x * dest_rect.w * scale_x);
	final_pivot_y = (pivot_y * dest_rect.h * scale_y);
	temp.pivot = glm::vec2(final_pivot_x, final_pivot_y);
	
	
	if (glm::sign(scale_x) == -1) {
		if (glm::sign(scale_y) == -1) {
			temp.flip = SDL_FLIP_NONE;
			rotation_degrees += 180;
		}
		else {
			temp.flip = SDL_FLIP_HORIZONTAL;
		}
	}
	else {
		if (glm::sign(scale_y) == -1) {
			temp.flip = SDL_FLIP_VERTICAL;
		}
		else {
			temp.flip = SDL_FLIP_NONE;
		}
	}
	temp.scale = glm::vec2(scale_x, scale_y); //stays as float
	temp.rotation = rotation_degrees;
	temp.destRect = dest_rect;
	renderQueueImages.push(temp);
}

void Renderer::PixelDraw(float x, float y, float r, float g, float b, float a)
{
	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	RenderRequest temp;
	temp.color = color;
	temp.position = glm::vec2(x, y);
	renderQueuePixels.push(temp);
}

void Renderer::CamSetPos(float x, float y)
{
	Renderer::camera_center = glm::vec2(x, y);
}

float Renderer::CamGetPosX()
{
	return Renderer::camera_center.x;
}

float Renderer::CamGetPosY()
{
	return Renderer::camera_center.y;
}

void Renderer::CamSetZoom(float zoom_factor)
{
	Renderer::camera_zoom = zoom_factor;
}

float Renderer::CamGetZoom()
{
	return Renderer::camera_zoom;
}

void Renderer::CreateDefaultParticleTextureWithName(const std::string& name)
{
	if (textureMap.find(name) != textureMap.end()) {
		return;
	}

	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 32, SDL_PIXELFORMAT_RGBA8888);

	Uint32 white_color = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
	SDL_FillRect(surface, NULL, white_color);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface);

	SDL_FreeSurface(surface);
	textureMap[name] = texture;
}

Renderer::Renderer()
{
	x_resolution = 640;
	y_resolution = 360;
	clear_color_r = 255;
	clear_color_g = 255;
	clear_color_b = 255;
	game_title = "";
	window = nullptr;
	rend = nullptr;
	font_ttf = nullptr;
	camera_center = glm::ivec2(0, 0);
	//camera_offset = glm::vec2(0, 0);
	player_movement = glm::vec2(0, 0);
	bad_ending = "";
	good_ending = "";
	camera_zoom = 1.0f;
	camera_ease_factor = 1.0f;
}

Renderer::Renderer(int x_res, int y_res, int clear_r, int clear_g, int clear_b, std::string title, std::vector<std::string> intro_img,
	std::vector<std::string> intro_txt, std::string font_type, glm::vec2 camera, glm::vec2 cam_offset, 
	float zoom_factor, float ease_factor, bool move_flip)
{
	x_resolution = x_res;
	y_resolution = y_res;
	clear_color_r = clear_r;
	clear_color_g = clear_g;
	clear_color_b = clear_b;
	game_title = title;
	window = nullptr;
	rend = nullptr;
	font_ttf = nullptr;
	intro_images = intro_img;
	intro_text = intro_txt;
	font = font_type;
	camera_center = camera;
	player_movement = glm::vec2(0, 0);
	camera_zoom = zoom_factor;
	camera_ease_factor = ease_factor;
}
