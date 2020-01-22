cmake_minimum_required(VERSION 3.1.0)

project(usb-streamer C)

add_executable(usb-streamer
        main.c)
target_link_libraries(usb-streamer
        usb-1.0)