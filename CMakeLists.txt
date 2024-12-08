cmake_minimum_required(VERSION 3.5)

project(ServerWithIcon VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network MultimediaWidgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network MultimediaWidgets Multimedia)

set(PROJECT_SOURCES
        main.cpp



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ServerWithIcon
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        mainwindow.cpp mainwindow.h mainwindow.ui serverthread.cpp serverthread.h
        res.qrc
        dragwidget.cpp dragwidget.h
        downloaditemwidget.h downloaditemwidget.cpp downloaditemwidget.ui
        progressbar.cpp progressbar.h
        musiclistwidget.h musiclistwidget.cpp musiclistwidget.ui
        dragslider.h dragslider.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ServerWithIcon APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ServerWithIcon SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ServerWithIcon
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ServerWithIcon PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets Qt${QT_VERSION_MAJOR}::Multimedia)

if(WIN32)
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/icon.rc")
    target_sources(ServerWithIcon PRIVATE ${APP_ICON_RESOURCE})
elseif(UNIX AND NOT APPLE)
    install(FILES img/cloud.png DESTINATION share/icons/hicolor/256x256/apps/)
    install(FILES res/myapp.desktop DESTINATION share/applications/)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ServerWithIcon)
endif()
set_target_properties(ServerWithIcon PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ServerWithIcon
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ServerWithIcon)
endif()
