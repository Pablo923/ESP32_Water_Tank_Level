# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/pablo/esp/esp-idf/components/bootloader/subproject"
  "/Users/pablo/Documents/Repositorios/Pablo/ESP32_Water_Tank_Level/build/bootloader"
  "/Users/pablo/Documents/Repositorios/Pablo/ESP32_Water_Tank_Level/build/bootloader-prefix"
  "/Users/pablo/Documents/Repositorios/Pablo/ESP32_Water_Tank_Level/build/bootloader-prefix/tmp"
  "/Users/pablo/Documents/Repositorios/Pablo/ESP32_Water_Tank_Level/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/pablo/Documents/Repositorios/Pablo/ESP32_Water_Tank_Level/build/bootloader-prefix/src"
  "/Users/pablo/Documents/Repositorios/Pablo/ESP32_Water_Tank_Level/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/pablo/Documents/Repositorios/Pablo/ESP32_Water_Tank_Level/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/pablo/Documents/Repositorios/Pablo/ESP32_Water_Tank_Level/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
