# SPDX-License-Identifier: Apache-2.0

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(central)

target_sources(app PRIVATE
  src/main.c
  src/temperature_display.c
  src/probe_connection.c
)

zephyr_library_include_directories(include)
