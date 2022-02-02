INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_PLL pll)

FIND_PATH(
    PLL_INCLUDE_DIRS
    NAMES pll/api.h
    HINTS $ENV{PLL_DIR}/include
        ${PC_PLL_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    PLL_LIBRARIES
    NAMES gnuradio-pll
    HINTS $ENV{PLL_DIR}/lib
        ${PC_PLL_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/pllTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PLL DEFAULT_MSG PLL_LIBRARIES PLL_INCLUDE_DIRS)
MARK_AS_ADVANCED(PLL_LIBRARIES PLL_INCLUDE_DIRS)
