
SET (WCS_SRCS
  ../gdal/qgsgdalproviderbase.cpp
  qgswcsprovider.cpp
  qgswcscapabilities.cpp
  qgswcssourceselect.cpp
  qgswcsdataitems.cpp
)

SET (WCS_MOC_HDRS
  qgswcsprovider.h
  qgswcscapabilities.h
  qgswcssourceselect.h
  qgswcsdataitems.h
)

QT5_WRAP_CPP (WCS_MOC_SRCS ${WCS_MOC_HDRS})

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src/core
  ${CMAKE_SOURCE_DIR}/src/core/auth
  ${CMAKE_SOURCE_DIR}/src/core/expression
  ${CMAKE_SOURCE_DIR}/src/core/raster
  ${CMAKE_SOURCE_DIR}/src/core/geometry
  ${CMAKE_SOURCE_DIR}/src/core/metadata
  ${CMAKE_SOURCE_DIR}/src/gui
  ${CMAKE_SOURCE_DIR}/src/gui/auth
  ${CMAKE_SOURCE_DIR}/src/providers/gdal

  ${CMAKE_BINARY_DIR}/src/core
  ${CMAKE_BINARY_DIR}/src/gui
  ${CMAKE_BINARY_DIR}/src/ui
)
INCLUDE_DIRECTORIES(SYSTEM
  ${GDAL_INCLUDE_DIR}
  ${QCA_INCLUDE_DIR}
  ${QTKEYCHAIN_INCLUDE_DIR}
  ${GEOS_INCLUDE_DIR}
)

ADD_LIBRARY(wcsprovider MODULE ${WCS_SRCS} ${WCS_MOC_SRCS})

TARGET_LINK_LIBRARIES(wcsprovider
  qgis_core
  qgis_gui
)

INSTALL (TARGETS wcsprovider
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})
