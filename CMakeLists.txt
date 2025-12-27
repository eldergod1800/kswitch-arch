cmake_minimum_required(VERSION 3.16)
project(ktodo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt5 REQUIRED COMPONENTS Core Quick QuickControls2 Widgets)
find_package(KF5 REQUIRED COMPONENTS Kirigami2 I18n CoreAddons Config)

add_executable(ktodo
    src/main.cpp
    src/taskmodel.cpp
    src/taskmanager.cpp
    src/resources.qrc
)

target_link_libraries(ktodo
    Qt5::Core
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Widgets
    KF5::Kirigami2
    KF5::I18n
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::ConfigGui
)

install(TARGETS ktodo ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.ktodo.desktop DESTINATION ${KDE_INSTALL_APPDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
