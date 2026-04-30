add_library(index_core STATIC src/InvertedIndex.cpp "src/Document.cpp" "src/DocumentBuilder.cpp")

target_link_libraries(index_core PRIVATE project_warnings)
if (TARGET project_sanitizers)
        target_link_libraries(index_core PRIVATE project_sanitizers)
endif ()

target_include_directories(index_core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)