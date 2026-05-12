# How Pepper works

Pepper works by creating a fake virtual reality device driver that SteamVR will talk to and believe that it is a legitimate VR device. This basically is a driver that will be seen as a VR headset by SteamVR but can be controlled by your computer.

---

## How SteamVR loads it

SteamVR looks into the folder "SteamVR/drivers/" for anything that looks like a VR device.  
It will find your driver package, which is called "driver.vrdrivermanifest", which tells it to load the driver.  
It will then load the DLL and call the function "HmdDriverFactory", which hands SteamVR your driver object.  
SteamVR will then start trusting you and will ask it every frame where the headset is, where the controllers are, and what buttons are being pressed.

---

HMD is the headset part, the driver will provide SteamVR with the position and the rotation.  
Controllers (left and right) are the fake hands, they will provide SteamVR with what your hands are doing.  
Frame loop is the entire emulator runtime, SteamVR calls it 90 times per second and it tells SteamVR to update the position of where you are.

---

## Controls

| Key | Action |
|-----|--------|
| Mouse | Head yaw/pitch |
| WASD | Move relative to head direction |
| Space / Shift | Up / Down |
| Q / E | Left / Right trigger |
| Z / C | Left / Right grip |
| Arrow keys | Right joystick |
| 1 / 2 | Right A/B buttons |
| 3 / 4 | Left X/Y buttons |

---

## Requirements and the install guide

Install these before anything else:
- Visual Studio 2022 (with desktop development with C++)
- Steam + SteamVR
- Git

---

## HOW TO INSTALL

Open a terminal and use:


git clone https://github.com/blakebeckcoding/Pepper.git


After doing that then use:


cd driver_fakevr


After you are done with that, use:


git clone https://github.com/ValveSoftware/openvr.git


After that open up Visual Studio and create a new DLL project named "driver_fakevr". Once it opens, remove all the default generated files Visual Studio created, then you should right click the project, add existing item, select all files inside the src folder. Do the same for driver_fakevr.def in the root folder.

Then open project properties (right-click project then properties) and make sure the dropdowns say Release and x64, then configure the following:

- C/C++ → General → Additional Include Directories: add the full path to openvr\headers  
- Linker → Input → Additional Dependencies: add the full path to openvr\lib\win64\openvr_api.lib  
- Linker → Input → Module Definition File: type driver_fakevr.def  
- General → Target Name: set to driver_fakevr  

After that click apply then ok.

---

After that then you should build (It should look like release and next to it should say "x64", if it doesnt say x64 SteamVR will not run it). To do that press Ctrl+Shift+B. 
When it finishes you should see no errors and a file called driver_fakevr.dll in your bin\Release folder.


---

Once done building you can open deploy.ps1 to install into SteamVR. If PowerShell blocks it then run:


Set-ExecutionPolicy -Scope CurrentUser RemoteSigned


If your Steam is not installed at "C:\Program Files (x86)\Steam", open deploy.ps1 in a text editor and update the path at the top before running.

---

When you are done with that then you can launch SteamVR, click the hamburger menu in the top left, go into developer, then driver status and then you should see PepperVR as an active driver.

---

This is the full README of Pepper, if you have any questions, recommendations, or issues at all please ask me. Have fun using my project.
