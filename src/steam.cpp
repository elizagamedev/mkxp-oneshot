#include "steam.h"
#include "debugwriter.h"

#include <map>
#include <SDL2/SDL.h>
#include "steamshim/steamshim_child.h"

/* Achievements */
static const char *const achievementNames[] = {
    "SaveWorld",
    "SaveNiko",
};
#define NUM_ACHIEVEMENTS (sizeof(achievementNames) / sizeof(achievementNames[0]))

/* SteamPrivate */
struct SteamPrivate
{
	std::map<std::string, bool> achievements;

	std::string userName;

	SteamPrivate()
	{
		STEAMSHIM_getPersonaName();
		STEAMSHIM_getCurrentGameLanguage();
		for (size_t i = 0; i < NUM_ACHIEVEMENTS; ++i)
			STEAMSHIM_getAchievement(achievementNames[i]);
		while (!initialized())
		{
			SDL_Delay(100);
			update();
		}
	}

	void setAchievement(const char *name, bool set)
	{
		achievements[name] = set;
		STEAMSHIM_setAchievement(name, set);
		STEAMSHIM_storeStats();
	}

	void updateAchievement(const char *name, bool isSet)
	{
		achievements[name] = isSet;
	}

	bool isAchievementSet(const char *name)
	{
		return achievements[name];
	}

	void update()
	{
		const STEAMSHIM_Event *e;
		while ((e = STEAMSHIM_pump()) != 0)
		{
			/* Skip erroneous events */
			if (!e->okay)
				continue;
			/* Handle events */
            switch (e->type)
			{
			case SHIMEVENT_GETACHIEVEMENT:
				updateAchievement(e->name, e->ivalue);
				break;
			case SHIMEVENT_GETPERSONANAME:
				userName = e->name;
				break;
			default:
				break;
			}
        }
	}

	bool initialized()
	{
		return !userName.empty()
		        && achievements.size() == NUM_ACHIEVEMENTS;
	}
};

/* Steam */
Steam::Steam()
    : p(new SteamPrivate())
{
}

Steam::~Steam()
{
	delete p;
}

const std::string &Steam::userName() const
{
	return p->userName;
}

void Steam::unlock(const char *name)
{
	p->setAchievement(name, true);
}

void Steam::lock(const char *name)
{
	p->setAchievement(name, false);
}

bool Steam::isUnlocked(const char *name)
{
	return p->isAchievementSet(name);
}
