cmake_minimum_required(VERSION 3.5)

project(CloudDisk VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network MultimediaWidgets Multimedia Core5Compat)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network MultimediaWidgets Multimedia Core5Compat)

set(PROJECT_SOURCES
        main.cpp



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CloudDisk
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        mainwindow.cpp mainwindow.h mainwindow.ui serverthread.cpp serverthread.h

        dragwidget.cpp dragwidget.h

        progressbar.cpp progressbar.h
        musiclistwidget.h musiclistwidget.cpp musiclistwidget.ui
        dragslider.h dragslider.cpp
        res.qrc
        backlabel.h backlabel.cpp
        filewidget.h filewidget.cpp filewidget.ui
        logintitle.cpp logintitle.h logintitle.ui
        logindialog.cpp logindialog.h logindialog.ui
        iconlineedit.cpp iconlineedit.h
        common.cpp common.h
        des.cpp des.h
        logger.cpp logger.h logininfoinstance.cpp logininfoinstance.h
        fileinfo.h
        filetransferprogress.h filetransferprogress.cpp filetransferprogress.ui
        downloadtask.h downloadtask.cpp
        downloadlayout.h downloadlayout.cpp
        uploadlayout.h uploadlayout.cpp
        uploadtask.h uploadtask.cpp
        networkdata.h networkdata.cpp


    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ServerWithIcon APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(CloudDisk SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CloudDisk
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(CloudDisk PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::Core5Compat)

if(WIN32)
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/icon.rc")
    target_sources(CloudDisk PRIVATE ${APP_ICON_RESOURCE})
elseif(UNIX AND NOT APPLE)
    install(FILES img/cloud.png DESTINATION share/icons/hicolor/256x256/apps/)
    install(FILES res/myapp.desktop DESTINATION share/applications/)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CloudDisk)
endif()
set_target_properties(CloudDisk PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CloudDisk
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CloudDisk)
endif()
