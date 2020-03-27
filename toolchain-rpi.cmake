set (CMAKE_SYSTEM_NAME Linux)
set (CMAKE_SYSTEM_VERSION 1)

set (CMAKE_CROSSCOMPILING TRUE)

#if ("$ENV{TC_LOC}" STREQUAL "")
#  message (FATAL_ERROR
#    "Define TC_LOC enviroment variable to point to your toolchain directory \
#    containing cross compiler and sysroot
#    e.g. TC_LOC=$HOME/local/x-tools-smartg
#    ")
#else()
#  set (TOOLCHAIN_PATH "$ENV{TC_LOC}")
#endif()

set (TOOLCHAIN_PATH "/opt/x-tools-smartg")
set (TOOLCHAIN_HOST "arm-linux-gnueabihf")

# specify cross compiler
set (CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/bin/arm-rpi-linux-gnueabihf-gcc")
set (CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/bin/arm-rpi-linux-gnueabihf-g++")

# target enviroment
set (SYSROOT_PATH "${TOOLCHAIN_PATH}/arm-rpi-linux-gnueabihf/sysroot")
set (CMAKE_FIND_ROOT_PATH "${SYSROOT_PATH}")
set (CMAKE_SYSROOT ${CMAKE_FIND_ROOT_PATH})

set (BOOST_INCLUDEDIR "${SYSROOT_PATH}/usr/local/include")
set (BOOST_LIBRARYDIR "${SYSROOT_PATH}/usr/local/lib")

set(LIB_DIRS
	"/opt/cross-pi-gcc/arm-linux-gnueabihf/lib"
	"/opt/cross-pi-gcc/lib"
	"${SYSROOT_PATH}/opt/vc/lib"
	"${SYSROOT_PATH}/lib/${TOOLCHAIN_HOST}"
	"${SYSROOT_PATH}/usr/local/lib"
	"${SYSROOT_PATH}/usr/lib/${TOOLCHAIN_HOST}"
	"${SYSROOT_PATH}/usr/lib"
	"${SYSROOT_PATH}/usr/lib/${TOOLCHAIN_HOST}/blas"
	"${SYSROOT_PATH}/usr/lib/${TOOLCHAIN_HOST}/lapack"
)
# You can additionally check the linker paths if you add the flags ' -Xlinker --verbose'
set(COMMON_FLAGS "-I${SYSROOT_PATH}/usr/include ")
FOREACH(LIB ${LIB_DIRS})
	set(COMMON_FLAGS "${COMMON_FLAGS} -L${LIB} -Wl,-rpath-link,${LIB}")
ENDFOREACH()

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SYSROOT_PATH}/usr/lib/${TOOLCHAIN_HOST}")

set(CMAKE_C_FLAGS "-mcpu=cortex-a53 -mfpu=neon-vfpv4 -mfloat-abi=hard ${COMMON_FLAGS}" CACHE STRING "Flags for Raspberry PI 3")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "Flags for Raspberry PI 3")

set(CMAKE_FIND_ROOT_PATH "${CMAKE_INSTALL_PREFIX};${CMAKE_PREFIX_PATH};${CMAKE_SYSROOT}")

# search for programs only in build host directories
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search for libs and headers only in target directories
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
