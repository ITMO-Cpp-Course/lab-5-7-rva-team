add_library(index_core STATIC
        index_core/src/Document.cpp
        index_core/src/DocumentBuilder.cpp
        index_core/src/InvertedIndex.cpp
        index_core/src/IndexStore.cpp
        index_core/src/Result.cpp
        index_core/src/UpdateTransaction.cpp)

target_link_libraries(index_core PRIVATE project_warnings)
if (TARGET project_sanitizers)
        target_link_libraries(index_core PRIVATE project_sanitizers)
endif ()

target_include_directories(index_core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/index_core/include
)
