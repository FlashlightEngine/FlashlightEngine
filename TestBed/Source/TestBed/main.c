#include <Flashlight/Core/Logger.h>
#include <Flashlight/Core/Asserts.h>

#include <Flashlight/Platform/Platform.h>

int main(void) {
    FL_LOG_FATAL("A test message: %f", 3.14f)
    FL_LOG_ERROR("A test message: %f", 3.14f)
    FL_LOG_WARN("A test message: %f", 3.14f)
    FL_LOG_INFO("A test message: %f", 3.14f)
    FL_LOG_DEBUG("A test message: %f", 3.14f)
    FL_LOG_TRACE("A test message: %f", 3.14f)

    FlPlatformState state;
    if (flPlatformStartup(&state, "Flashlight Engine TestBed", 100, 100, 1280, 720)) {
        while (TRUE) {
            flPlatformPumpMessages(&state);
        }
    }
    flPlatformShutdown(&state);
    
    return 0;
}