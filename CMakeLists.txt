cmake_minimum_required(VERSION 3.10)

project(msdfgen VERSION 1.7.1 LANGUAGES CXX)
option(MSDFGEN_USE_OPENMP "Build with OpenMP support for multithreaded code" OFF)
option(MSDFGEN_USE_CPP11 "Build with C++11 enabled" ON)

find_package(freetype CONFIG REQUIRED)

#----------------------------------------------------------------
# Target configuration
#----------------------------------------------------------------

add_library(msdfgen 
	"src/Contour.cpp"
	"src/edge-coloring.cpp"
	"src/edge-segments.cpp"
	"src/edge-selectors.cpp"
	"src/EdgeHolder.cpp"
	"src/equation-solver.cpp"
	"src/estimate-sdf-error.cpp"
	"src/msdfgen.cpp"
	"src/rasterization.cpp"
	"src/render-sdf.cpp"
	"src/save-bmp.cpp"
	"src/save-tiff.cpp"
	"src/Scanline.cpp"
	"src/shape-description.cpp"
	"src/Shape.cpp"
	"src/SignedDistance.cpp"
	"src/Vector2.cpp")

add_library(msdfgen::msdfgen ALIAS msdfgen)

set_target_properties(msdfgen PROPERTIES PUBLIC_HEADER "${msdfgen_HEADERS}")
target_include_directories(msdfgen PUBLIC
	$<INSTALL_INTERFACE:include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(MSDFGEN_USE_CPP11)
	target_compile_features(msdfgen PUBLIC cxx_std_11)
	target_compile_definitions(msdfgen PUBLIC MSDFGEN_USE_CPP11)
endif()

if(MSDFGEN_USE_OPENMP)
	# Note: Clang doesn't support OpenMP by default...
	find_package(OpenMP REQUIRED COMPONENTS CXX)
	target_link_libraries(msdfgen PRIVATE OpenMP::OpenMP_CXX)
	target_compile_definitions(msdfgen PRIVATE MSDFGEN_USE_OPENMP)
endif()

