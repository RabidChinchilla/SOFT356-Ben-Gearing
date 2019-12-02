# SOFT356-Ben-Gearing

## SOFT356 Markdown Report

## Version: Visual studio 2019

### Packages used

* Nupengl
* GLM

### File Contents

The project file consists of 4 main files; 
* source.cpp
* LoadShaders.cpp
* LoadShaders.h
* stb_image.h

The source.cpp file is the core file that contains a majority of the code required to load the models and textures, it also creates the display window and brings up the console for the user.

LoadShaders.cpp condenses all the shading code so it an be easily called by the source.cpp file.

Loadshaders.h and stb_image.h are header files that are called early in the source.cpp file and contain complex code needed by the main file but would take up too much space in that file when they could easily be put into headers and called from outside the main file.

### Before Runtime

Firstly make sure that the Creeper file inside Media/Creeper-obj/ is a .obj file instead of a .fbx file, you can check this in the project files side bar inside the visual studio IDE. If the file is not a .obj file redownload the test objects file from the dle and place those objects into the Media file.

Before running the program make sure that all the packages are installed correctly and no errors are being shown in the code.

If errors are being shown under the GLM imports go into tools --> NuGet Package Manager --> Manage NuGet packages and find the glm package, uninstall this package and then install it again by searching for it under the browse tab.

### Running the program

To run the program either click the green arrow at the top of the IDE that says Local Windows Debugger or by hitting F5 on the keyboard.

### During Runtime

During the run time of the program all you have to do is enter the name of the file you want to open into the console, this project cannot load multiple objects in its current state and can only load the Creeper file and textures.

When you get the prompt **"Please enter the Object you want to open:"** all you have to do is type in **Creeper** and the program will do the rest to load the model in the display window. There is no need to add .obj to the end of your input because the code handles it for you. The code is very simple and will show you a rotating version of the model with the texture applied, as of now you cannot move the camera or model.
