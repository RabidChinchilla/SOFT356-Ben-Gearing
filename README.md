# SOFT356-Ben-Gearing

SOFT356 Markdown Report

Version: Visual studio 2019

To work the program you make sure there are no errors in the files caused by missing packages, if there is an error uninstall and reinstall the glm package by going into tools --> NuGet Package Manager --> Manage NuGet packages for solution and browse for glm, then in the codes current state you should run the solution by clicking the green run arrow at the top of the program or hitting F5 then you should see the model loader open and display a cube with the Minecraft slime texture.

All the header files are not to be edited or removed since they are called in the main source.cpp file and LoadShaders.cpp file. The source.cpp file is the main file where the file parsing and display function is located, the LoadShaders.cpp file is called in this file as well since the shaders applied in the source.cpp file as well.

This is a very basic model loader that uses a hardcoded file path, if you wish to change the file change the path variable at the top of the source.cpp, but the loader will not be able to load multiple objects or textures
