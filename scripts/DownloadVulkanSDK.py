import Vulkan
import os
import subprocess
import CheckPython

# Make sure everything we need is installed
CheckPython.ValidatePackages()

# Change from Scripts directory to root
os.chdir('../')

if (not Vulkan.CheckVulkanSDK()):
    print("Vulkan SDK not installed.")

if (not Vulkan.CheckVulkanSDKDebugLibs()):
    print("Vulkan SDK debug libs not found.")
