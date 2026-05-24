#ifndef ZLAY_OS_SYSTEM_ZLAY_PATH_H
#define ZLAY_OS_SYSTEM_ZLAY_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zlay.h>

ZLAY_API const char* ZLay_GetExecutablePath(void);
ZLAY_API const char* ZLay_GetConfigPath(void);
ZLAY_API const char* ZLay_GetCachePath(void);
ZLAY_API const char* ZLay_GetTempPath(void);

#ifdef __cplusplus
}
#endif

#endif
