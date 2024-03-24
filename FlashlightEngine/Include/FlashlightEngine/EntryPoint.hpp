/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * EntryPoint.hpp - Entry point for every application.
 * This file contains the entry point for every application using Flashlight engine.
 */
#pragma once

#include <memory>

#if defined(_WIN64) || defined(__linux__)

extern std::unique_ptr<Flashlight::Application> Flashlight::CreateApp();

int main(int argc, char *argv[]) {
    const auto app = Flashlight::CreateApp();
    try {
        app->Run();
    } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif