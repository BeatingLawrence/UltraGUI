# UltraGUI
This is the UltraGUI graphic library. Based on Qt

# Development Environment Build
1-Create a new deploy step of type "make" and add "install" as argument
2-Create a new build configuration called "TestAPP" (template=Release) and add qmake argument: "CONFIG+=testappconfiguration"
3-In the Debug configuration add qmake argument: "CONFIG+=debugconfiguration"
4-In the Release configuration add qmake argument: "CONFIG+=releaseconfiguration"
