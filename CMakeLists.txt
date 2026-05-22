cmake_minimum_required(VERSION 3.16)
project(RemoteSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Network 
    Multimedia 
    MultimediaWidgets
)

# Enable automatic MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Sources
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/models/NetworkConfigModel.cpp
    src/models/DeviceConfigModel.cpp
    src/models/ConnectionStatusModel.cpp
    src/models/WifiInfoModel.cpp
    src/views/MainControlView.cpp
    src/views/WifiInfoView.cpp
    src/views/IpConfigView.cpp
    src/views/CameraWidget.cpp
    src/controllers/MainController.cpp
    src/controllers/CameraController.cpp
    src/controllers/NetworkController.cpp
    src/controllers/DeviceController.cpp
    src/network/TcpServer.cpp
    src/network/TcpClient.cpp
    src/network/NetworkUtils.cpp
    src/camera/CameraManager.cpp
    src/camera/MjpegStreamHandler.cpp
)

# Headers
set(HEADERS
    src/MainWindow.h
    src/models/NetworkConfigModel.h
    src/models/DeviceConfigModel.h
    src/models/ConnectionStatusModel.h
    src/models/WifiInfoModel.h
    src/views/MainControlView.h
    src/views/WifiInfoView.h
    src/views/IpConfigView.h
    src/views/CameraWidget.h
    src/controllers/MainController.h
    src/controllers/CameraController.h
    src/controllers/NetworkController.h
    src/controllers/DeviceController.h
    src/network/TcpServer.h
    src/network/TcpClient.h
    src/network/NetworkUtils.h
    src/camera/CameraManager.h
    src/camera/MjpegStreamHandler.h
)

# UI Files
set(UI_FILES
    ui/MainControlView.ui
    ui/WifiInfoView.ui
    ui/IpConfigView.ui
)

# Create executable
# CMAKE_AUTOUIC will automatically process UI_FILES and generate ui_XXX.h headers
add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Tell UIC where to find .ui files when processing #include "ui_XXX.h"
# Since .ui files are in ui/ but .h files are in src/views/, we need to specify the search path
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui"
)

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

# Windows specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

