cmake_minimum_required(VERSION 3.16.0)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
cmake_minimum_required(VERSION 3.16)

# Bypass git-describe calls in PlatformIO-packaged ESP-IDF by setting static
# commit descriptions before including the ESP-IDF build system.
# This avoids failures when the framework directory isn't a real git repo.
set(IDF_GIT_COMMIT_DESC "SmvIT-static-desc" CACHE STRING "Static IDF commit desc")
set(IDF_BOOT_COMMIT_DESC "SmvIT-static-boot-desc" CACHE STRING "Static boot commit desc")
set(ENV{IDF_GIT_COMMIT_DESC} "SmvIT-static-desc")
set(ENV{IDF_BOOT_COMMIT_DESC} "SmvIT-static-boot-desc")

project(SmvIT VERSION 0.1.0)
