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

NSScreen *screen = [NSScreen mainScreen];
NSWorkspace *sharedworkspace = [NSWorkspace sharedWorkspace];

void MacDesktop::CacheCurrentBackground() {
	if (isCached == NO) {
		originalBackground = [sharedworkspace desktopImageURLForScreen:screen];
		originalOptions = [sharedworkspace desktopImageOptionsForScreen:screen];
	}
	isCached = YES;
}

void MacDesktop::ChangeBackground(std::string imageURL, double red, double green, double blue) {
	NSURL *URL = [NSURL fileURLWithPath:@(imageURL.c_str())];
	NSDictionary<NSWorkspaceDesktopImageOptionKey, id> *options = @{NSWorkspaceDesktopImageScalingKey : @3, NSWorkspaceDesktopImageAllowClippingKey : @0, NSWorkspaceDesktopImageFillColorKey : [NSColor colorWithSRGBRed:red green:green blue:blue alpha:1.0]};

	[sharedworkspace setDesktopImageURL:[URL absoluteURL] forScreen:screen options:options error:nil];
}

void MacDesktop::ResetBackground() {
	if (isCached) [sharedworkspace setDesktopImageURL:originalBackground forScreen:screen options:originalOptions error:nil];
}
