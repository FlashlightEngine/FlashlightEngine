#include "TestApplication.hpp"

#include <FlashlightEngine/FlashlightEngine.hpp>

int main() {    
    std::unique_ptr<TestApplication::TestApplication> application = std::make_unique<TestApplication::TestApplication>();

    try {
        std::cout << "Starting application.";
        application->Run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
