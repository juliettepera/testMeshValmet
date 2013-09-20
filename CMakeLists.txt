# minimum version required
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 ) 

# name of the project
PROJECT( testMeshValmet )

# find the VTK headers
FIND_PACKAGE( VTK REQUIRED )
INCLUDE( ${VTK_USE_FILE} )

# find the QT4 headers
FIND_PACKAGE( Qt4 REQUIRED ) 
INCLUDE( ${QT_USE_FILE} )

# include the directories for the binary and sources files
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}  ${QT_INCLUDE_DIR} )

# ?????
ADD_DEFINITIONS( -DQT_GUI_LIBS -DQT_CORE_LIB -DQT3_SUPPORT )

# generate moc and ui files
QT4_WRAP_CPP( QtProject_HEADERS_MOC testMeshValmet.h )
QT4_WRAP_UI( UI_FILES testMeshValmetGui.ui )

# set the sources files
SET( Dsources 

main.cxx 

testMeshValmet.cxx 
testMeshValmet.h

MeshValmet/3dmodel.h

MeshValmet/mesh_run.cxx
MeshValmet/mesh_run.h

MeshValmet/model_in.cxx
MeshValmet/model_in.h

MeshValmet/read_model.h

MeshValmet/compute_error.cxx
MeshValmet/compute_error.h

MeshValmet/geomutils.cxx
MeshValmet/geomutils.h

MeshValmet/model_analysis.cxx
MeshValmet/model_analysis.h

MeshValmet/reporting.cxx
MeshValmet/reporting.h

MeshValmet/xalloc.cxx
MeshValmet/xalloc.h

MeshValmet/compute_volume_overlap.cxx
MeshValmet/compute_volume_overlap.h

${QtProject_HEADERS_MOC} ${UI_FILES} )

# generate the executable
ADD_EXECUTABLE( testMeshValmet ${Dsources} )

# link the libraries to the executable
TARGET_LINK_LIBRARIES( testMeshValmet ${QT_LIBRARIES} ${VTK_LIBRARIES} )
