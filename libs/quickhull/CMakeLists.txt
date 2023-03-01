

add_library(quickhull STATIC
            Structs/Mesh.hpp
            Structs/Plane.hpp
            Structs/Pool.hpp
            Structs/Ray.hpp
            Structs/Vector3.hpp
            Structs/VertexDataSource.hpp
#            Tests/QuickHullTests.cpp
#            Tests/QuickHullTests.hpp
            ConvexHull.hpp
            HalfEdgeMesh.hpp
            MathUtils.hpp
            QuickHull.cpp
            QuickHull.hpp
            Types.hpp
        )

#target_include_directories(libs PRIVATE ${GLIB_INCLUDE_DIRS})
#target_link_libraries(libs PRIVATE ${GLIB_LIBRARIES})
