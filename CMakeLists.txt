# the minimum version of CMake.
cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0079 NEW) # Allow to link libraries from different directories than targets

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(taro_harmony)
set(LIBRARY_NAME "TaroHarmonyLibrary")
add_definitions(-DLIBRARY_NAME="${LIBRARY_NAME}")

include(FetchContent)

set(HARMONY_COMPILER_ENV $ENV{HARMONY_COMPILER_ENV})
set(patches_dir "${CMAKE_CURRENT_SOURCE_DIR}/patches")

# ${OHOS_SDK_NATIVE}/cmake/ohos.toolchain.cmake
if (HARMONY_COMPILER_ENV STREQUAL "clion")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(OHOS_ARCH arm64-v8a)
    set(HUAWEI_HOME  $ENV{HUAWEI_HOME})
    set(HMOS_SDK_NATIVE ${HUAWEI_HOME}/default/hms/native/)
    set(OHOS_SDK_NATIVE ${HUAWEI_HOME}/default/openharmony/native/)
    set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES OHOS_SDK_NATIVE HMOS_SDK_NATIVE OHOS_ARCH)

    # include ohos cmake
    # include(${OHOS_SDK_NATIVE}/build/cmake/ohos.toolchain.cmake)
    #${OHOS_SDK_NATIVE}/build/cmake/ohos.toolchain.cmake
    set(OHOS_TOOLCHAIN_NAME aarch64-linux-ohos)
    set(OHOS_LLVM ${OHOS_TOOLCHAIN_NAME})
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    include_directories(${OHOS_SDK_NATIVE}/sysroot/usr/include)
    include_directories(${OHOS_SDK_NATIVE}/sysroot/usr/include/aarch64-linux-ohos)
    list(APPEND CMAKE_FIND_ROOT_PATH "${OHOS_SDK_NATIVE}")
    list(APPEND CMAKE_SYSTEM_LIBRARY_PATH "/usr/lib/${OHOS_TOOLCHAIN_NAME}")

    set(TOOLCHAIN_ROOT_PATH "${OHOS_SDK_NATIVE}/llvm")
    set(TOOLCHAIN_BIN_PATH  "${OHOS_SDK_NATIVE}/llvm/bin")

    set(CMAKE_SYSROOT "${OHOS_SDK_NATIVE}/sysroot")
    set(CMAKE_LIBRARY_ARCHITECTURE "${OHOS_TOOLCHAIN_NAME}")
    list(APPEND CMAKE_SYSTEM_LIBRARY_PATH "/usr/lib/${OHOS_TOOLCHAIN_NAME}")
    set(CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN   "${TOOLCHAIN_ROOT_PATH}")
    set(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN "${TOOLCHAIN_ROOT_PATH}")
    set(CMAKE_ASM_COMPILER_EXTERNAL_TOOLCHAIN "${TOOLCHAIN_ROOT_PATH}")
#    set(CMAKE_C_COMPILER "${TOOLCHAIN_BIN_PATH}/clang${HOST_SYSTEM_EXE_SUFFIX}")
#    set(CMAKE_CXX_COMPILER "${TOOLCHAIN_BIN_PATH}/clang++${HOST_SYSTEM_EXE_SUFFIX}")

    include_directories(${HMOS_SDK_NATIVE}/sysroot/usr/include)
    list(APPEND CMAKE_FIND_ROOT_PATH ${HMOS_SDK_NATIVE}/sysroot/usr)
endif ()

# FMT
FetchContent_Declare(
    FMT
    GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
    GIT_TAG         11.1.4
)
FetchContent_MakeAvailable(FMT)
set(fmt_include_dir "${fmt_SOURCE_DIR}/include")
set(fmt_src_dir "${fmt_SOURCE_DIR}/src")
add_library(fmt_target)
target_include_directories(fmt_target PRIVATE
    "${fmt_include_dir}"
)
target_sources(fmt_target PRIVATE
    "${fmt_src_dir}/format.cc"
    "${fmt_src_dir}/os.cc"
)
target_link_libraries(fmt_target PUBLIC fmt::fmt)

# DOUBLE CONVERSION
FetchContent_Declare(
    DOUBLE_CONVERSION
    GIT_REPOSITORY  https://github.com/google/double-conversion.git
    GIT_TAG         v3.1.5
    SOURCE_SUBDIR   double-conversion
)
FetchContent_MakeAvailable(DOUBLE_CONVERSION)
set(double_conversion_include_dir "${double_conversion_SOURCE_DIR}")
set(double_conversion_src_dir "${double_conversion_SOURCE_DIR}/double-conversion")
add_library(double_conversion_target)
target_include_directories(double_conversion_target PRIVATE
    "${double_conversion_include_dir}"
)
target_sources(double_conversion_target PRIVATE
    "${double_conversion_src_dir}/bignum-dtoa.cc"
    "${double_conversion_src_dir}/bignum.cc"
    "${double_conversion_src_dir}/cached-powers.cc"
    "${double_conversion_src_dir}/diy-fp.cc"
    "${double_conversion_src_dir}/double-conversion.cc"
    "${double_conversion_src_dir}/fast-dtoa.cc"
    "${double_conversion_src_dir}/fixed-dtoa.cc"
    "${double_conversion_src_dir}/strtod.cc"
)

# GLOG
FetchContent_Declare(
    GLOG
    GIT_REPOSITORY  https://github.com/google/glog.git
    GIT_TAG         v0.4.0
)
FetchContent_MakeAvailable(GLOG)
set(glog_include_dir "${glog_SOURCE_DIR}/src")
set(glog_src_dir "${glog_SOURCE_DIR}/src")
add_library(glog_target)
target_include_directories(glog_target PUBLIC
    "${glog_BINARY_DIR}"
    "${glog_include_dir}"
    "${glog_include_dir}/base"
)
target_sources(glog_target PRIVATE
    "${glog_src_dir}/demangle.cc"
    "${glog_src_dir}/logging.cc"
    "${glog_src_dir}/raw_logging.cc"
    "${glog_src_dir}/signalhandler.cc"
    "${glog_src_dir}/symbolize.cc"
    "${glog_src_dir}/utilities.cc"
    "${glog_src_dir}/vlog_is_on.cc"
)
target_compile_options(glog_target PRIVATE
    -Wno-shorten-64-to-32
    -Wno-header-hygiene
    -Wno-deprecated-declarations
    -fdeclspec
)

# BOOST
FetchContent_Declare(
    BOOST
    GIT_REPOSITORY  https://github.com/boostorg/boost.git
    GIT_TAG         boost-1.82.0
    GIT_SUBMODULES_RECURSE TRUE
)
FetchContent_Populate(BOOST)
file(GLOB boost_include_dirs "${boost_SOURCE_DIR}/libs/*/include")
set(BOOST_ENABLE_CMAKE On)
add_subdirectory("${boost_SOURCE_DIR}/libs/core" ${boost_BINARY_DIR}/libs/core)
add_subdirectory("${boost_SOURCE_DIR}/libs/config" ${boost_BINARY_DIR}/libs/config)
add_subdirectory("${boost_SOURCE_DIR}/libs/assert" ${boost_BINARY_DIR}/libs/assert)
add_subdirectory("${boost_SOURCE_DIR}/libs/static_assert" ${boost_BINARY_DIR}/libs/static_assert)
add_subdirectory("${boost_SOURCE_DIR}/libs/type_traits" ${boost_BINARY_DIR}/libs/type_traits)
add_subdirectory("${boost_SOURCE_DIR}/libs/throw_exception" ${boost_BINARY_DIR}/libs/throw_exception)
add_subdirectory("${boost_SOURCE_DIR}/libs/move" ${boost_BINARY_DIR}/libs/move)
add_subdirectory("${boost_SOURCE_DIR}/libs/predef" ${boost_BINARY_DIR}/libs/predef)
add_subdirectory("${boost_SOURCE_DIR}/libs/pool" ${boost_BINARY_DIR}/libs/pool)
add_subdirectory("${boost_SOURCE_DIR}/libs/mp11" ${boost_BINARY_DIR}/libs/mp11)
add_subdirectory("${boost_SOURCE_DIR}/libs/smart_ptr" ${boost_BINARY_DIR}/libs/smart_ptr)
add_subdirectory("${boost_SOURCE_DIR}/libs/winapi" ${boost_BINARY_DIR}/libs/winapi)
add_subdirectory("${boost_SOURCE_DIR}/libs/integer" ${boost_BINARY_DIR}/libs/integer)
add_subdirectory("${boost_SOURCE_DIR}/libs/context" ${boost_BINARY_DIR}/libs/context)

# FOLLY
FetchContent_Declare(
    FOLLY
    GIT_REPOSITORY  https://github.com/facebook/folly.git
    GIT_TAG         v2022.01.24.00
)
FetchContent_Populate(FOLLY)
set(folly_include_dir "${folly_SOURCE_DIR}")
set(folly_src_dir "${folly_SOURCE_DIR}/folly")
add_library(folly_target)
target_include_directories(folly_target PUBLIC
    "${folly_include_dir}"
    "${boost_include_dirs}"
    "${double_conversion_include_dir}"
    "${glog_include_dir}"
    "${fmt_include_dir}"
)
set(folly_compile_options
    -DFOLLY_NO_CONFIG=1
    -DFOLLY_MOBILE=1
    -DFOLLY_USE_LIBCPP=1
    -DFOLLY_HAVE_RECVMMSG=1
    -DFOLLY_HAVE_PTHREAD=1
    -Wno-comma
    -Wno-shorten-64-to-32
    -Wno-documentation
    -faligned-new
)
target_compile_options(folly_target PUBLIC ${folly_compile_options})
target_sources(folly_target PRIVATE
    "${folly_src_dir}/SharedMutex.cpp"
    "${folly_src_dir}/concurrency/CacheLocality.cpp"
    "${folly_src_dir}/detail/Futex.cpp"
    "${folly_src_dir}/portability/Malloc.cpp"
    "${folly_src_dir}/synchronization/ParkingLot.cpp"
    "${folly_src_dir}/system/ThreadId.cpp"
    "${folly_src_dir}/lang/SafeAssert.cpp"
    "${folly_src_dir}/lang/ToAscii.cpp"
    "${folly_src_dir}/dynamic.cpp"
    "${folly_src_dir}/hash/SpookyHashV2.cpp"
    "${folly_src_dir}/json_pointer.cpp"
    "${folly_src_dir}/Conv.cpp"
    "${folly_src_dir}/Format.cpp"
    "${folly_src_dir}/memory/detail/MallocImpl.cpp"
    "${folly_src_dir}/json.cpp"
    "${folly_src_dir}/Unicode.cpp"
    "${folly_src_dir}/lang/Assume.cpp"
    "${folly_src_dir}/ScopeGuard.cpp"
)
target_link_libraries(folly_target PUBLIC
    glog_target
    double_conversion_target
)

# 去除编译警告以错误形式呈现
add_compile_options(-Wno-unused-command-line-argument)

set(INIT_FILE init.cpp)
file(GLOB_RECURSE ARKJS_FILES CONFIGURE_DEPENDS arkjs/*.cpp)
file(GLOB_RECURSE ENGINE_FILES CONFIGURE_DEPENDS engine/*.cpp)
file(GLOB_RECURSE EXPORTS_FILES CONFIGURE_DEPENDS exports/*.cpp)
file(GLOB_RECURSE HELPER_FILES CONFIGURE_DEPENDS helper/*.cpp)
file(GLOB_RECURSE RUNTIME_FILES CONFIGURE_DEPENDS runtime/*.cpp)
file(GLOB_RECURSE TARO_FILES CONFIGURE_DEPENDS taro/*.cpp)
file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS test/*.cpp)
file(GLOB_RECURSE THREAD_FILES CONFIGURE_DEPENDS thread/*.cpp)
file(GLOB_RECURSE SYSTEM_FILES CONFIGURE_DEPENDS system/*.cpp)

# YOGA Start
FetchContent_Declare(
    YOGA
    GIT_REPOSITORY  https://github.com/NervJS/yoga.git
    GIT_TAG         feat/taro_harmony_cpp
)
FetchContent_Populate(YOGA)
add_subdirectory(${yoga_SOURCE_DIR}/yoga ${yoga_BINARY_DIR}/yoga)
# YOGA End

# QuickJS Start
FetchContent_Declare(
    QUICKJS
    GIT_REPOSITORY  https://github.com/bellard/quickjs.git
    GIT_TAG         revert-265-has-exception
)
FetchContent_Populate(QUICKJS)
# QuickJS build options
set(qjs_cflags -Wall -D_GNU_SOURCE)
if(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang")
  list(APPEND qjs_cflags
       -Wextra
       -Wno-sign-compare
       -Wno-missing-field-initializers
       -Wno-unused-parameter
       -Wno-unused-variable
       -funsigned-char)
else()
  list(APPEND qjs_cflags -Wno-array-bounds -Wno-unused-variable
       -Wno-unused-but-set-variable)
endif()

file(STRINGS "${quickjs_SOURCE_DIR}/VERSION" QJS_VERSION_STR)

# Build QuickJS library
add_library(qjs STATIC ${quickjs_SOURCE_DIR}/cutils.c
                       ${quickjs_SOURCE_DIR}/libbf.c
                       ${quickjs_SOURCE_DIR}/libregexp.c
                       ${quickjs_SOURCE_DIR}/libunicode.c
                       ${quickjs_SOURCE_DIR}/quickjs.c
                       ${quickjs_SOURCE_DIR}/quickjs-libc.c)
set_target_properties(qjs PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED ON)
target_compile_options(qjs PRIVATE ${qjs_cflags})
target_compile_definitions(qjs PUBLIC QJS_VERSION_STR="${QJS_VERSION_STR}")
target_compile_definitions(qjs PUBLIC CONFIG_BIGNUM CONFIG_VERSION="${QJS_VERSION_STR}")
if(CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(qjs PRIVATE DUMP_LEAKS)
endif()
file(WRITE ${quickjs_SOURCE_DIR}/VERSION "")
include_directories(${quickjs_SOURCE_DIR})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries(qjs atomic)
endif()

if(UNIX)
  target_link_libraries(qjs PRIVATE m dl pthread)
endif()
# QuickJS End

# JSI Start
FetchContent_Declare(
    JSI
    GIT_REPOSITORY  https://github.com/microsoft/v8-jsi.git
    GIT_TAG         0.71-stable
    SOURCE_SUBDIR   src
)
FetchContent_Populate(JSI)
set(jsi_src_dir "${jsi_SOURCE_DIR}/src")
# 基本源文件
set(jsi_sources
    ${jsi_src_dir}/jsi/jsi.cpp
    ${jsi_src_dir}/jsi/JSIDynamic.cpp
)

# 根据系统类型添加特定源文件
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND jsi_sources ${jsi_src_dir}/jsi/jsilib-windows.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND jsi_sources ${jsi_src_dir}/jsi/jsilib-posix.cpp)
endif()

# 添加库
add_library(jsi_target STATIC ${jsi_sources})
target_link_libraries(jsi_target PUBLIC folly_target)
# 设置 include 目录
include_directories("${jsi_src_dir}")
# JSI End

# GoogleTest Start
# 将 Google Test 子模块添加到项目中
FetchContent_Declare(
    GOOGLE_TEST
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         v1.16.0
)
FetchContent_MakeAvailable(GOOGLE_TEST)
# 包含 Google Test 头文件目录
include_directories(${google_test_SOURCE_DIR}/googletest/include)
# GoogleTest End

add_library(${LIBRARY_NAME} SHARED
    ${INIT_FILE}
    ${ARKJS_FILES}
    ${ENGINE_FILES}
    ${EXPORTS_FILES}
    ${HELPER_FILES}
    ${RUNTIME_FILES}
    ${TARO_FILES}
    ${TEST_FILES}
    ${THREAD_FILES}
    ${SYSTEM_FILES}
)
# target_link_libraries(entry PUBLIC libace_napi.z.so, libace_ndk.z.so, libhilog_ndk.z.so)

find_library(
    # Sets the name of the path variable.
    hilog-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    hilog_ndk.z
)

find_library(
    # Sets the name of the path variable.
    libace-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    ace_ndk.z
)

find_library(
    # Sets the name of the path variable.
    libnapi-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    ace_napi.z
)

find_library(
    # Sets the name of the path variable.
    libvsync-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    native_vsync
)

find_library(
    libdrawing-lib
    native_drawing
)

# 获取项目版本号
set(PACKAGE_JSON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../../oh-package.json5")
file(READ ${PACKAGE_JSON_FILE} PACKAGE_JSON_CONTENT)
string(REGEX MATCH "\"version\": \"([0-9]+(\\.[0-9]+)+[^\"\\s]*)\"" _ ${PACKAGE_JSON_CONTENT})
set(PACKAGE_VERSION ${CMAKE_MATCH_1})
add_definitions(-DTARO_PACKAGE_VERSION="${PACKAGE_VERSION}")
add_definitions(-DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_definitions(IS_DEBUG=0)
    target_compile_options(${LIBRARY_NAME} PRIVATE -fvisibility=hidden)
else ()
    add_compile_definitions(IS_DEBUG=1)
    add_compile_definitions(WITH_HITRACE_SYSTRACE)
    set(WITH_HITRACE_SYSTRACE 1)  # 此配置为 hitrace 启动
    # 更新 dts 文件说明
    add_custom_command(
        TARGET ${LIBRARY_NAME}
        COMMAND ${CMAKE_BINARY_DIR}/../../../../../src/main/cpp/types/taro-native-node/bin/init.sh
        COMMENT "Generate taro-native-node.d.ts file"
    )
endif ()

set_property(TARGET ${LIBRARY_NAME} PROPERTY C_STANDARD 11)
add_definitions(-DTARO_CONFIG_VERSION="2024-02-14")
target_include_directories(${LIBRARY_NAME} PUBLIC ${HMOS_SDK_NATIVE}/sysroot/usr/include)
target_link_directories(${LIBRARY_NAME} PUBLIC ${HMOS_SDK_NATIVE}/sysroot/usr/lib/aarch64-linux-ohos)
target_link_libraries(${LIBRARY_NAME} PUBLIC qjs uv libqos.so folly_target jsi_target glog_target
        ${hilog-lib} ${libace-lib} ${libnapi-lib} ${libvsync-lib} ${libdrawing-lib} libpixelmap.so libnative_window.so libnative_drawing.so libimage_packer.so libimage_source.so librcp_c.so yogacore gtest_main)

if(DEFINED WITH_HITRACE_SYSTRACE)
    target_link_libraries(${LIBRARY_NAME} PUBLIC libhitrace_ndk.z.so)
endif()

# 输出版本号
message(STATUS "Package version: ${PACKAGE_VERSION}")

# 启用测试
enable_testing()

include_directories(./)
