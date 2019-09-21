cmake_minimum_required(VERSION 2.8)
project(cvui_test)

# Create cvui_test project
set(SOURCES
	Main.cpp
	AreaContainer.cpp
)

set(HEADERS
	Common.h
	AreaContainer.h
)

add_executable(cvui_test
	${SOURCES}
	${HEADERS}
)


# For OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
	target_include_directories(cvui_test PUBLIC ${OpenCV_INCLUDE_DIRS})
	target_link_libraries(cvui_test ${OpenCV_LIBS})
endif()

# for cvui
target_include_directories(cvui_test PUBLIC cvui)
