vcpkg_fail_port_install(ON_TARGET "uwp")

vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO malaterre/GDCM
    REF b56f671dba79dca00d8cb3ebde83dbd88292e9e1 # v3.0.10
    SHA512 58b4f1473e58e24339dccd8b8b7458a381077aacdbe7f059cfdcfe002a1f91ae837717889389ea33e3189e14ef6de472bac4f86d94f727757e534b0b94d7ed7b
	#REF 17dc99ed77f10aada90448c524fcd0be62a40365 # v3.0.9
    #SHA512 bfabd7b16d0301fb97be902cf1b1a63f387e1c289d7d87d2a39d4a169862a64b3c973df1d705da3da650f3908a51cca57b24353e993c8a2047f255f48196885d
    HEAD_REF master
    PATCHES
        use-openjpeg-config.patch
        fix-share-path.patch
        Fix-Cmake_DIR.patch
)



if ("vtk" IN_LIST FEATURES)
    set(GDCM_USE_VTK                  ON )
else()
    set(GDCM_USE_VTK                  OFF )
endif()

if(GDCM_USE_VTK)
    list(APPEND ADDITIONAL_OPTIONS
        -DGDCM_USE_VTK=ON
    )
endif()

file(REMOVE ${SOURCE_PATH}/CMake/FindOpenJPEG.cmake)

if(VCPKG_LIBRARY_LINKAGE STREQUAL "dynamic")
  set(VCPKG_BUILD_SHARED_LIBS ON)
else()
  set(VCPKG_BUILD_SHARED_LIBS OFF)
endif()

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS
        -DGDCM_BUILD_DOCBOOK_MANPAGES=OFF
        -DGDCM_BUILD_SHARED_LIBS=${VCPKG_BUILD_SHARED_LIBS}
        -DGDCM_INSTALL_INCLUDE_DIR=include
        -DGDCM_USE_SYSTEM_EXPAT=ON
        -DGDCM_USE_SYSTEM_ZLIB=ON
        -DGDCM_USE_SYSTEM_OPENJPEG=ON
        -DGDCM_BUILD_TESTING=OFF
		${ADDITIONAL_OPTIONS}
)

vcpkg_install_cmake()
vcpkg_fixup_cmake_targets(CONFIG_PATH lib/gdcm)
vcpkg_copy_pdbs()

file(REMOVE_RECURSE
    ${CURRENT_PACKAGES_DIR}/debug/include
    ${CURRENT_PACKAGES_DIR}/debug/share
)

vcpkg_replace_string(${CURRENT_PACKAGES_DIR}/share/gdcm/GDCMTargets.cmake
    "set(CMAKE_IMPORT_FILE_VERSION 1)"
    "set(CMAKE_IMPORT_FILE_VERSION 1)
    find_package(OpenJPEG QUIET)"
)

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/bin ${CURRENT_PACKAGES_DIR}/debug/bin)
endif()

file(INSTALL ${SOURCE_PATH}/Copyright.txt DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)