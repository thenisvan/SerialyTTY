#!/usr/bin/env python3
"""
Pre-build script to fix ESP-IDF Git reference issues when building with PlatformIO.
CMake expects Git metadata files that don't exist in PlatformIO's packaged ESP-IDF.
"""
Import("env")
import os

# Run immediately on import (before any build stages)
platform_packages_dir = env.subst("$PLATFORMIO_CORE_DIR/packages")
build_dir = env.subst("$BUILD_DIR")

# Fix OpenThread submodule Git reference
openthread_submodule_git = os.path.join(
    platform_packages_dir,
    "framework-espidf@3.50301.0",
    ".git",
    "modules",
    "components",
    "openthread",
    "openthread"
)

if not os.path.exists(openthread_submodule_git):
    os.makedirs(openthread_submodule_git, exist_ok=True)
    
    # Create minimal Git repository structure
    with open(os.path.join(openthread_submodule_git, "config"), 'w') as f:
        f.write("[core]\n\trepositoryformatversion = 0\n")
    
    with open(os.path.join(openthread_submodule_git, "HEAD"), 'w') as f:
        f.write("ref: refs/heads/main\n")
    
    os.makedirs(os.path.join(openthread_submodule_git, "refs", "heads"), exist_ok=True)
    with open(os.path.join(openthread_submodule_git, "refs", "heads", "main"), 'w') as f:
        f.write("0000000000000000000000000000000000000000\n")
    
    print("✓ Fixed OpenThread Git submodule references")

def create_git_refs_callback(source, target, env):
    """Create dummy Git reference files to satisfy ESP-IDF CMake scripts"""
    # Main project git-data
    main_git_dir = os.path.join(build_dir, "CMakeFiles", "git-data")
    os.makedirs(main_git_dir, exist_ok=True)
    
    with open(os.path.join(main_git_dir, "head-ref"), 'w') as f:
        f.write("ref: refs/heads/main\n")
    
    # Bootloader git-data
    bootloader_git_dir = os.path.join(build_dir, "bootloader", "CMakeFiles", "git-data")
    os.makedirs(bootloader_git_dir, exist_ok=True)
    
    with open(os.path.join(bootloader_git_dir, "head-ref"), 'w') as f:
        f.write("ref: refs/heads/main\n")
    
    print("✓ Git reference files created for CMake")

# Create git refs before any build action
env.Execute(lambda *args: create_git_refs_callback(None, None, env))
