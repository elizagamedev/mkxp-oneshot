/*
** input.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "input.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "keybindings.h"
#include "exception.h"
#include "util.h"

#include <SDL_scancode.h>
#include <SDL_mouse.h>

#include <vector>
#include <string.h>
#include <assert.h>


#define BUTTON_CODE_COUNT 261

// SDLK can range from 0x00 to 0x7f for ones with character representation
// and 0x40000039 to 0x4000011a for ones without character representation
// we squash 0x40000039 - 0x4000007f to 0x139 - 0x17f
// and 0x40000080 - 0x400000ff to 0x80 - 0xff
// and 0x40000100 - 0x4000011a to 0x100 - 0x11a
// very ugly, but this saves a ton of space, allowing us to use only 0x180 codes max (still a ton of holes in it, but close enough)
// convert actual keycode to this scuffed mapping: (keycode & 0xff | (keycode & 0x180 == 0x100 ? 0x180 : 0)) + BUTTONCODE_SDLK_OFFSET
// no need for reverse conversion, fortunately

struct ButtonState
{
	bool pressed;
	bool triggered;
	bool repeated;

	ButtonState()
		: pressed(false),
		  triggered(false),
		  repeated(false)
	{}
};

struct KbBindingData
{
	SDL_Scancode source;
	Input::ButtonCode target;
};

struct Binding
{
	Binding(Input::ButtonCode target = Input::None)
		: target(target)
	{}

	virtual bool sourceActive() const = 0;
	virtual bool sourceRepeatable() const = 0;

	Input::ButtonCode target;
};

/* Keyboard binding */
struct KbBinding : public Binding
{
	KbBinding() {}

	KbBinding(const KbBindingData &data)
		: Binding(data.target),
		  source(data.source)
	{}

	bool sourceActive() const
	{
		/* Special case aliases */
		if (source == SDL_SCANCODE_LSHIFT)
			return EventThread::keyStates[source]
			    || EventThread::keyStates[SDL_SCANCODE_RSHIFT];

		if (source == SDL_SCANCODE_RETURN)
			return EventThread::keyStates[source]
			    || EventThread::keyStates[SDL_SCANCODE_KP_ENTER];

		return EventThread::keyStates[source];
	}

	bool sourceRepeatable() const
	{
		return (source >= SDL_SCANCODE_A     && source <= SDL_SCANCODE_0)    ||
		       (source >= SDL_SCANCODE_RIGHT && source <= SDL_SCANCODE_UP)   ||
		       (source >= SDL_SCANCODE_F1    && source <= SDL_SCANCODE_F12);
	}

	SDL_Scancode source;
};

/* Controller button binding */
struct GcButtonBinding : public Binding
{
	GcButtonBinding() {}

	bool sourceActive() const
	{
		return EventThread::gcState.buttons[source];
	}

	bool sourceRepeatable() const
	{
		return true;
	}

	uint8_t source;
};

/* Controller axis binding */
struct GcAxisBinding : public Binding
{
	GcAxisBinding() {}

	GcAxisBinding(uint8_t source,
	              AxisDir dir,
	              Input::ButtonCode target)
	    : Binding(target),
	      source(source),
	      dir(dir)
	{}

	bool sourceActive() const
	{
		int val = EventThread::gcState.axes[source];

		if (dir == Negative)
			return val < -JAXIS_THRESHOLD;
		else /* dir == Positive */
			return val > JAXIS_THRESHOLD;
	}

	bool sourceRepeatable() const
	{
		return true;
	}

	uint8_t source;
	AxisDir dir;
};

/* Joystick button binding */
struct JsButtonBinding : public Binding
{
	JsButtonBinding() {}

	bool sourceActive() const
	{
		return EventThread::joyState.buttons[source];
	}

	bool sourceRepeatable() const
	{
		return true;
	}

	uint8_t source;
};

/* Joystick axis binding */
struct JsAxisBinding : public Binding
{
	JsAxisBinding() {}

	JsAxisBinding(uint8_t source,
	              AxisDir dir,
	              Input::ButtonCode target)
	    : Binding(target),
	      source(source),
	      dir(dir)
	{}

	bool sourceActive() const
	{
		int val = EventThread::joyState.axes[source];

		if (dir == Negative)
			return val < -JAXIS_THRESHOLD;
		else /* dir == Positive */
			return val > JAXIS_THRESHOLD;
	}

	bool sourceRepeatable() const
	{
		return true;
	}

	uint8_t source;
	AxisDir dir;
};

/* Joystick hat binding */
struct JsHatBinding : public Binding
{
	JsHatBinding() {}

	JsHatBinding(uint8_t source,
	              uint8_t pos,
	              Input::ButtonCode target)
	    : Binding(target),
	      source(source),
	      pos(pos)
	{}

	bool sourceActive() const
	{
		/* For a diagonal input accept it as an input for both the axes */
		return (pos & EventThread::joyState.hats[source]) != 0;
	}

	bool sourceRepeatable() const
	{
		return true;
	}

	uint8_t source;
	uint8_t pos;
};

/* Mouse button binding */
struct MsBinding : public Binding
{
	MsBinding() {}

	MsBinding(int buttonIndex,
	          Input::ButtonCode target)
	    : Binding(target),
	      index(buttonIndex)
	{}

	bool sourceActive() const
	{
		return EventThread::mouseState.buttons[index];
	}

	bool sourceRepeatable() const
	{
		return false;
	}

	int index;
};

/* Not rebindable */
static const KbBindingData staticKbBindings[] =
{
	{ SDL_SCANCODE_F5,     Input::F5    },
	{ SDL_SCANCODE_F6,     Input::F6    },
	{ SDL_SCANCODE_F7,     Input::F7    },
	{ SDL_SCANCODE_F8,     Input::F8    },
	{ SDL_SCANCODE_F9,     Input::F9    }
};

static elementsN(staticKbBindings);

/* Maps ButtonCode enum values to indices
 * in the button state array */
static const int mapToIndex[] =
{
	0, 0,
	1, 0, 2, 0, 3, 0, 4, 0,
	0,
	5, 6, 7, 8, 9, 10, 11, 12,
	0, 0,
	13, 14, 15,
	0,
	16, 17, 18, 19, 20,
	0, 0, 0, 0, 0, 0, 0, 0,
	21, 22, 23,
	//settings & pause
	260, 261
	// pad: index 43 to 59
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	// sdl keycodes, 60 (0x00) to 91 (0x1f)
	24, 0, 0, 0, 0, 0, 0, 0, 25, 26, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0,
	// sdl keycodes, 92 (0x20) to 123 (0x3f)
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
	// sdl keycodes, 124 (0x40) to 155 (0x5f)
	61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 63, 64, 65, 66,
	// sdl keycodes, 156 (0x60) to 187 (0x7f)
	67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 0, 0, 0, 0, 94,
	// sdl keycodes, 188 (0x80 or 0x40000080) to 219 (0x9f or 0x4000009f)
	95, 96, 0, 0, 0, 97, 98, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 99, 100, 101, 102, 103, 104, 105,
	// sdl keycodes, 220 (0xa0 or 0x400000a0) to 251 (0xbf or 0x400000bf)
	106, 107, 108, 109, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126,
	// sdl keycodes, 252 (0xc0 or 0x400000c0) to 283 (0xdf or 0x400000df)
	127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 0, 0,
	// sdl keycodes, 284 (0xe0 or 0x400000e0) to 315 (0xdf or 0x400000ff)
	157, 158, 159, 160, 161, 162, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	// sdl keycodes, 316 (0x100 or 0x40000100) to 347 (0x11f or 0x4000011f) (range is not a typo! read comment at top of file)
	0, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 0, 0, 0, 0, 0,
	// sdl keycodes, 348 (0x120 or 0x40000020) to 379 (0x13f or 0x4000003f)
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 191, 192, 193, 194, 195, 196, 197,
	// sdl keycodes, 380 (0x140 or 0x40000040) to 411 (0x15f or 0x4000005f)
	198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 0, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228,
	// sdl keycodes, 412 (0x160 or 0x40000060) to 443 (0x17f or 0x4000007f)
	229, 230, 231, 232, 0, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259,
};

static elementsN(mapToIndex);

static const Input::ButtonCode dirs[] =
{ Input::Down, Input::Left, Input::Right, Input::Up };

static const int dirFlags[] =
{
	1 << Input::Down,
	1 << Input::Left,
	1 << Input::Right,
	1 << Input::Up
};

/* Dir4 is always zero on these combinations */
static const int deadDirFlags[] =
{
	dirFlags[0] | dirFlags[3],
	dirFlags[1] | dirFlags[2]
};

static const Input::ButtonCode otherDirs[4][3] =
{
	{ Input::Left, Input::Right, Input::Up    }, /* Down  */
	{ Input::Down, Input::Up,    Input::Right }, /* Left  */
	{ Input::Down, Input::Up,    Input::Left  }, /* Right */
	{ Input::Left, Input::Right, Input::Up    }  /* Up    */
};

struct InputPrivate
{
	std::vector<KbBinding> kbStatBindings;
	std::vector<KbBinding> kbBindings;
	std::vector<GcAxisBinding> gcABindings;
	std::vector<GcButtonBinding> gcBBindings;
	std::vector<JsAxisBinding> jsABindings;
	std::vector<JsHatBinding> jsHBindings;
	std::vector<JsButtonBinding> jsBBindings;
	std::vector<MsBinding> msBindings;

	/* Collective binding array */
	std::vector<Binding*> bindings;

	ButtonState stateArray[BUTTON_CODE_COUNT*2];

	ButtonState *states;
	ButtonState *statesOld;

	Input::ButtonCode repeating;
	unsigned int repeatCount;

	bool triedExit;

	struct
	{
		int active;
		Input::ButtonCode previous;
	} dir4Data;

	struct
	{
		int active;
	} dir8Data;


	InputPrivate(const RGSSThreadData &rtData)
	{
		initStaticKbBindings();
		initMsBindings();

		/* Main thread should have these posted by now */
		checkBindingChange(rtData);

		states    = stateArray;
		statesOld = stateArray + BUTTON_CODE_COUNT;

		/* Clear buffers */
		clearBuffer();
		swapBuffers();
		clearBuffer();

		repeating = Input::None;
		repeatCount = 0;

		dir4Data.active = 0;
		dir4Data.previous = Input::None;

		dir8Data.active = 0;

		triedExit = false;
	}

	inline ButtonState &getStateCheck(int code)
	{
		int index;

		if (code < 0 || (size_t) code > mapToIndexN-1)
			index = 0;
		else
			index = mapToIndex[code];

		return states[index];
	}

	inline ButtonState &getState(Input::ButtonCode code)
	{
		return states[mapToIndex[code]];
	}

	inline ButtonState &getOldState(Input::ButtonCode code)
	{
		return statesOld[mapToIndex[code]];
	}

	void swapBuffers()
	{
		ButtonState *tmp = states;
		states = statesOld;
		statesOld = tmp;
	}

	void clearBuffer()
	{
		const size_t size = sizeof(ButtonState) * BUTTON_CODE_COUNT;
		memset(states, 0, size);
	}

	void checkBindingChange(const RGSSThreadData &rtData)
	{
		BDescVec d;

		if (!rtData.bindingUpdateMsg.poll(d))
			return;

		applyBindingDesc(d);
	}

	template<class B>
	void appendBindings(std::vector<B> &bind)
	{
		for (size_t i = 0; i < bind.size(); ++i)
			bindings.push_back(&bind[i]);
	}

	void applyBindingDesc(const BDescVec &d)
	{
		kbBindings.clear();
		gcABindings.clear();
		gcBBindings.clear();
		jsABindings.clear();
		jsHBindings.clear();
		jsBBindings.clear();

		for (size_t i = 0; i < d.size(); ++i)
		{
			const BindingDesc &desc = d[i];
			const SourceDesc &src = desc.src;

			if (desc.target == Input::None)
				continue;

			switch (desc.src.type)
			{
			case Invalid :
				break;
			case Key :
			{
				KbBinding bind;
				bind.source = src.d.scan;
				bind.target = desc.target;
				kbBindings.push_back(bind);

				break;
			}
			case CAxis :
			{
				GcAxisBinding bind;
				bind.source = src.d.ja.axis;
				bind.dir = src.d.ja.dir;
				bind.target = desc.target;
				gcABindings.push_back(bind);

				break;
			}
			case CButton :
			{
				GcButtonBinding bind;
				bind.source = src.d.jb;
				bind.target = desc.target;
				gcBBindings.push_back(bind);

				break;
			}
			case JAxis :
			{
				JsAxisBinding bind;
				bind.source = src.d.ja.axis;
				bind.dir = src.d.ja.dir;
				bind.target = desc.target;
				jsABindings.push_back(bind);

				break;
			}
			case JHat :
			{
				JsHatBinding bind;
				bind.source = src.d.jh.hat;
				bind.pos = src.d.jh.pos;
				bind.target = desc.target;
				jsHBindings.push_back(bind);

				break;
			}
			case JButton :
			{
				JsButtonBinding bind;
				bind.source = src.d.jb;
				bind.target = desc.target;
				jsBBindings.push_back(bind);

				break;
			}
			default :
				assert(!"unreachable");
			}
		}

		bindings.clear();

		appendBindings(kbStatBindings);
		appendBindings(msBindings);

		appendBindings(kbBindings);
		appendBindings(gcABindings);
		appendBindings(gcBBindings);
		appendBindings(jsABindings);
		appendBindings(jsHBindings);
		appendBindings(jsBBindings);
	}

	void initStaticKbBindings()
	{
		kbStatBindings.clear();

		for (size_t i = 0; i < staticKbBindingsN; ++i)
			kbStatBindings.push_back(KbBinding(staticKbBindings[i]));
	}

	void initMsBindings()
	{
		msBindings.resize(3);

		size_t i = 0;
		msBindings[i++] = MsBinding(SDL_BUTTON_LEFT,   Input::MouseLeft);
		msBindings[i++] = MsBinding(SDL_BUTTON_MIDDLE, Input::MouseMiddle);
		msBindings[i++] = MsBinding(SDL_BUTTON_RIGHT,  Input::MouseRight);
	}

	void pollBindings(Input::ButtonCode &repeatCand)
	{
		for (size_t i = 0; i < bindings.size(); ++i)
			pollBindingPriv(*bindings[i], repeatCand);

		for(int i = 0; i < BUTTONCODE_SDLK_COUNT; i++)
			pollKeyboardCode(i);

		updateDir4();
		updateDir8();
	}

	void pollBindingPriv(const Binding &b,
	                     Input::ButtonCode &repeatCand)
	{	
		if (!b.sourceActive())
			return;

		if (b.target == Input::None)
			return;

		ButtonState &state = getState(b.target);
		ButtonState &oldState = getOldState(b.target);

		state.pressed = true;

		/* Must have been released before to trigger */
		if (!oldState.pressed)
			state.triggered = true;

		/* Unbound keys don't create/break repeat */
		if (repeatCand != Input::None)
			return;

		if (repeating != b.target &&
			!oldState.pressed)
		{
			if (b.sourceRepeatable())
				repeatCand = b.target;
			else
				/* Unrepeatable keys still break current repeat */
				repeating = Input::None;
		}
	}

	void pollKeyboardCode(int i) {
		if (!EventThread::keyStates[SDL_NUM_SCANCODES + i])
			return;
		ButtonState & state = getState((Input::ButtonCode) (BUTTONCODE_SDLK_OFFSET + i));
		ButtonState & oldState = getOldState((Input::ButtonCode) (BUTTONCODE_SDLK_OFFSET + i));
		state.pressed = true;
		state.triggered = !oldState.pressed;
	}

	void updateDir4()
	{
		int dirFlag = 0;

		for (size_t i = 0; i < 4; ++i)
			dirFlag |= (getState(dirs[i]).pressed ? dirFlags[i] : 0);

		if (dirFlag == deadDirFlags[0] || dirFlag == deadDirFlags[1])
		{
			dir4Data.active = Input::None;
			return;
		}

		if (dir4Data.previous != Input::None)
		{
			/* Check if prev still pressed */
			if (getState(dir4Data.previous).pressed)
			{
				for (size_t i = 0; i < 3; ++i)
				{
					Input::ButtonCode other =
							otherDirs[(dir4Data.previous/2)-1][i];

					if (!getState(other).pressed)
						continue;

					dir4Data.active = other;
					return;
				}
			}
		}

		for (size_t i = 0; i < 4; ++i)
		{
			if (!getState(dirs[i]).pressed)
				continue;

			dir4Data.active = dirs[i];
			dir4Data.previous = dirs[i];
			return;
		}

		dir4Data.active   = Input::None;
		dir4Data.previous = Input::None;
	}

	void updateDir8()
	{
		static const int combos[4][4] =
		{
			{ 2, 1, 3, 0 },
			{ 1, 4, 0, 7 },
			{ 3, 0, 6, 9 },
			{ 0, 7, 9, 8 }
		};

		dir8Data.active = 0;

		for (size_t i = 0; i < 4; ++i)
		{
			Input::ButtonCode one = dirs[i];

			if (!getState(one).pressed)
				continue;

			for (int j = 0; j < 3; ++j)
			{
				Input::ButtonCode other = otherDirs[i][j];

				if (!getState(other).pressed)
					continue;

				dir8Data.active = combos[(one/2)-1][(other/2)-1];
				return;
			}

			dir8Data.active = one;
			return;
		}
	}
};


Input::Input(const RGSSThreadData &rtData)
{
	p = new InputPrivate(rtData);
}

void Input::update()
{
	shState->checkShutdown();
	p->checkBindingChange(shState->rtData());

	p->swapBuffers();
	p->clearBuffer();

	ButtonCode repeatCand = None;

	/* Poll all bindings */
	p->pollBindings(repeatCand);
	modkeys = EventThread::modkeys;

	/* Check for new repeating key */
	if (repeatCand != None && repeatCand != p->repeating)
	{
		p->repeating = repeatCand;
		p->repeatCount = 0;
		p->getState(repeatCand).repeated = true;

		return;
	}

	/* Check if repeating key is still pressed */
	if (p->getState(p->repeating).pressed)
	{
		p->repeatCount++;

		bool repeated;
		if (rgssVer >= 2)
			repeated = p->repeatCount >= 23 && ((p->repeatCount+1) % 6) == 0;
		else
			repeated = p->repeatCount >= 15 && ((p->repeatCount+1) % 4) == 0;

		p->getState(p->repeating).repeated |= repeated;

		return;
	}

	p->repeating = None;

	RGSSThreadData &rtData = shState->rtData();
	p->triedExit = rtData.triedExit;
	rtData.triedExit.clear();
}

bool Input::isPressed(int button)
{
	return p->getStateCheck(button).pressed;
}

bool Input::isTriggered(int button)
{
	return p->getStateCheck(button).triggered;
}

bool Input::isRepeated(int button)
{
	return p->getStateCheck(button).repeated;
}

int Input::dir4Value()
{
	return p->dir4Data.active;
}

int Input::dir8Value()
{
	return p->dir8Data.active;
}

int Input::mouseX()
{
	RGSSThreadData &rtData = shState->rtData();

	return (EventThread::mouseState.x - rtData.screenOffset.x) * rtData.sizeResoRatio.x;
}

int Input::mouseY()
{
	RGSSThreadData &rtData = shState->rtData();

	return (EventThread::mouseState.y - rtData.screenOffset.y) * rtData.sizeResoRatio.y;
}

bool Input::hasQuit()
{
	return p->triedExit;
}

Input::~Input()
{
	delete p;
}
