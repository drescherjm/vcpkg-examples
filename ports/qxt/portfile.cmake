include(vcpkg_common_functions)

string(LENGTH "${CURRENT_BUILDTREES_DIR}" BUILDTREES_PATH_LENGTH)
if(BUILDTREES_PATH_LENGTH GREATER 37 AND CMAKE_HOST_WIN32)
    message(WARNING "${PORT}'s buildsystem uses very long paths and may fail on your system.\n"
        "We recommend moving vcpkg to a short path such as 'C:\\src\\vcpkg' or using the subst command."
    )
endif()

MESSAGE( STATUS "${VCPKG_QT_HOST_TOOLS_ROOT_DIR}=" ${VCPKG_QT_HOST_TOOLS_ROOT_DIR})

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
		QXT_MODULES+=sql
		QXT_MODULES+=network
)

if (CMAKE_HOST_WIN32)
    vcpkg_build_qmake(
        RELEASE_TARGETS sub-tools-3rdparty-qdoc3 sub-src-core sub-src-widgets sub-src-sql
        DEBUG_TARGETS sub-tools-3rdparty-qdoc3 sub-src-core sub-src-widgets sub-src-sql
    )
elseif (CMAKE_HOST_UNIX OR CMAKE_HOST_APPLE) # Build in UNIX
    vcpkg_build_qmake(
        RELEASE_TARGETS sub-tools-3rdparty-qdoc3 sub-src-core sub-src-widgets sub-src-sql
        DEBUG_TARGETS sub-tools-3rdparty-qdoc3 sub-src-core sub-src-widgets sub-src-sql
    )
endif()