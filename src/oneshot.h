#ifndef ONESHOT_H
#define ONESHOT_H

#include "etc-internal.h"
#include <string>

struct OneshotPrivate;
struct RGSSThreadData;

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
	const std::vector<uint8_t> &obscuredMap() const;
	bool obscuredCleared() const;
	bool allowExit() const;

	//Mutators
	void setYesNo(const char *yes, const char *no);
	void setWindowPos(int x, int y);
	void setAllowExit(bool allowExit);
	void resetObscured();

	//Functions
	bool msgbox(int type, const char *body, const char *title);

	//Dirty flag for obscured texture
	bool obscuredDirty;

private:
	OneshotPrivate *p;
	RGSSThreadData &threadData;
};

#endif // ONESHOT_H
