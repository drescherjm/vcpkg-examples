include(vcpkg_common_functions)

string(LENGTH "${CURRENT_BUILDTREES_DIR}" BUILDTREES_PATH_LENGTH)
if(BUILDTREES_PATH_LENGTH GREATER 37 AND CMAKE_HOST_WIN32)
    message(WARNING "${PORT}'s buildsystem uses very long paths and may fail on your system.\n"
        "We recommend moving vcpkg to a short path such as 'C:\\src\\vcpkg' or using the subst command."
    )
endif()

vcpkg_from_bitbucket(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO libqxt/libqxt
	REF 696423b68972
	SHA512 32d3c452bccb3a0a331ac29bcb312149573cb6a058b7027f56e14890d92e026d25c964e64c7cc87c597cae7f17a255fc9fe80541e959097915b9f0492b1b1bf0
    HEAD_REF master
    PATCHES
		0001-Fix-compile-of-qdoc3-with-Qt5.patch
)

set(DEBUG_DIR "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg")
set(RELEASE_DIR "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel")

set(ENV{QXT_MODULES} "core widgets")

vcpkg_configure_qmake(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        CONFIG+=${VCPKG_LIBRARY_LINKAGE}
		QXT_MODULES=core
		QXT_MODULES+=widgets
		QXT_MODULES+=docs
)

file(APPEND "${DEBUG_DIR}/.qmake.cache" "INCLUDEPATH+=${SOURCE_PATH}/include/QxtCore \nQXT_BUILD_TREE=${DEBUG_DIR} \nQXT_INSTALL_BINS=${DEBUG_DIR}/bin \nQXT_INSTALL_HEADERS=${DEBUG_DIR}/include \n")
file(APPEND "${DEBUG_DIR}/.qmake.cache" "QXT_INSTALL_FEATURES=${DEBUG_DIR}/features\n")

file(APPEND "${RELEASE_DIR}/.qmake.cache" "INCLUDEPATH+=${SOURCE_PATH}/include/QxtCore \nQXT_BUILD_TREE=${RELEASE_DIR} \nQXT_INSTALL_BINS=${RELEASE_DIR}/bin \nQXT_INSTALL_HEADERS=${RELEASE_DIR}/include \n")
file(APPEND "${RELEASE_DIR}/.qmake.cache" "QXT_INSTALL_FEATURES=${RELEASE_DIR}/features\n")

set(ENV{QXT_BUILD_TREE} ${CURRENT_BUILDTREES_DIR})


if (CMAKE_HOST_WIN32)
    vcpkg_build_qmake(
        RELEASE_TARGETS sub-src-core sub-src-widgets install
        DEBUG_TARGETS sub-src-core sub-src-widgets install
    )
elseif (CMAKE_HOST_UNIX OR CMAKE_HOST_APPLE) # Build in UNIX
    vcpkg_build_qmake(
        RELEASE_TARGETS sub-tools-3rdparty-qdoc3 sub-src-core sub-src-widgets sub-src-sql
        DEBUG_TARGETS sub-tools-3rdparty-qdoc3 sub-src-core sub-src-widgets sub-src-sql
    )
endif()


#Install the header files
#file(GLOB HEADER_FILES ${RELEASE_DIR}/include/*.h)
file(INSTALL ${RELEASE_DIR}/include DESTINATION ${CURRENT_PACKAGES_DIR}/include/qxt)

#Install the module files
if (CMAKE_HOST_WIN32)
    file(INSTALL ${RELEASE_DIR}/lib/QxtCore.lib ${RELEASE_DIR}/lib/QxtCore.exp DESTINATION ${CURRENT_PACKAGES_DIR}/lib)
    file(INSTALL ${DEBUG_DIR}/lib/QxtCored.lib ${DEBUG_DIR}/lib/QxtCored.exp DESTINATION ${CURRENT_PACKAGES_DIR}/debug/lib)

    if(VCPKG_LIBRARY_LINKAGE STREQUAL "dynamic")
        file(INSTALL ${RELEASE_DIR}/bin  DESTINATION ${CURRENT_PACKAGES_DIR}/bin)
        file(INSTALL ${DEBUG_DIR}/bin DESTINATION ${CURRENT_PACKAGES_DIR}/debug/bin)
    endif()
    vcpkg_copy_pdbs()
elseif (CMAKE_HOST_UNIX OR CMAKE_HOST_APPLE) # Build in UNIX
#    file(INSTALL ${RELEASE_DIR}/lib/libqwt.a DESTINATION ${CURRENT_PACKAGES_DIR}/lib)
#    file(INSTALL ${DEBUG_DIR}/lib/libqwt.a DESTINATION ${CURRENT_PACKAGES_DIR}/debug/lib)
endif()


configure_file(${SOURCE_PATH}/COPYING ${CURRENT_PACKAGES_DIR}/share/qxt/copyright COPYONLY)