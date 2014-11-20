include_directories("${PROJECT_SOURCE_DIR}/opensubdiv")

if(IOS)
	add_library(OpenSubdivUnity OpenSubdivUnity.cpp)
else()
	add_library(OpenSubdivUnity MODULE OpenSubdivUnity.cpp)
endif()

target_link_libraries(OpenSubdivUnity osd_static_cpu osdutil)

if(IOS)
	set(BUILD_ARCHS "$(ARCHS_STANDARD)")
else()
	set(BUILD_ARCHS "$(ARCHS_STANDARD_32_64_BIT)")
endif()

if(IOS)
	set_target_properties(OpenSubdivUnity PROPERTIES
		OSX_ARCHITECTURES "${BUILD_ARCHS}"
		XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libstdc++"
	)
else()
	set_target_properties(OpenSubdivUnity PROPERTIES
		OSX_ARCHITECTURES "${BUILD_ARCHS}"
		XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libstdc++"
		BUNDLE TRUE
	)
endif()

add_executable(TestOpenSubdivUnity TestOpenSubdivUnity.cpp OpenSubdivUnity.cpp)
target_link_libraries(TestOpenSubdivUnity osd_static_cpu osdutil)

set_target_properties(osd_static_cpu PROPERTIES
	OSX_ARCHITECTURES "${BUILD_ARCHS}"
	XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libstdc++"
)

set_target_properties(osd_cpu_obj PROPERTIES
	OSX_ARCHITECTURES "${BUILD_ARCHS}"
	XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libstdc++"
)

set_target_properties(osdutil PROPERTIES
	OSX_ARCHITECTURES "${BUILD_ARCHS}"
	XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libstdc++"
)
