# Contributing
This file is a guide to help you to contribute to Flashlight Engine. It contains conventions and rules to follow when
contributing to the engine.

## File header
Every file should have a header which should be presented with the following form : 
```c++
/* Copyright (C) <year> <Author> (<Author E-Mail address>)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : <File name>
 * Description : <File description>
 */
```

## Modules
A module is a part of the engine, whose code is stored in the folder named with the module's name. For example `Core` is a
module, and its code is stored in the `Core` folder in `Include` and `Source`.
Each module must contain a `<module name>.hpp` file at the root of the module. This file includes the header files that 
can be used by the user of the module. This header file must also contain a define of the module name for namespace
declaration in the form of `#define FLASHLIGHT_<module name>_MODULE ::<module name>` in all caps. This definitions must
be used in the declaration of the namespace.

Example :
```c++
// Module main file (Example.hpp).
#pragma once
#define FLASHLIGHT_EXAMPLE_MODULE ::Example

#include "ExampleClass.hpp"
```

```c++
/// ExampleClass.hpp

#include "Example.hpp"

// Puts the class under the Flashlight::<module name> namespace, in that case Flashlight::Example
namespace Flashlight FLASHLIGHT_EXAMPLE_MODULE {

class ExampleClass {};
}
```
## File types
Header files (`.hpp`) should only contain the declaration of structures and classes. Structures are allowed to have their
constructors inside of a header file only if they initialize fields and do noting more. They must be in the `Include` folder
of the corresponding project, under the right module folder.

Inline files (`.inl`) should only contain definitions of inline functions and templates. These should be included in header
files. They must be in the same folder as header files.

Source files (`.cpp`) should only contain definitions of functions that are not inline and definitions of local functions.
They Must be in the `Source` folder of the corresponding project, under the right module folder.

## Code style
Function names are in UpperCamelCase, parameters are in lowerCamel case.  
Public fields in structs and classes are in UpperCamelCase.  
Protected fields in structs and classes are prefixed with `p_` and named in UpperCamelCase.  
Private fields in structs and classes are prefixed with `m_` and named in UpperCamelCase.  
Header files and inline files use `#pragma once` as header guard.  
Lines have a limit of 120 characters.  
Indentation is 4 spaces.  
Code inside of namespaces is not indented.  
Access modifiers in structures and classes must be on the same indent level as the class declaration.  
Functions must be documented with the following form : 
```c++
/// @brief <Brief description of the function>
///
/// @param param1 <description of the parameter>
/// @param param2 <description of the parameter>
/// ...
///
/// @returns <description of the return value>
```

Example :
```c++
/// @brief Adds two integers together.
///
/// @param a The first number.
/// @param b The second number.
///
/// @returns The sum of the two parameters.
int addInt(int a, int b) {
    return a + b;
}
```
Before writing the definition of functions in a source or inline file (or header file if the constructor of a structure 
only initialises fields), there must be documentation lines in the following form :
```c++
/// @ingroup <Module name>
/// @class or @struct <namespace>::<class/struct name>
/// @brief Description of the struct/class
```
Includes must be in the following order :
```c++
/* // Main module file
 *
 * // Header files from other modules
 *
 * // Other header files from the same module
 *
 * #include "FlashlightEngine/pch.hpp" // This is the precompiled header, containing files used a lot across the project.
 *
 * // Header files from external libraries.
 *
 * // Header files from the standard library.
 */
```
The code must use type definitions from `defines.hpp` (included in `pch.hpp`) for primitive integer, unsigned integer and
decimal types.

## Commit messages

You must follow the Conventional Commits specification to write commit messages. This convention makes it easier to
understand the changes that are made in the repository.

So, the commit message should be structured as follows:

```<type>(optional scope): <description>```

Where `type` is one of the following:

- `build`: Changes that affect the build system or external dependencies
- `ci`: Changes to the CI configuration files and scripts
- `docs`: Changes that don't modify source files
- `feat`: A new feature that can be:
    - A new feature/module to the engine.
    - A change to an existing module.
- `fix`: A bug fix that can be:
    - A bug fix in a module.
    - A typo fix in the documentation or in the code
- `perf`: A code change that improves performance
- `refactor`: A code change that neither fixes a bug nor adds a feature, often a code cleanup or a name change
- `test`: Adding missing tests or correcting existing tests

The `scope` is optional and can be anything specifying the place of the commit change. For example, `Core`, `VulkanRenderer`,
`Docs`, `Readme`, etc.

The `description` should be a short description of the change. It should be written in the imperative mood and should not 
end with a period.

## Branch naming
Branch naming isn't strict, it just needs to be a comprehensive name that describes the changes that are made in the 
branch. For example, `feat/new-module-implementation`, `fix/swapchain-crash`, etc.