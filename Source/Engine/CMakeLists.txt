# Set the basic project information.
project(
	FlintEngine
	VERSION 1.0.0
	DESCRIPTION "Flint engine project."
)

# Set the source and header files.
file(GLOB FLINT_ENGINE_SOURCE_FILES *.cpp)
file(GLOB FLINT_ENGINE_HEADER_FILES *.hpp)

# Set the include directories.
target_include_directories(FlintEngine ${GLM_INCLUDE_DIR} ${OPTICK_INCLUDE_DIR})

# Add the library.
add_library(
	FlintEngine
	STATIC
	${FLINT_ENGINE_SOURCE_FILES}
	${FLINT_ENGINE_HEADER_FILES}
)

# Add the FlintGraphics static library as a target link.
target_link_libraries(FlintEngine FlintGraphics)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintEngine PRIVATE "/MP")	
endif ()
