cmake_minimum_required( VERSION 3.13 )

project( meshL )
add_library( ${PROJECT_NAME} STATIC
             BLoopL.hxx
             EdgeL.hxx
             FaceL.hxx
             HalfedgeL.hxx
             LoopL.hxx
             MeshL.hxx
             MeshUtiL.hxx
             NodeL.hxx
             NormalL.hxx
             TexcoordL.hxx
             VertexL.hxx
             VertexLCirculator.hxx
             SMFLIO.hxx
)

if(UNIX)
        target_include_directories( ${PROJECT_NAME}
                                    PRIVATE
                                    ${PROJECT_SOURCE_DIR}/../eigen
                                    ${PROJECT_SOURCE_DIR}/../util
                                    )

        target_compile_features( ${PROJECT_NAME}
                                 PRIVATE
                                 cxx_std_11
                                 )
endif(UNIX)

# Set StartUp Project (Option)
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "meshL" )
