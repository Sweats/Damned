## What is this?

 ### This is the source code for the game [Damned 2014](https://store.steampowered.com/app/251170/Damned/) that's on Steam.


### :warning: :warning: 
# Important. Read the software license that comes with the source code before following any of these steps. 

### :warning: :warning: 

# Damned 1 — Build Environment Setup Guide

---

## What You'll Need

### Visual Studio 2010 Express

- Includes the VS100 build tools required to build Damned

### Python 2.7.6

- If you don't have this installed you'll get `"Python.h not found"` when trying to build the game.

- Download: [Python Release Python 2.7.6 | Python.org](https://www.python.org/downloads/release/python-276/)

- You can also grab the installer from the Damned directory if you have the game installed on Steam.


## Setup Steps

### 1. Install Visual Studio 2010 Express

### 2. Install Python 2.7.6

### 3. Set Up the Source Code

> :warning: **Very important!** Take the source code and extract it to the root of your C drive.

The path of the source code should be:

```

C:\terror_workspace

```

Navigate to where you extracted the source code. You can either run the `build_solution.bat` script or open up a terminal in the directory and type:

```

./premake4.exe vs100

```

**Why extract to the root of the C drive?**

After running premake4, the generated Visual Studio solution will use a hardcoded path that starts at the root of your C drive —

If you do not want to put the project at the root of your C drive, you have two options:

1. Go into Visual Studio and change all the include paths to where you put the `terror_workspace`. There's quite a few.

2.  Modify the premake script to use whatever path you want or use a relative path. This script will be modified in the very near future.

## Setting up the dependencies

1. Download the zip from [here](https://drive.proton.me/urls/JZXPG2CT1W#itum21kZEC7t)

2. Extract it to the root of your workspace. If you put it on the root of your C drive, it will look like this:

```

C:\terror_workspace\9gears_dependencies_2014

```

You are now ready to build the game.


## Building the Game

1. Open Visual Studio and open a new project. The project you want to open is **`Engine.sln`**. 

2. Set the build configuration to **Release** if it's not already.

3. It should build successfully.



## You are now ready to make modifications to the game. Have fun!


