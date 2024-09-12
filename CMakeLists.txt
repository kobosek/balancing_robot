cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

# Set the components to include the main component
set(COMPONENTS main)

project(balancing-robot)

# Specify the SPIFFS directory
spiffs_create_partition_image(storage spiffs FLASH_IN_PROJECT)