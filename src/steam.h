#ifndef STEAM_H
#define STEAM_H

#include <string>

struct SteamPrivate;

class Achievement
{
public:
	enum ID
	{
		Invalid = -1,

		SaveWorld = 0,
		SaveNiko,

		MAX
	};

	static const char *const names[MAX];

	void unlock();
	void lock();
	bool isUnlocked() { return unlocked; }

private:
	const char *name;
	bool unlocked;

	friend struct SteamPrivate;

	Achievement()
	    : name(0),
	      unlocked(false)
	{}
	Achievement(const char *name)
	    : name(name),
	      unlocked(false)
	{
		update();
	}

	void update();
};

class Steam
{
public:
	void unlock(Achievement::ID id);
	void lock(Achievement::ID id);
	bool isUnlocked(Achievement::ID id);

	const std::string &userName() const;

private:
	Steam();
	~Steam();

	friend struct SharedStatePrivate;

	SteamPrivate *p;
};

#endif // STEAM_H
