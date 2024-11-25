include (VCICommon)
 
include_directories (
  ../../..
  ${CMAKE_CURRENT_SOURCE_DIR}
)
 
set (targetName SupportGeneration)

 
# collect all header and source files
vci_append_files (headers "*.hpp" .)
vci_append_files (sources "*.cpp" .)
vci_append_files (sources "*.h" .)
vci_append_files (sources "*.hh" .)
vci_append_files (sources "*.cc" .)

vci_add_executable (${targetName} ${headers} ${sources})
 
target_link_libraries (${targetName}
  OpenMeshCore
  OpenMeshTools
)