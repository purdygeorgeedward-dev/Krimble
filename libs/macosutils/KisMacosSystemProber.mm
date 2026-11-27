/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2023 Ivan Santa María <ghevan@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#import "KisMacosSystemProber.h"

bool iskritaRunningActivate()
{
    NSRunningApplication *current = [NSRunningApplication currentApplication];
    NSArray<NSRunningApplication*> *openApps;
    openApps = [NSWorkspace sharedWorkspace].runningApplications;

    uint krita = 0;
    for (NSRunningApplication *app in openApps) {
        if ([app.bundleIdentifier isEqualToString:current.bundleIdentifier]) {
            krita++;
            [app activateWithOptions:NSApplicationActivateIgnoringOtherApps];
        }
    }

    return (krita > 1);
}
