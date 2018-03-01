//
// SyngleChance Engine - mac-desktop.mm
// ©2018 Vinyl Darkscratch.  You may use this code for anything you'd like.
// https://www.queengoob.org
//

#include "mac-desktop.h"
#include <string>

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

bool ChangeBackground(std::string imageURL) {
	NSURL *URL = [NSURL fileURLWithPath:@(imageURL.c_str())];
	NSScreen *mainscreen = [NSScreen mainScreen];
	NSWorkspace *sharedworkspace = [NSWorkspace sharedWorkspace];

	BOOL success = [sharedworkspace setDesktopImageURL:[URL absoluteURL] forScreen:mainscreen options:[sharedworkspace desktopImageOptionsForScreen:mainscreen] error:nil];
	return (bool)success;
}

bool ResetBackground() {
	// XXX Implement me!
	return false;
}
