import os
from SCons.Script import ARGUMENTS

# Set an explicit IDF git description to bypass git-describe calls in build
os.environ.setdefault("IDF_GIT_COMMIT_DESC", "SmvIT-0.1.0")

# Ensure bootloader also picks it up
os.environ.setdefault("IDF_BOOT_COMMIT_DESC", "SmvIT-boot-0.1.0")

print("[extra_script] Set IDF_GIT_COMMIT_DESC=", os.environ.get("IDF_GIT_COMMIT_DESC"))
