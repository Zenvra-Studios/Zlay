#include <os/cocoa/zlay_cocoa_backend.h>
#import <os/cocoa/zlay_cocoa_platform.h>

void ZLay_Cocoa_PollEvents(void) {
#if ZLAY_COCOA_TARGET_IOS
  /* UIKit owns its run loop; host apps forward touch/input events explicitly. */
#else
  @autoreleasepool {
    for (;;) {
      NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                          untilDate:[NSDate distantPast]
                                             inMode:NSDefaultRunLoopMode
                                            dequeue:YES];
      if (event == nil) {
        break;
      }
      [NSApp sendEvent:event];
    }
  }
#endif
}
