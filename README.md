# STM32F405RGTx Peripheral Test Workspace

## Overview
This repository provides an entire STM32Cube Workspace dedicated to ensuring peripherals on the board were soldered correctly and also provides base code for interfacing with devices and board peripherals as they are added. Since these projects are mainly used to ececk hardware, they will just be using the hardware abstraction layer (HAL) where you have a GUI of the MCU pins and can assign them roles easily without much code. The idea is to have individual projects to test aspects of the flight controller so that the base code can stay as clear as possible, so do not combine too many features in one project. Please adhere to the [git branching strategy](https://github.com/UARK-Quadcopter-Flight-Controller/KnowledgeBase/blob/main/Git/Branching%20Strategy.md) for contributions.

## Contents
* [Description of Existing Projects](#description-of-existing-projects)
* [How to Open and Import Projects in Workspace](#how-to-open-and-import-projects-in-workspace)
* [How to Create a New Project for Our Target MCU](#how-to-create-a-new-project-for-our-target-mcu)
* [Debugging Using STLINK](#debugging-using-stlink)

### Description of Existing Projects
- [**LED-Blink**](/LED-Blink/README.md): Tests the onboard LED and toggles it synchronously every 500ms.
- [**LED-Blink-OSC**](/LED-Blink-OSC/README.md): Tests the onboard LED and the external onboard crystal oscillator.
- [**USB-Serial-Print**](/USB-Serial-Print/README.md): Tests the serial print middleware capability of the micro-USB port.


### How to Open and Import Projects in Workspace
open STM32Cube and change your workspace location to the repository root "STM32F4_PeripheralTest" wherever you may have put it. This will not populate STM32Cube with the projects and must be done manually. Go to **File** > **Import** and choose the option **Projects from Folder or Archive** found under **General**. Navigate to the git repository and then ensure all projects are checked and click finish. From there, all the projects are now imported and should appear on the left sidebar. Clicking the project to highlight it makes it the "current" project and then clicking the debug/build/run buttons will do those respective operations on the current project. The source code for any project can be found in `<ProjectName>/Core/Src/` and the `main.c` file within is auto-generated if the `<ProjectName>.ioc` is ever modified. Because of this, it is recommended that you build out your feature in a separate file and include it in main.c so all of your work isn't wasted if main.c is ever overwritten.

### How to Create a New Project for Our Target MCU 
#### Git
If you are creating a new project, that means you are probably implementing a feature. Assuming you are starting from `git clone ...`, create your feature branch from development. As a tip, using VSCode as a GUI for git might be a faster and more user friendly way to interface with git commands rather than using a terminal. This can be done by typing in `code STM32F4_PeripheralTest` in terminal after cloning. From there, in the bottom left corner of the VSCode window will show the main branch. Clicking on this and choosing "create new branch from..." will let you quickly create a feature branch from development. 

#### Getting STM32Cube to Recognize Existing Projects
After git is configured for your new project, refer to the section above to learn how to [open and import projects into the workspace](#how-to-open-and-import-projects-in-workspace).

#### Create a New Project
In the top left of the IDE (or using File > New), click on the new project drop down button and a project create menu will appear. Choose **New STM32 Project**. For part number, type in `STM32F405RG` and click the only option that shows up in the MCU list. Click **next** at the bottom. **Name the project with dashes for spaces**. So if your project is "LED Blink", name it "LED-Blink". Leave the rest of the settings alone. Click **Finish**. After the project finishes, you will be greeted with with project's .ioc file, which is where you can configure the I/O of the MCU. A great tutorial to reference to learn how to use this can be found [here](https://youtu.be/x_5rYfAyqq0?t=524). After you have enabled the pins you want for this project, click save and it will generate code corresponding to your pin assignments.

#### Documenting Your Process
At some point in your project, you will need to add a README to your project. This should be right inside the root of your project directory. Use this readme as a place to briefly explain what you did, what MCU pins have been enabled from the IOC configuration file, and the purpose for the project. Then, go back to this README and update the [Description of Existing Projects](#description-of-existing-projects) section to include your README.md file.


### Debugging Using STLINK
At some point, you'll want to test your feature or a production board out to ensure either the featuer or the flight controller PCB is working as expected. For this, you will need a board with the 2x4 WIFI headers soldered on, as the WIFI pins are also used for the STM Debugging interface, as well as the ST-Link debugging module with the 4x female-female jumper wires. The instructions from here will break into 2 parts as the process differs for the development board vs the production flight controller boards. **Note: You will need to power the board with a USB cable AND the debugger. The debug interface is not meant to solely supply power to the MCU, it is just to give the debugger a reference voltage.** After following one of the sets of instructions below, hitting the debug button on any active project will bring up a menu for the first time where you can configure what debugging interface you want. Click the **Debugger** tab and keep everything as it is, but click the check box next to **ST-LINK S/N** and scan (assuming the ST-Link is connected). After it finds it, or requests to update the ST-Link, click apply and OK.

#### Flight Controller Board
With the 2x4 WIFI header installed, notice that there are labels to the side of the J8 header that point out the power pins 3.3V and GND. If you are looking at the board with the USB connection on top, the lower right portion of the board will look like the following:

```
           WIFI MODULE
        -----------------
 3V3 -> | 1 | 2 | 3 | 4 |
        -----------------
        | 5 | 6 | 7 | 8 | <- GND
      J8-----------------
```

Using this as a pinout, the relevant pins for debugging are:
- Pin 1 is 3.3V
- Pin 2 is SWDIO
- Pin 6 is SWCLK
- Pin 8 is GND

*No other connections are used on the WIFI header for debugging. Please get these right and don't fry our production boards.*

#### Development Board
[Documentation needed...]


