import os
import subprocess
import sys
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

VULKAN_SDK = os.environ.get('VULKAN_SDK')
VULKAN_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/1.2.170.0/windows/VulkanSDK-1.2.170.0-Installer.exe'
HAZEL_VULKAN_VERSION = '1.2.170.0'
VULKAN_SDK_LOCAL_PATH = 'Dependencies/VulkanSDK'
VULKAN_SDK_EXE_PATH = f'{VULKAN_SDK_LOCAL_PATH}/VulkanSDK.exe'


def InstallVulkanSDK():
    Path(VULKAN_SDK_LOCAL_PATH).mkdir(parents=True, exist_ok=True)
    print('Downloading {} to {}'.format(
        VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH))
    Utils.DownloadFile(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH)
    print("Done!")
    print("Running Vulkan SDK installer...")
    os.startfile(os.path.abspath(VULKAN_SDK_EXE_PATH))
    print("Re-run this script after installation")


def InstallVulkanPrompt():
    print("Would you like to install the Vulkan SDK?")
    install = Utils.YesOrNo()
    if (install):
        InstallVulkanSDK()
        quit()


def CheckVulkanSDK():
    if (VULKAN_SDK is None):
        print("You don't have the Vulkan SDK installed!")
        InstallVulkanPrompt()
        return False
    elif (HAZEL_VULKAN_VERSION not in VULKAN_SDK):
        print(f"Located Vulkan SDK at {VULKAN_SDK}")
        print(
            f"You don't have the correct Vulkan SDK version! (Hazel requires {HAZEL_VULKAN_VERSION})")
        InstallVulkanPrompt()
        return False

    print(f"Correct Vulkan SDK located at {VULKAN_SDK}")
    return True


VULKAN_SDK_DEBUG_LIBS_URL = 'https://sdk.lunarg.com/sdk/download/1.2.170.0/windows/VulkanSDK-1.2.170.0-DebugLibs.zip'
VULKAN_SDK_DEBUG_LIBS_ZIP = f'{VULKAN_SDK_LOCAL_PATH}/VulkanDebugLibs.zip'


def CheckVulkanSDKDebugLibs():
    shadercdLib = Path(f"{VULKAN_SDK_LOCAL_PATH}/Lib/shaderc_sharedd.lib")
    if (not shadercdLib.exists()):
        print(f"No Vulkan SDK debug libs found. (Checked {shadercdLib})")
        print(f"{Path(VULKAN_SDK_DEBUG_LIBS_ZIP)}")
        if (not Path(VULKAN_SDK_DEBUG_LIBS_ZIP).exists()):
            if (not Path(VULKAN_SDK_LOCAL_PATH).exists()):
                Path(VULKAN_SDK_LOCAL_PATH).mkdir(parents=True, exist_ok=True)
            Utils.DownloadFile(VULKAN_SDK_DEBUG_LIBS_URL,
                               VULKAN_SDK_DEBUG_LIBS_ZIP)
        print("Extracting...")
        with ZipFile(VULKAN_SDK_DEBUG_LIBS_ZIP, 'r') as zip:
            zip.extractall(VULKAN_SDK_LOCAL_PATH)

        os.remove(VULKAN_SDK_DEBUG_LIBS_ZIP)

    return True
