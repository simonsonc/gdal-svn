# You can override default build options here

[build_ext]
include_dirs = @GDAL_ROOT_SOURCE_DIR@/port:@GDAL_ROOT_BINARY_DIR@/port:@GDAL_ROOT_SOURCE_DIR@/gcore:@GDAL_ROOT_SOURCE_DIR@/alg:@GDAL_ROOT_SOURCE_DIR@/ogr/ 
#library_dirs = ../../.libs:../../ 
libraries = gdal 
gdal_config=@GDAL_ROOT_BINARY_DIR@/gdal-config

