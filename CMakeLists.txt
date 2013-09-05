PROJECT( testMeshValmet )

CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

FIND_PACKAGE( Qt3 REQUIRED ) 

FIND_PACKAGE( VTK REQUIRED )
INCLUDE( ${VTK_USE_FILE} )

INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

LINK_LIBRARIES( ${QT_QT_LIBRARY} ${QT_GL_LIBRARY} vtkRendering vtkWidgets )

SET( testMeshValmet_SRCS
 main.cxx
 MeshValmetControls.cxx
 reporting.cxx
 mesh_run.cxx
 compute_error.cxx
 3dmodel.h
 compute_volume_overlap.cxx
 TextWidget.cxx
 geomutils.cxx
)

SET( testMeshValmet_GUI_SRCS  testMeshValmetGui.ui )

QT_WRAP_UI( testMeshValmet testMeshValmet_HDRS  testMeshValmet_SRCS  ${testMeshValmet_GUI_SRCS} )

ADD_DEFINITIONS( -DQT_DLL )

ADD_DEFINITIONS( -DQT_THREAD_SUPPORT )

ADD_EXECUTABLE( testMeshValmet ${testMeshValmet_SRCS} )
