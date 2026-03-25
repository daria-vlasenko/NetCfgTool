cmake_minimum_required(VERSION 3.16)

project(NetCfgTool
    VERSION 1.0.0
    LANGUAGES CXX
)

# -------------------------------
# Общие настройки компилятора
# -------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Включаем автоматическую обработку Qt-файлов
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# -------------------------------
# Поиск Qt6
# -------------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets Test)

# -------------------------------
# Исходники приложения
# -------------------------------
set(APP_SOURCES
    src/main.cpp

    src/mainwindow.cpp
    src/mainwindow.h

    src/network_manager.cpp
    src/network_manager.h

    src/nmcli_service.cpp
    src/nmcli_service.h

    src/profiles.cpp
    src/profiles.h

    src/validator.cpp
    src/validator.h

    src/logger.cpp
    src/logger.h

    src/core/paths.cpp
    src/core/paths.h

    src/core/logger.cpp
    src/core/logger.h
)

# -------------------------------
# Цель приложения
# -------------------------------
add_executable(NetCfgTool
    ${APP_SOURCES}
)

target_include_directories(NetCfgTool
    PRIVATE
        src
        src/core
)

target_link_libraries(NetCfgTool
    PRIVATE
        Qt6::Widgets
)

# Для Linux: положим бинарник в bin/
set_target_properties(NetCfgTool PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# -------------------------------
# Тесты (QtTest)
# -------------------------------
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    add_executable(NetCfgToolTests
    tests/test_validator.cpp
    src/validator.cpp
    src/validator.h
)


    target_include_directories(NetCfgToolTests
        PRIVATE
            src
            src/core
    )

    target_link_libraries(NetCfgToolTests
        PRIVATE
            Qt6::Test
            Qt6::Widgets
    )

    set_target_properties(NetCfgToolTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()
