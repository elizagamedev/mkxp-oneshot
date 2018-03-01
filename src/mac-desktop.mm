//
// SyngleChance Engine - mac-desktop.mm
// ©2018 Vinyl Darkscratch.  You may use this code for anything you'd like.
// https://www.queengoob.org
//

#include "mac-desktop.h"
#include <string>

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

NSURL *originalBackground;
NSDictionary<NSWorkspaceDesktopImageOptionKey, id> *originalOptions;

NSScreen *mainscreen = [NSScreen mainScreen];
NSWorkspace *sharedworkspace = [NSWorkspace sharedWorkspace];

void MacDesktop::CacheCurrentBackground() {
	originalBackground = [sharedworkspace desktopImageURLForScreen:mainscreen];
	originalOptions = [sharedworkspace desktopImageOptionsForScreen:mainscreen];
}

bool MacDesktop::ChangeBackground(std::string imageURL) {
	NSURL *URL = [NSURL fileURLWithPath:@(imageURL.c_str())];

	BOOL success = [sharedworkspace setDesktopImageURL:[URL absoluteURL] forScreen:mainscreen options:[sharedworkspace desktopImageOptionsForScreen:mainscreen] error:nil];
	return (bool)success;
}

bool MacDesktop::ResetBackground() {
	BOOL success = [sharedworkspace setDesktopImageURL:originalBackground forScreen:mainscreen options:originalOptions error:nil];
	return (bool)success;
}
