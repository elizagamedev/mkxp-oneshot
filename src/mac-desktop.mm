//
// SyngleChance Engine - mac-desktop.mm
// ©2018 Vinyl Darkscratch.  You may use this code for anything you'd like.
// https://www.queengoob.org
//

#include "mac-desktop.h"
#include <string>

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

BOOL isCached = NO;
NSURL *originalBackground;
NSDictionary<NSWorkspaceDesktopImageOptionKey, id> *originalOptions;

NSScreen *mainscreen = [NSScreen mainScreen];
NSWorkspace *sharedworkspace = [NSWorkspace sharedWorkspace];

void MacDesktop::CacheCurrentBackground() {
	originalBackground = [sharedworkspace desktopImageURLForScreen:mainscreen];
	originalOptions = [sharedworkspace desktopImageOptionsForScreen:mainscreen];
	isCached = YES;
}

bool MacDesktop::ChangeBackground(std::string imageURL) {
	NSURL *URL = [NSURL fileURLWithPath:@(imageURL.c_str())];
	NSDictionary<NSWorkspaceDesktopImageOptionKey, id> *options = @{NSWorkspaceDesktopImageScalingKey : @0, NSWorkspaceDesktopImageAllowClippingKey : @NO, NSWorkspaceDesktopImageFillColorKey : [NSColor blackColor]};

	BOOL success = [sharedworkspace setDesktopImageURL:[URL absoluteURL] forScreen:mainscreen options:options error:nil];
	return (bool)success;
}

bool MacDesktop::ResetBackground() {
	if (isCached == NO) return true;
	BOOL success = [sharedworkspace setDesktopImageURL:originalBackground forScreen:mainscreen options:originalOptions error:nil];
	return (bool)success;
}
