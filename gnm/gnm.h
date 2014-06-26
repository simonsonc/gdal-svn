/******************************************************************************
 * $Id$
 *
 * Name:     gnm.h
 * Project:  GDAL/OGR Geography Network support (Geographic Network Model)
 * Purpose:  GNM declarations.
 * Author:   Mikhail Gusev (gusevmihs at gmail dot com)
 *
 ******************************************************************************
 * Copyright (c) 2014, Mikhail Gusev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef _GNM_H_INCLUDED
#define _GNM_H_INCLUDED

#include "ogrsf_frmts.h"

// Supported GDAL formats for connectivities.
// FORMAT NOTE: the connectivity will not be created for those formats,
// which does not appear in this list, while there are some issues which
// which can cause an incorrect work of connectivity.
static const char *GNMSupportedDatasets[] = {
    "ESRI Shapefile",
    NULL };

// General constants.
#define GNM_VERSION "1.0.0"

// Types.
typedef int GNMGFID; // Alias for long. Can be replaced with GUIntBig.

// Error codes.
typedef int GNMErr;
#define GNMERR_NONE 0
#define GNMERR_FAILURE 1
#define GNMERR_UNABLE_CREATE_SYSLAYER 2
#define GNMERR_UNABLE_CREATE_SYSFIELD 3
//#define GNMERR_UNSUPPORTED_BEHAVIOR 4

// Obligatory system layers.
#define GNM_CON_SYSLAYERS_COUNT 3
#define GNM_SYSLAYER_META "_gnm_meta"
#define GNM_SYSLAYER_GRAPH "_gnm_graph"
#define GNM_SYSLAYER_CLASSES "_gnm_classes"

// Direction of an edge.
typedef int GNMDirection;
#define GNM_DIR_DOUBLE 0 // double-directed
#define GNM_DIR_SRCTOTGT 1 // from source to target
#define GNM_DIR_TGTTOSRC 2 // from target to source

// Names of options in the pair name & value, which can be passed
// when the connectivity is being created.
//#define GNM_CREATE_OPTION_NOTTRANSFORMALL "not_transform_all"
//#define GNM_CREATE_OPTIONPAIR_ALIAS "con_alias"
#define GNM_CREATE_OPTIONPAIR_NAME "con_name"
#define GNM_CREATE_OPTIONPAIR_DESCR "con_descr"

// Connectivity metadata parameter names.
#define GNM_METAPARAM_VERSION "gnm_version"
#define GNM_METAPARAM_SRS "common_srs"
#define GNM_METAPARAM_GFIDCNT "gfid_counter" // the current is that we should assign
#define GNM_METAPARAM_NAME "con_name"
#define GNM_METAPARAM_DESCR "con_descr" // connectivity description

// System field names.
// FORMAT NOTE: Shapefile driver does not support field names more than 10 characters.
#define GNM_SYSFIELD_PARAMNAME "param_name"
#define GNM_SYSFIELD_PARAMVALUE "param_val"
#define GNM_SYSFIELD_SOURCE "source"
#define GNM_SYSFIELD_TARGET "target"
#define GNM_SYSFIELD_CONNECTOR "connector"
#define GNM_SYSFIELD_COST "cost"
#define GNM_SYSFIELD_INVCOST "inv_cost"
#define GNM_SYSFIELD_DIRECTION "direction"
#define GNM_SYSFIELD_GFID "gfid"
#define GNM_SYSFIELD_LAYERNAME "layer_name"
//#define GNM_SYSFIELD_FID "fid"

	
/************************************************************************/
/*                      GNMConnectivity                               */
/************************************************************************/
class CPL_DLL GNMConnectivity
{ 
    friend class GNMManager;

 // Fields:

    private:

    GDALDataset *_poDataSet;
    long _meta_vrsn_i;
    long _meta_srs_i;
    long _meta_gfidcntr_i;
    long _meta_name_i;
    long _meta_descr_i;
    bool _ownDataset;

    protected:

    OGRSpatialReference *_poSpatialRef;

 // Additional:

    OGRFeature *_findConnection (OGRLayer *l,GNMGFID s,GNMGFID t,GNMGFID c);

    bool _isClassLayer (const char *layerName);

    char *_makeNewLayerName (const char *name, OGRwkbGeometryType geotype);

 // Interface:

    private:

    virtual GNMErr _create (GDALDataset *poDS,
                            char *pszSrsInput,
                            char **papszOptions = NULL);

    virtual GNMErr _open(GDALDataset *poDS);

    static GNMErr _remove (GDALDataset *poDS);

    static bool _has (GDALDataset *poDS);

    public:

    GNMConnectivity();
    ~GNMConnectivity();

 // Additional methods:

    static bool IsDatasetFormatSupported (GDALDataset *poDS);

    GDALDataset *GetDataset ();

    void FlushCache ();

    char **GetMetaParamValues ();

    const OGRSpatialReference* GetSpatialReference() const;

 // Interface for reimplementing in subclasses:

    virtual OGRLayer *CreateLayer (const char *pszName,
                             OGRFeatureDefn *FeatureDefn,
                             OGRwkbGeometryType eGType = wkbPoint,
                             char **papszOptions = NULL);

    virtual GNMErr CopyLayer (OGRLayer *poSrcLayer,
                                 const char *pszNewName);

    virtual GNMErr ConnectFeatures (GNMGFID nSourceGFID,
                                    GNMGFID nTargetGFID,
                                    GNMGFID nConnectorGFID,
                                    double dCost,
                                    double dInvCost,
                                    GNMDirection dir);

    virtual GNMErr DisconnectFeatures (GNMGFID nSourceGFID,
                                       GNMGFID nTargetGFID,
                                       GNMGFID nConnectorGFID);

    virtual GNMErr ReconnectFeatures (GNMGFID nSourceGFID,
                                      GNMGFID nTargetGFID,
                                      GNMGFID nConnectorGFID,
                                      GNMDirection newDir);

    virtual GNMErr ReconnectFeatures (GNMGFID nSourceGFID,
                                      GNMGFID nTargetGFID,
                                      GNMGFID nConnectorGFID,
                                      double dNewCost,
                                      double dNewInvCost);
};


/************************************************************************/
/*                      GNMManager                               */
/************************************************************************/
/**
 * \brief Provides an abstraction to manage the connectivities. This class
 * decides which connectivity format exactly to manipulate, while user
 * works only with GNMConnectivity.
 *
 * @since GDAL 2.0
 */
class CPL_DLL GNMManager
{
    public:

    static GNMConnectivity *CreateConnectivity (const char *pszName,
                                                    const char *pszFormat,
                                                    char *pszSrsInput,
                                                    char **papszConOptions = NULL,
                                                    char **papszDatasetOptions = NULL);

    static GNMConnectivity *CreateConnectivity (GDALDataset *poDS,
                                                char *pszSrsInput,
                                                char **papszOptions = NULL,
                                                int bNative = FALSE);

    static GNMConnectivity *OpenConnectivity (GDALDataset *poDS,
                                              int bNative = FALSE);

    static void CloseConnectivity (GNMConnectivity *poCon);

    static GNMErr RemoveConnectivity (GDALDataset *poDS,
                                      int bNative);

    static bool HasConnectivity (GDALDataset *poDS,
                                 int bNative);
};

#endif // _GNM_H_INCLUDED
