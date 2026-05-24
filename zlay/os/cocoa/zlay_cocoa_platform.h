#ifndef ZLAY_OS_COCOA_ZLAY_COCOA_PLATFORM_H
#define ZLAY_OS_COCOA_ZLAY_COCOA_PLATFORM_H

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#  define ZLAY_COCOA_TARGET_IOS 1
#else
#  define ZLAY_COCOA_TARGET_IOS 0
#endif

#if ZLAY_COCOA_TARGET_IOS
#  import <UIKit/UIKit.h>
#else
#  import <AppKit/AppKit.h>
#endif

#import <QuartzCore/QuartzCore.h>

#endif
