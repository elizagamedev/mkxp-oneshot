#ifndef ONESHOT_H
#define ONESHOT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "etc-internal.h"
#include <string>

struct OneshotPrivate;
struct RGSSThreadData;

//Texture wrapper class
class LTexture {
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile(std::string path, SDL_Renderer *gRenderer);

		#ifdef _SDL_TTF_H
		//Creates image from font string
		bool loadFromRenderedText(std::string textureText, SDL_Color textColor, SDL_Renderer *gRenderer, TTF_Font *gFont);
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor(Uint8 red, Uint8 green, Uint8 blue);

		//Set blending
		void setBlendMode(SDL_BlendMode blending);

		//Set alpha modulation
		void setAlpha(Uint8 alpha);

		//Renders texture at given point
		void render(SDL_Renderer *gRenderer, int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL,
		            SDL_RendererFlip flip = SDL_FLIP_NONE);

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture *mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

class Oneshot
{
public:
	Oneshot(RGSSThreadData &threadData);
	~Oneshot();

	//msgbox type codes
	enum
	{
		MSG_INFO,
		MSG_YESNO,
		MSG_WARN,
		MSG_ERR,
	};

	//Wallpaper style
	enum
	{
		STYLE_NONE,
		STYLE_TILE,
		STYLE_CENTER,
		STYLE_STRETCH,
		STYLE_FIT,
		STYLE_FILL,
		STYLE_SPAN,
	};

	//Wallpaper gradient
	enum
	{
		GRADIENT_NONE,
		GRADIENT_HORIZONTAL,
		GRADIENT_VERTICAL,
	};

	void update();

	//Accessors
	const std::string &lang() const;
	const std::string &userName() const;
	const std::string &savePath() const;
	const std::string &docsPath() const;
	const std::string &gamePath() const;
	const std::string &journal() const;
	const std::vector<uint8_t> &obscuredMap() const;
	bool obscuredCleared() const;
	bool allowExit() const;
	bool exiting() const;

	//Mutators
	void setYesNo(const char *yes, const char *no);
	void setWindowPos(int x, int y);
	void setExiting(bool exiting);
	void setAllowExit(bool allowExit);
	void resetObscured();

	//Functions
	bool msgbox(int type, const char *body, const char *title);
	std::string textinput(const char* prompt, int char_limit, const char* fontName);

	//Dirty flag for obscured texture
	bool obscuredDirty;

private:
	OneshotPrivate *p;
	RGSSThreadData &threadData;
};

#endif // ONESHOT_H
