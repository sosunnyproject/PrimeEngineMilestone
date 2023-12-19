## Installation Instructions

Important: information below might assume you have perforce access, we don't have it. I have uploaded the PrimeEngine workspace to blackboard. So use that for now by just exporting it.
You can go to blackboard course content, or this link should work if you are signed in into blackboard.

Installation steps for your laptop:
- Install visual studio (2022 community edition is fine)
When installing, choose 
* Desktop development with C++
* Game development with C++
You don't need to install any other windows SDKs

If you want to use the directx texture tool you can install the old directx sdk from jun 2010:
old DirectX SDK has some good tools that we will use.

You will be running premake to generate visual studio solutions. They will be for older visual studio but you can open them in 2022 version just fine.
**Install cygwin**  
- For cygwin, just install latest cygwin, default installation
  Run cygwin once, it will create the user folder. In cygwin user folder, in .bashrc file (in my case C:\cygwin\home\Artem\.bashrc) , append these lines to make cygwin handle files with windows line endings properly.
export SHELLOPTS
set -o igncr
- make sure to use notepad++ or something that has good handling of line endings.
- Once lines are added, restart cygwin so it reloads with the new file.

**Install Autodesk Maya**
(use autodesk and free student license)
Please use Maya up to Maya 2020! 2022 has upgraded to Python 3 and some of our scripts don't work.
Export PEWorkspace for a fairly shallow folder, like C:\projects. Don't install into a deep folder and don't have spaces in the path

**Building and running page:**
This page goes over some details on how to generate visual studio solution.
Note one of the first things you will need to do is to modify setenv.sh to specify the proper paths to Visual studio and Maya

**Here is the pdf of website page**
[generating-and-building.pdf](https://cdn-uploads.piazza.com/paste/hl3e3o1lmbqp5/5f9308d08b8b4e6d3d290dc3d0e5024aeecf1e1a433a63347ef57283fc0d7d01/generating-and-building.pdf)

I go over running Cygwin, generating solution and building executable and running the game. [video](https://drive.google.com/file/d/0B_zbSZQrDGv7Yk1uclQ4YkRTWFU/view?usp=sharing&resourcekey=0-gUkA5w4N95I0zLBxLQvSNg)
