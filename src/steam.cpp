#include "steam.h"
#include "debugwriter.h"

#include <steam/steam_api.h>

/* Achievements */
const char *const Achievement::names[Achievement::MAX] = {
    "SaveWorld",
    "SaveNiko",
};

void Achievement::update()
{
	SteamUserStats()->GetAchievement(name, &unlocked);
}

void Achievement::unlock()
{
	unlocked = true;
	SteamUserStats()->SetAchievement(name);
	SteamUserStats()->StoreStats();
}

void Achievement::lock()
{
	unlocked = false;
	SteamUserStats()->ClearAchievement(name);
	SteamUserStats()->StoreStats();
}

/* SteamPrivate */
struct SteamPrivate
{
	Achievement achievements[Achievement::MAX];

	int appid;
	std::string userName;

	SteamPrivate()
	    : appid(SteamUtils()->GetAppID()),
	      userName(SteamFriends()->GetPersonaName())
	{
		for (int i = 0; i < Achievement::MAX; ++i)
			achievements[i] = Achievement(Achievement::names[i]);
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

void Steam::unlock(Achievement::ID id)
{
	p->achievements[id].unlock();
}

void Steam::lock(Achievement::ID id)
{
	p->achievements[id].lock();
}

bool Steam::isUnlocked(Achievement::ID id)
{
	return p->achievements[id].isUnlocked();
}
