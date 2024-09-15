#include <Flashlight/Core/Logger.h>

int main() {
    FL_LOG_FATAL("A test message %f", 3.14f)
    FL_LOG_ERROR("A test message %f", 3.14f)
    FL_LOG_WARN("A test message %f", 3.14f)
    FL_LOG_INFO("A test message %f", 3.14f)
    FL_LOG_DEBUG("A test message %f", 3.14f)
    FL_LOG_TRACE("A test message %f", 3.14f)

    return 0;
}