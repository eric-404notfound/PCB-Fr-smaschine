# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/dev/rp2040/pico-sdk/tools/pioasm"
  "/home/dev/rp2040/PCB-Fr-smaschine/build/pioasm"
  "/home/dev/rp2040/PCB-Fr-smaschine/build/pioasm-install"
  "/home/dev/rp2040/PCB-Fr-smaschine/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/home/dev/rp2040/PCB-Fr-smaschine/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
  "/home/dev/rp2040/PCB-Fr-smaschine/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/home/dev/rp2040/PCB-Fr-smaschine/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/dev/rp2040/PCB-Fr-smaschine/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/dev/rp2040/PCB-Fr-smaschine/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
