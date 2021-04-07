# Flint
Flint is a low level, OOP based, cross-platform, high performance graphics API. You have three main APIs to choose from at compile time. After that just build against the `Flint` static library and your all set!

## How to use
There are two ways to use Flint. Either build it on your local machine (which requires Windows) or download the pre-built binaries (which is currently unavailable). <br />
Once you have the Flint binary, build your application against the static library. The include directory should be `...\Flint\Source`.

For more information, head to our [wiki.](https://github.com/DhirajWishal/Flint/wiki)

## How to build the binaries
Before we actually build, we need the following dependencies.
1. [GitBash](https://git-scm.com/downloads) or [GitHub Desktop](https://desktop.github.com/).
2. [Visual Studio 2019](https://visualstudio.microsoft.com/downloads/) with the C++ workloads.

Once everything is set, clone the repository to a local location. <br />
For GitBash
```
> git clone https://github.com/DhirajWishal/Flint Flint
> cd Flint
```
For GitHub Desktop, go to File -> Clone Repository, set the download URL as `https://github.com/DhirajWishal/Flint`, set an appropriate Local Path and Clone.

After this, run the `GenerateProjects.bat` file which will generate the required `.sln` and `.vcxproj` files. Once the files are created, open the `Flint.sln` file. 

Then in Visual Studio, select the configuration you need. You can select one of the following.
* Debug Vulkan
* Release Vulkan
* Debug DirectX 12
* Release DirectX 12
* Debug WebGPU
* Release WebGPU

Note: Flint currently only supports `Debug Vulkan` and `Release Vulkan`. Also make sure that the startup project is set to `Flint`.

After configuring, build the library file. The binaries will be placed under `Flint\Builds\Binaries\<Configuration>\` directory. <br />
Optionally you can try and build a demo application provided under the `Demos` section. 


## License
This repository is licensed under Apache-2.0.