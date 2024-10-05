cmake_minimum_required(VERSION 3.5)

# Include the ESP-IDF components
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Set additional component directories
set(EXTRA_COMPONENT_DIRS
    $ENV{IDF_PATH}/examples/common_components/protocol_examples_common
    ${CMAKE_CURRENT_SOURCE_DIR}/main/HardwareAbstractionLayer)

# Define the project
project(balancing-robot)

# Set the components to include
set(COMPONENTS main HardwareAbstractionLayer)

# Specify the SPIFFS directory
spiffs_create_partition_image(storage spiffs FLASH_IN_PROJECT)
