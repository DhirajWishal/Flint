"""
Copyright 2021 Dhiraj Wishal
SPDX-License-Identifier: Apache-2.0

Download assets from Google drive.
"""

import zipfile
import sys
import os

sys.path.insert(0, "..")
os.system("cd ../Assets && mkdir Packages")

from ThirdParty.gdown import gdown

assets = {"Skybox": "https://drive.google.com/uc?id=1WAPhGzEfjb5vAnpLR5sAsmZ7UXxWB8Nl",
          "Tree001": "https://drive.google.com/uc?id=1Ea03yXvJJVbJo6D0_RXh5WzNFlRu0EkE",
          "VikingRoom": "https://drive.google.com/uc?id=1IJZooaN5A2EwdBXLl4-L-HmFBHxk6wdC"}

for key in assets:
    gdown.download(assets[key], f"../Assets/Packages/{key}.zip", quiet=False)

    with zipfile.ZipFile(f"../Assets/Packages/{key}.zip", 'r') as file:
        file.extractall(f"../Assets/Packages/{key}")

    os.remove(f"../Assets/Packages/{key}.zip")
