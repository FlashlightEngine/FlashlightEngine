#include "TestApplication.hpp"

#include <FlashlightEngine/FlashlightEngine.hpp>

int main() {    
    std::unique_ptr<TestApplication::TestApplication> application = std::make_unique<TestApplication::TestApplication>();

    application->Run();

    return EXIT_SUCCESS;
}
