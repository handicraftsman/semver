# Using semver library

1. Clone this git repository into your project.
2. Add the repository root to the include directories (using the `-I` flag in GCC or `include_directories` in CMake).
3. Build `semver.c` with your project. I prefer building it as a library (`add_library` in CMake) and linking it to required targets (`target_link_libraries` in CMake).
4. To use the library after adding it to the project, just #include `<semver.h>`.