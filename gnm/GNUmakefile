
include ../GDALmake.opt

OBJ	=	gnmconnectivity.o gnmmanager.o

CPPFLAGS	:=	$(GDAL_INCLUDE) $(CPPFLAGS)  -I../ogr -I../ogr/ogrsf_frmts -I../gcore


default:	$(OBJ:.o=.$(OBJ_EXT))

$(OBJ):	gnm.h

clean:
	$(RM) *.o $(O_OBJ)

docs:	
	(cd ..; $(MAKE) docs)

#gdal_misc.$(OBJ_EXT):	gdal_misc.cpp gdal_version.h

install:
	for f in *.h ; do $(INSTALL_DATA) $$f $(DESTDIR)$(INST_INCLUDE) ; done


