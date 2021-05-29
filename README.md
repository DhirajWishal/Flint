# Flint
Flint is a low level, OOP based, cross-platform, high performance graphics library based on a simple API.

## How to use
Flint can be used as either a static library or as a shared library. If your building the project manually, make sure to select the correct platform and configuration. The binaries are built to the `/Builds/Binaries/[Configuration]` directory. The include directory should be set to `/Include`.

For in-depth API documentation, head onto our repository [wiki](https://github.com/DhirajWishal/Flint/wiki).

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

Then in Visual Studio, select the platform and configuration you need. You can select one of the following configurations,
* Debug Static
* Debug Shared
* Release Static
* Release Shared

Where Static will build the library as a `.lib/.a` file, Shared will build the library as a `.dll/.so` file. Optionally you can try and build a demo application provided under the `Demos` section. 


## License
This repository is licensed under Apache-2.0.