include (VCICommon)
 
include_directories (
  ../../..
  ${CMAKE_CURRENT_SOURCE_DIR}
)
 
set (targetName Sajat)
 
# collect all header and source files
vci_append_files (headers "*.hpp" .)
vci_append_files (sources "*.cpp" .)
 
vci_add_executable (${targetName} ${headers} ${sources})
 
target_link_libraries (${targetName}
  OpenMeshCore
  OpenMeshTools
)
 

