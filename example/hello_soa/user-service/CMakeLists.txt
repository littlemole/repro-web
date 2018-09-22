############################################
# preamble
############################################
cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0054 NEW)
project(hello_soa_userservice)

############################################
# c++ std
############################################

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


############################################
# basic dependecies
############################################

find_package(OpenSSL REQUIRED)


############################################
# clang support
############################################

set(LINKERFLAGS "")
set(STDLIB "")
set(FLAGS "")


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(STDLIB "c++abi")
    set(LINKERFLAGS "-stdlib=libc++ -fcoroutines-ts  -std=c++14")
    set(FLAGS "-stdlib=libc++ -fcoroutines-ts -D_RESUMABLE_FUNCTIONS_SUPPORTED")
endif()

############################################
# OS support (Linux/Windows)
############################################

IF (WIN32)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

message("+++++++++++++++++")
message("${CMAKE_BUILD_TYPE}")
message("+++++++++++++++++")

	set(OSLIBS "Ws2_32" "Rpcrt4" "Shlwapi")
	set(BUILDFLAGS "-await ${FLAGS}")

	find_library(LIBEVENT_LIBRARY NAMES event)
	find_library(CURL_LIBRARY NAMES libcurl)
	find_library(NG2HTTP_LIBRARY NAMES nghttp2)
	set(LIBS ${OPENSSL_LIBRARIES} ${STDLIB}  )

	if(${CMAKE_BUILD_TYPE} MATCHES "Release")
		find_library(PRIOCPP_LIBRARY NAMES priocpp)

		find_library(PRIOCPP_LIBRARY NAMES priocpp)
		find_library(PRIOHTTP_LIBRARY NAMES priohttp)
		find_library(NG2HTTP_LIBRARY NAMES nghttp2)
		find_library(LIBEVENT_LIBRARY NAMES event)
		find_library(CRYPTONEAT_LIBRARY NAMES cryptoneat)
		find_library(JSONCPP_LIBRARY NAMES jsoncpp)
		find_library(CURL_LIBRARY NAMES libcurl)
		find_library(ZLIB_LIBRARY NAMES zlib.lib)
		find_library(REPROCURL_LIBRARY NAMES reprocurl)
		find_library(REPROWEB_LIBRARY NAMES reproweb)
		find_library(REPROMYSQL_LIBRARY NAMES repromysql)
		find_library(MYSQL_LIBRARY NAMES mysql)

		set(LIBS 
			${REPROWEB_LIBRARY} 
			${PRIOCPP_LIBRARY} 
			${REPROMYSQL_LIBRARY}
			${PRIOHTTP_LIBRARY} 
			${REPROCURL_LIBRARY} 
			${CRYPTONEAT_LIBRARY} 
			${NG2HTTP_LIBRARY} 
			${JSONCPP_LIBRARY} 
			${CURL_LIBRARY} 
			${ZLIB_LIBRARY}
			${MYSQL_LIBRARY}
			${LIBS} 
		)
    else()
    	find_library(PRIOCPP_LIBRARY NAMES priocpp.lib)
    	find_library(PRIOCPPD_LIBRARY NAMES priocppd.lib)

		find_library(PRIOCPP_LIBRARY NAMES priocpp)
		find_library(PRIOCPPD_LIBRARY NAMES priocppd)
		find_library(PRIOHTTP_LIBRARY NAMES priohttp)
		find_library(PRIOHTTPD_LIBRARY NAMES priohttpd)
		find_library(NG2HTTP_LIBRARY NAMES nghttp2)
		find_library(LIBEVENT_LIBRARY NAMES event)
		find_library(CRYPTONEAT_LIBRARY NAMES cryptoneat)
		find_library(CRYPTONEATD_LIBRARY NAMES cryptoneatd)
		find_library(JSONCPP_LIBRARY NAMES jsoncpp)
		find_library(CURL_LIBRARY NAMES libcurl)
		find_library(REPROCURL_LIBRARY NAMES reprocurl)
		find_library(REPROCURLD_LIBRARY NAMES reprocurld)
		find_library(ZLIB_LIBRARY NAMES zlib.lib)
		find_library(ZLIBD_LIBRARY NAMES zlibd.lib)
		find_library(REPROWEB_LIBRARY NAMES reproweb)
		find_library(REPROWEBD_LIBRARY NAMES reprowebd)
		find_library(REPROMYSQL_LIBRARY NAMES repromysql)
		find_library(REPROMYSQLD_LIBRARY NAMES repromysqld)
		find_library(MYSQL_LIBRARY NAMES mysql)


		set(LIBS  
			debug ${PRIOCPPD_LIBRARY} optimized  ${PRIOCPP_LIBRARY} 
			debug ${REPROMYSQLD_LIBRARY} optimized  ${REPROMYSQL_LIBRARY} 
			debug ${REPROWEBD_LIBRARY} optimized ${REPROWEB_LIBRARY}  
			debug ${PRIOCPPD_LIBRARY} optimized ${PRIOCPP_LIBRARY} 
			debug ${PRIOHTTPD_LIBRARY} optimized ${PRIOHTTP_LIBRARY} 
			debug ${CRYPTONEATD_LIBRARY} optimized ${CRYPTONEAT_LIBRARY} 
			debug ${REPROCURLD_LIBRARY} optimized ${REPROCURL_LIBRARY}
			${NG2HTTP_LIBRARY}
			${JSONCPP_LIBRARY} 
			${CURL_LIBRARY} 
			${MYSQL_LIBRARY}
			debug ${ZLIBD_LIBRARY} optimized ${ZLIB_LIBRARY}
			${LIBS}
		)
	endif()


 ELSEIF (UNIX)

    set(OSLIBS "pthread")

    find_package(PkgConfig)
    pkg_check_modules(CURL REQUIRED libcurl)
    pkg_check_modules(NGHTTP2 REQUIRED libnghttp2)
    pkg_check_modules(PRIOCPP REQUIRED priocpp)
    pkg_check_modules(PRIOHTTP REQUIRED priohttp)
    pkg_check_modules(REPROWEB REQUIRED reproweb)
    pkg_check_modules(REPROCURL REQUIRED reprocurl)
    pkg_check_modules(REPROSMYSQL REQUIRED repromysql)
    pkg_check_modules(LIBEVENT REQUIRED libevent_pthreads)
    pkg_check_modules(CRYPTONEAT REQUIRED cryptoneat)
    pkg_check_modules(JSONCPP REQUIRED jsoncpp)
    pkg_check_modules(MYSQL REQUIRED mysqlclient)

    set(LIBEVENT_LIBRARY ${LIBEVENT_LIBRARIES})

    set(BUILDFLAGS " ${FLAGS}")

    set(LIBS 
		debug reprowebd optimized reproweb
		debug reprocurld optimized reprocurl 
		debug priohttpd optimized priohttp 
		debug repromysqld optimized repromysql
		debug priocppd optimized priocpp 
		debug cryptoneatd optimized cryptoneat 
		${GTEST_LIBRARIES} ${OPENSSL_LIBRARIES} ${NGHTTP2_LIBRARIES} ${CURL_LIBRARIES} ${JSONCPP_LIBRARIES} ${MYSQL_LIBRARIES} ${STDLIB} z
	)


ENDIF ()

message("+++++++++++++++++")
message("LIBS: ${LIBS}")
message("+++++++++++++++++")


############################################
# set linker and compiler flags
############################################

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LINKERFLAGS}" )
set(CMAKE_CXX_FLAGS_DEBUG  	"${CMAKE_CXX_FLAGS_DEBUG} -DMOL_PROMISE_DEBUG")
set(CMAKE_CXX_FLAGS  		"${CMAKE_CXX_FLAGS} ${BUILDFLAGS}")


############################################
# backend: libevent or boost_asio
############################################


option(WITH_LIBEVENT "use libevent (default:on)" ON)
if(WITH_LIBEVENT)
    add_definitions(-DPROMISE_USE_LIBEVENT)    
    set(BACKEND ${LIBEVENT_LIBRARY})
else()
    add_definitions(-DPROMISE_USE_BOOST_ASIO)
    set(BACKEND "boost_system")
endif()

############################################
# include directories
############################################

include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${PRIOCPP_INCLUDE_DIRS})
include_directories(${MYSQL_INCLUDE_DIRS})

message("+++++++++++++++++")
message("DIRS: ${OPENSSL_INCLUDE_DIR}")
message("+++++++++++++++++")

############################################
# sources
############################################

file(GLOB SOURCES "./*.cpp")


############################################
# exe
############################################

add_executable(${CMAKE_PROJECT_NAME}.bin ./main.cpp)

############################################
# libs for exe
############################################

target_link_libraries(${CMAKE_PROJECT_NAME}.bin ${LIBS} ${BACKEND} ${OSLIBS} )

