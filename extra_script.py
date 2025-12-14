import os
import subprocess
from SCons.Script import ARGUMENTS
from pathlib import Path

# Set an explicit IDF git description to bypass git-describe calls in build
os.environ.setdefault("IDF_GIT_COMMIT_DESC", "SmvIT-0.1.0")

# Ensure bootloader also picks it up
os.environ.setdefault("IDF_BOOT_COMMIT_DESC", "SmvIT-boot-0.1.0")

print("[extra_script] Set IDF_GIT_COMMIT_DESC=", os.environ.get("IDF_GIT_COMMIT_DESC"))

# Fix CMake git-data files before build
def fix_git_refs():
    # In SCons context, use current working directory
    project_dir = Path(os.getcwd())
    
    # Get current Git HEAD
    try:
        result = subprocess.run(
            ["git", "rev-parse", "--symbolic-full-name", "HEAD"],
            cwd=project_dir,
            capture_output=True,
            text=True,
            check=True
        )
        head_ref = result.stdout.strip()
        
        # Create git-data directories and head-ref files
        git_data_dirs = [
            project_dir / ".pio" / "build" / "esp32c6" / "CMakeFiles" / "git-data",
            project_dir / ".pio" / "build" / "esp32c6" / "bootloader" / "CMakeFiles" / "git-data"
        ]
        
        for git_dir in git_data_dirs:
            git_dir.mkdir(parents=True, exist_ok=True)
            head_ref_file = git_dir / "head-ref"
            head_ref_file.write_text(f"{head_ref}\n")
            print(f"[extra_script] Created {head_ref_file}")
            
    except Exception as e:
        print(f"[extra_script] Warning: Could not fix git refs: {e}")

fix_git_refs()
