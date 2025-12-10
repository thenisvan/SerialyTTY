#!/usr/bin/env python3
"""
Pre-build script to fix ESP-IDF Git reference issues when building with PlatformIO.
CMake expects Git metadata files that don't exist in PlatformIO's packaged ESP-IDF.
"""
Import("env")
import os

def create_git_refs(source, target, env):
    """Create dummy Git reference files to satisfy ESP-IDF CMake scripts"""
    build_dir = env.subst("$BUILD_DIR")
    
    # Main project git-data
    main_git_dir = os.path.join(build_dir, "CMakeFiles", "git-data")
    os.makedirs(main_git_dir, exist_ok=True)
    
    head_ref = os.path.join(main_git_dir, "head-ref")
    with open(head_ref, 'w') as f:
        f.write("ref: refs/heads/main\n")
    
    # Bootloader git-data
    bootloader_git_dir = os.path.join(build_dir, "bootloader", "CMakeFiles", "git-data")
    os.makedirs(bootloader_git_dir, exist_ok=True)
    
    bootloader_head_ref = os.path.join(bootloader_git_dir, "head-ref")
    with open(bootloader_head_ref, 'w') as f:
        f.write("ref: refs/heads/main\n")
    
    print("✓ Git reference files created for CMake")

# Create git refs immediately before CMake runs
create_git_refs(None, None, env)
