#ifndef STEAM_H
#define STEAM_H

#include <string>

struct SteamPrivate;

class Steam
{
public:
	void unlock(const char *name);
	void lock(const char *name);
	bool isUnlocked(const char *name);

	const std::string &userName() const;
	const std::string &lang() const;

private:
	Steam();
	~Steam();

	friend struct SharedStatePrivate;

	SteamPrivate *p;
};

#endif // STEAM_H
