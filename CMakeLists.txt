cmake_minimum_required(VERSION 3.17)
project(webpext CXX)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  set(CMAKE_STATIC_LINKER_FLAGS
      "${CMAKE_MODULE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
  set(CMAKE_MODULE_LINKER_FLAGS
      "${CMAKE_MODULE_LINKER_FLAGS} /DEF:${webpext_SOURCE_DIR}/src/dllmain.def /NODEFAULTLIB:LIBCMT")
endif()

add_subdirectory(__lib__/libwebp-1.1.0-windows-x64)

set(TARGET_NAME webpext64)

set(SOURCE_FILES 
 "src/ClassFactory.cpp"
 "src/dllmain.cpp"
 "src/ThumbnailProvider.cpp"
 "src/WebpReader.cpp")

add_library(${TARGET_NAME} MODULE ${SOURCE_FILES})
target_link_libraries(${TARGET_NAME} PRIVATE libwebp shlwapi)

include(GNUInstallDirs)
install(TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
