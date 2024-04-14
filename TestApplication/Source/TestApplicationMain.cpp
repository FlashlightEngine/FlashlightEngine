#include "TestApplication.hpp"

int main() {
    std::unique_ptr<TestApplication::TestApplication> application = std::make_unique<TestApplication::TestApplication>();

    try {
        application->Run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "\\033[2J\\033[1;1H";

    return EXIT_SUCCESS;
}
