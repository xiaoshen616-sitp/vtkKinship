### Find VTK
INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

### Listing of the files used in VesselExtraction
FILE(GLOB SRC_VTKKINSHIP_FILTERS *.cxx *.h)

IF( WIN32 )
   ADD_LIBRARY( vtkKinshipFilters STATIC ${SRC_VTKKINSHIP_FILTERS} )
ELSE( WIN32 )
   ADD_LIBRARY( vtkKinshipFilters SHARED ${SRC_VTKKINSHIP_FILTERS} )
ENDIF( WIN32)

TARGET_LINK_LIBRARIES( 
                       vtkKinshipFilters
                       vtkFiltering 
                       vtkRendering 
                       vtkGraphics 
                       vtkImaging 
                       vtkHybrid
                     )
