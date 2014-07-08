#!/bin/sh

CONFIG_LIBS="@CONFIG_LIBS_INS@"
CONFIG_DEP_LIBS="@LIBS@"
CONFIG_PREFIX="@INST_PREFIX@"
CONFIG_CFLAGS="-I@INST_INCLUDE@"
CONFIG_DATA="@GDAL_DATA@"
CONFIG_VERSION="@GDAL_VERSION@"
CONFIG_OGR_ENABLED="@OGR_ENABLED@"
CONFIG_FORMATS="@GDAL_FORMATS@"

usage()
{
	cat <<EOF
Usage: gdal-config [OPTIONS]
Options:
	[--prefix[=DIR]]
	[--libs]
	[--dep-libs]
	[--cflags]
	[--datadir]
	[--version]
	[--ogr-enabled]
	[--formats]
EOF
	exit $1
}

if test $# -eq 0; then
	usage 1 1>&2
fi

case $1 in 
  --libs)
    echo $CONFIG_LIBS
    ;;

  --dep-libs)
    echo $CONFIG_DEP_LIBS
    ;;

  --cflags)
    echo $CONFIG_CFLAGS
    ;;

  --datadir)
    echo $CONFIG_DATA
    ;;

  --prefix)
    echo $CONFIG_PREFIX
    ;;

  --version)
    echo $CONFIG_VERSION
    ;;

  --ogr-enabled)
    echo $CONFIG_OGR_ENABLED
    ;;

  --formats)
    echo $CONFIG_FORMATS
    ;;

  *)
    usage 1 1>&2
    ;;

esac

