# You can override default build options here

[build_ext]
include_dirs = @PYTHON_SWIG_INCLUDE@ 
library_dirs = @PYTHON_SWIG_LIBRARY@ 
libraries = @GDAL_LIB_NAME@ 
gdal_config=@GDAL_ROOT_BINARY_DIR@/gdal-config

