#include "TestApplication.hpp"

int main() {
    std::unique_ptr<TestApplication::TestApplication> application = std::make_unique<TestApplication::TestApplication>();

    try {
        application->Run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
