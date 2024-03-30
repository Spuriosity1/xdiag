cmake_minimum_required(VERSION 3.19)

set(hydra_distributed_known_comps static shared)
set(hydra_distributed_comp_static NO)
set(hydra_distributed_comp_shared NO)
foreach (hydra_distributed_comp IN LISTS ${CMAKE_FIND_PACKAGE_NAME}_FIND_COMPONENTS)
    if (hydra_distributed_comp IN_LIST hydra_distributed_known_comps)
        set(hydra_distributed_comp_${hydra_distributed_comp} YES)
    else ()
        set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE
            "hydra_distributed does not recognize component `${hydra_distributed_comp}`.")
        set(${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
        return()
    endif ()
endforeach ()

if (hydra_distributed_comp_static AND hydra_distributed_comp_shared)
    set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE
        "hydra_distributed `static` and `shared` components are mutually exclusive.")
    set(${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    return()
endif ()

set(hydra_distributed_static_targets "${CMAKE_CURRENT_LIST_DIR}/hydra_distributed-static-targets.cmake")
set(hydra_distributed_shared_targets "${CMAKE_CURRENT_LIST_DIR}/hydra_distributed-shared-targets.cmake")

macro(hydra_distributed_load_targets type)
    if (NOT EXISTS "${hydra_distributed_${type}_targets}")
        set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE
            "hydra_distributed `${type}` libraries were requested but not found.")
        set(${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
        return()
    endif ()
    include("${hydra_distributed_${type}_targets}")
endmacro()

if (hydra_distributed_comp_static)
    hydra_distributed_load_targets(static)
elseif (hydra_distributed_comp_shared)
    hydra_distributed_load_targets(shared)
elseif (DEFINED hydra_distributed_SHARED_LIBS AND hydra_distributed_SHARED_LIBS)
    hydra_distributed_load_targets(shared)
elseif (DEFINED hydra_distributed_SHARED_LIBS AND NOT hydra_distributed_SHARED_LIBS)
    hydra_distributed_load_targets(static)
elseif (BUILD_SHARED_LIBS)
    if (EXISTS "${hydra_distributed_shared_targets}")
        hydra_distributed_load_targets(shared)
    else ()
        hydra_distributed_load_targets(static)
    endif ()
else ()
    if (EXISTS "${hydra_distributed_static_targets}")
        hydra_distributed_load_targets(static)
    else ()
        hydra_distributed_load_targets(shared)
    endif ()
endif ()