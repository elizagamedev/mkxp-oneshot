//
// SyngleChance Engine - mac-desktop.h
// ©2018 Vinyl Darkscratch.  You may use this code for anything you'd like.
// https://www.queengoob.org
//

#pragma once

#include <string>

namespace MacDesktop {
	void CacheCurrentBackground();
	bool ChangeBackground(std::string imageURL);
	bool ResetBackground();
}