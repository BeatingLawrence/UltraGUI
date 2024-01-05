#------------------------------------------------VERSION CONFIGURATION

VER_MAJ = 0
VER_MIN = 9
VER_PAT = 1
VER_SUF = "-beta"

#--------------------------------------------------------------------- TODO CONVERT TO CMAKE

QT += widgets
CONFIG += c++11
DISTFILES += ./cpp.astylerc

VER_STR = UltraGUI-$${VER_MAJ}.$${VER_MIN}.$${VER_PAT}$${VER_SUF}
DEFINES += VERSION_MAJ=\"\\\"$${VER_MAJ}\\\"\" \
           VERSION_MIN=\"\\\"$${VER_MIN}\\\"\" \
           VERSION_PAT=\"\\\"$${VER_PAT}\\\"\" \
           VERSION_SUF=\"\\\"$${VER_SUF}\\\"\" \
           VERSION_STR=\"\\\"$${VER_STR}\\\"\"

SOURCES += \
    components/ultragui.cpp \
    components/ultralinesgraph.cpp \
    components/ultraloadingbar.cpp \
    components/ultraloadingcircle.cpp \
    components/ultraloadingdots.cpp \
    components/ultraloadingsquares.cpp \
    components/ultralogs.cpp \
    components/ultrascroll.cpp \
    components/ultraselector.cpp \
    components/ultraslider.cpp \
    components/ultratoggle.cpp \
    components/ultrabutton.cpp \
    components/ultraled.cpp

HEADERS += \
    components/UltraGUI_global.h \
    components/ultralinesgraph.h \
    components/ultraloadingbar.h \
    components/ultraloadingcircle.h \
    components/ultraloadingdots.h \
    components/ultraloadingsquares.h \
    components/ultralogs.h \
    components/ultrascroll.h \
    components/ultraselector.h \
    components/ultraslider.h \
    components/ultratypes.h \
    components/ultratoggle.h \
    components/ultrabutton.h \
    components/ultraled.h \
    components/ultragui.h

DISTFILES += \
    .gitignore

testappconfiguration {
    TEMPLATE = app
    TARGET = TestAPP-$${VER_STR}
    QT += core gui
    DEFINES += ULTRAGUI_TESTINGAPP

    FORMS += \
        mainwindow.ui

    RESOURCES += \
        resources.qrc

    SOURCES += \
        main.cpp \
        mainwindow.cpp \

    HEADERS += \
        mainwindow.h \
}

releaseconfiguration {
    TEMPLATE = lib
    TARGET = $${VER_STR}
    DEFINES += ULTRAGUI_LIBRARY

    CONFIG += dll skip_target_version_ext
    target.path = ../Dist/UltraGUI/lib
    includes.path = ../Dist/UltraGUI/include/UltraGUI
    includes.files += ./components/*.h
    INSTALLS += target
    INSTALLS += includes
}

debugconfiguration {
    TEMPLATE = lib
    TARGET = $${VER_STR}-d
    DEFINES += ULTRAGUI_LIBRARY

    CONFIG += dll skip_target_version_ext
    target.path = ../Dist/UltraGUI/lib
    includes.path = ../Dist/UltraGUI/include/UltraGUI
    includes.files += ./components/*.h
    INSTALLS += target
    INSTALLS += includes
}
