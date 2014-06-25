/******************************************************************************
 * $Id$
 *
 * Name:     gnmconnectivity.cpp
 * Project:  GDAL/OGR Geography Network support (Geographic Network Model)
 * Purpose:  GNMConnectivity class.
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

#include "gnm.h"


/************************************************************************/
/*                             Constructor                              */
/************************************************************************/
GNMConnectivity::GNMConnectivity()
{
    _poDataSet = NULL;
    _poSpatialRef = NULL;
    _meta_vrsn_i;
    _meta_srs_i = -1;
    _meta_gfidcntr_i = -1;
    _meta_name_i = -1;
    _meta_descr_i = -1;
    _ownDataset = false;
}

/************************************************************************/
/*                             Destructor                               */
/************************************************************************/
GNMConnectivity::~GNMConnectivity()
{
    if (_poSpatialRef != NULL) _poSpatialRef->Release();

    // We delete _poDataSet here if it is owned by the GNMConnectivity
    // object.
    if (_ownDataset) GDALClose(_poDataSet);
}

/************************************************************************/
/*                          _findConnection()                         */
/************************************************************************/
OGRFeature *GNMConnectivity::_findConnection(OGRLayer *l, GNMGFID s,
                                                  GNMGFID t, GNMGFID c)
{
    // TODO: it would be good to somehow use index on the graph layer
    // for fast searching.

    char q[100];
    // TODO: replace field names with defined constants.
    sprintf(q,"source = %ld and target = %ld and connector = %ld",s,t,c);
    l->SetAttributeFilter(q);
    l->ResetReading();
    OGRFeature *f = l->GetNextFeature();
    l->SetAttributeFilter(NULL);
    return f;
}


/************************************************************************/
/*                             _create()                                */
/************************************************************************/
/**
 * \brief Internal method to create the connectivity
 *
 * NOTE: Even if there is already a set of necessary layers or if there
 * is the connectivity in another format the special layers and fields
 * will be created anyway. Just try to open connectivity so guarantee not
 * to modify the dataset.
 *
 * NOTE: This method will read and modify any feature which is already
 * in the dataset (assign special identifiers). Pass the void dataset
 * so to import necessary features to the void connectivity in future.
 *
 * @param poDS ...
 * @param pszSrsInput It is necessary to set the SRS because newly created
 * or imported features must have the same one.
 * @param papszOptions ...
 * @return ...
 *
 * @since GDAL 2.0
 */
GNMErr GNMConnectivity::_create (GDALDataset *poDS,
                                 char *pszSrsInput,
                                 char **papszOptions)
{
    // Check for the supported vector formats.
    if (!IsDatasetFormatSupported(poDS))
        return GNMERR_FAILURE;

    // GNMConnectivity does not take ownership of the given dataset.
    _poDataSet = poDS;

/* -------------------------------------------------------------------- */
/*                    Check dataset and get its info                    */
/* -------------------------------------------------------------------- */

    if (_poDataSet->TestCapability(ODsCCreateLayer) == FALSE)
    {
        //CPLError
        return GNMERR_UNABLE_CREATE_SYSLAYER;
    }

    int classCount = _poDataSet->GetLayerCount();
    if (classCount > 0
        && _poDataSet->GetLayer(0)->TestCapability(OLCCreateField) == FALSE)
    {
        //CPLError
        return GNMERR_UNABLE_CREATE_SYSFIELD;
    }

    // Load all dataset layer names, while these layers become classes
    // in the context of connectivity/network.
    char **classes = NULL;
    for (int i=0; i<classCount; i++)
    {
        classes = CSLAddString(classes, _poDataSet->GetLayer(i)->GetName());
    }

/* -------------------------------------------------------------------- */
/*           Create the necessary system layers and fields              */
/* -------------------------------------------------------------------- */

    OGRLayer *newLayer;
    OGRFeature *feature;
    OGRFieldDefn newField1("",OFTInteger);
    OGRFieldDefn newField2("",OFTInteger);
    OGRFieldDefn newField3("",OFTInteger);
    OGRFieldDefn newField4("",OFTInteger);
    OGRFieldDefn newField5("",OFTInteger);
    OGRFieldDefn newField6("",OFTInteger);

// ---------------------------- Create meta layer --------------------- */

    newLayer = _poDataSet->CreateLayer(GNM_SYSLAYER_META, NULL, wkbNone, NULL);
    if (newLayer == NULL)
        return GNMERR_UNABLE_CREATE_SYSLAYER;

    newField1.Set(GNM_SYSFIELD_PARAMNAME, OFTString);
    newField2.Set(GNM_SYSFIELD_PARAMVALUE, OFTString);
    if(newLayer->CreateField(&newField1) != OGRERR_NONE
       || newLayer->CreateField(&newField2) != OGRERR_NONE)
    {
        //CPLError
        // TODO: delete layer.
        return GNMERR_UNABLE_CREATE_SYSLAYER;
    }

    // Write obligatory metaparameters.

    _poSpatialRef = new OGRSpatialReference();
    if (pszSrsInput == NULL
        || _poSpatialRef->SetFromUserInput(pszSrsInput) != OGRERR_NONE)
    {
        //CPLError
        delete _poSpatialRef;
        return GNMERR_FAILURE;
    }
    feature = OGRFeature::CreateFeature(newLayer->GetLayerDefn());
    feature->SetField(GNM_SYSFIELD_PARAMNAME, GNM_METAPARAM_SRS);
    feature->SetField(GNM_SYSFIELD_PARAMVALUE, pszSrsInput);
    if(newLayer->CreateFeature(feature) != OGRERR_NONE)
    {
        //CPLError
        // TODO: delete layer.
        return GNMERR_UNABLE_CREATE_SYSLAYER;
    }
    _meta_srs_i = feature->GetFID();
    OGRFeature::DestroyFeature(feature);  

    feature = OGRFeature::CreateFeature(newLayer->GetLayerDefn());
    feature->SetField(GNM_SYSFIELD_PARAMNAME, GNM_METAPARAM_VERSION);
    feature->SetField(GNM_SYSFIELD_PARAMVALUE, GNM_VERSION);
    if(newLayer->CreateFeature(feature) != OGRERR_NONE)
    {
        //CPLError
        // TODO: delete layer.
        return GNMERR_UNABLE_CREATE_SYSLAYER;
    }
    _meta_vrsn_i = feature->GetFID();
    OGRFeature::DestroyFeature(feature);

    OGRFeature *featureGfid = OGRFeature::CreateFeature(newLayer->GetLayerDefn());
    featureGfid->SetField(GNM_SYSFIELD_PARAMNAME, GNM_METAPARAM_GFIDCNT);
    if(newLayer->CreateFeature(feature) != OGRERR_NONE)
    {
        //CPLError
        // TODO: delete layer.
        return GNMERR_UNABLE_CREATE_SYSLAYER;
    }
    _meta_gfidcntr_i = feature->GetFID();
    OGRLayer *layerGfid = newLayer; // For future use.

    feature = OGRFeature::CreateFeature(newLayer->GetLayerDefn());
    feature->SetField(GNM_SYSFIELD_PARAMNAME, GNM_METAPARAM_NAME);
    const char *conName = CSLFetchNameValue(papszOptions, GNM_CREATE_OPTIONPAIR_NAME);
    if (conName != NULL)
        feature->SetField(GNM_SYSFIELD_PARAMVALUE, conName);
    else // default name
        feature->SetField(GNM_SYSFIELD_PARAMVALUE, "_gnm_connectivity");
    if(newLayer->CreateFeature(feature) != OGRERR_NONE)
    {
        //CPLError
        // TODO: delete layer.
        return GNMERR_UNABLE_CREATE_SYSLAYER;
    }
    _meta_name_i = feature->GetFID();
    OGRFeature::DestroyFeature(feature);

    // Write additional metaparameters.

    const char *conDescr = CSLFetchNameValue(papszOptions, GNM_CREATE_OPTIONPAIR_DESCR);
    if (conDescr != NULL)
    {
        feature = OGRFeature::CreateFeature(newLayer->GetLayerDefn());
        feature->SetField(GNM_SYSFIELD_PARAMNAME, GNM_METAPARAM_DESCR);
        feature->SetField(GNM_SYSFIELD_PARAMVALUE, conDescr);
        if(newLayer->CreateFeature(feature) != OGRERR_NONE)
        {
            // CPL Warning
        }
        _meta_descr_i = feature->GetFID();
        OGRFeature::DestroyFeature(feature);
    }

/* ---------------------------- Create graph layer --------------------- */
    // NOTE: until any connect method is called the graph will be void.

    newLayer = _poDataSet->CreateLayer(GNM_SYSLAYER_GRAPH,NULL,wkbNone,NULL);
    if (newLayer == NULL)
    {
        //CPLError
        return GNMERR_UNABLE_CREATE_SYSLAYER;
    }

    newField1.Set(GNM_SYSFIELD_SOURCE, OFTInteger);
    newField2.Set(GNM_SYSFIELD_TARGET, OFTInteger);
    newField3.Set(GNM_SYSFIELD_CONNECTOR, OFTInteger);
    newField4.Set(GNM_SYSFIELD_COST, OFTReal);
    newField5.Set(GNM_SYSFIELD_INVCOST, OFTReal);
    newField6.Set(GNM_SYSFIELD_DIRECTION, OFTInteger);
    if(newLayer->CreateField(&newField1) != OGRERR_NONE
       || newLayer->CreateField(&newField2) != OGRERR_NONE
       || newLayer->CreateField(&newField3) != OGRERR_NONE
       || newLayer->CreateField(&newField4) != OGRERR_NONE
       || newLayer->CreateField(&newField5) != OGRERR_NONE
       || newLayer->CreateField(&newField6) != OGRERR_NONE)
    {
        //CPLError
        // TODO: delete created system layers.
        return GNMERR_UNABLE_CREATE_SYSLAYER;
    }

/* ---------------------------- Create classes layer --------------------- */

    newLayer = _poDataSet->CreateLayer(GNM_SYSLAYER_CLASSES,NULL,wkbNone,NULL);
    if (newLayer == NULL)
    {
        //CPLError
        return GNMERR_UNABLE_CREATE_SYSLAYER;
    }

    newField1.Set(GNM_SYSFIELD_LAYERNAME, OFTString);
    if(newLayer->CreateField(&newField1) != OGRERR_NONE)
    {
        //CPLError
        // TODO: delete created system layers.
        return GNMERR_UNABLE_CREATE_SYSLAYER;
    }

    // Write all non system layers in a dataset to the classes layer.
    for (int i=0; i<classCount; i++)
    {
        OGRFeature *feature;
        feature = OGRFeature::CreateFeature(newLayer->GetLayerDefn());
        feature->SetField(GNM_SYSFIELD_LAYERNAME, CSLGetField(classes,i)); // The name must be copied.
        if(newLayer->CreateFeature(feature) != OGRERR_NONE)
        {
            //CPLError
            // TODO: delete created system layers.
            return GNMERR_UNABLE_CREATE_SYSLAYER;
        }
        OGRFeature::DestroyFeature(feature);
    }

/* -------------------------------------------------------------------- */
/*          Add system fields and transform their SRS if needed         */
/* -------------------------------------------------------------------- */

    // Assign GFIDs to the existing dataset features.
    GNMGFID gfidCounter = 0;
    for (int i=0; i<classCount; i++)
    {
        // We fetch all layers and all features.
        // As we assume that the underlying dataset have no connectivity,
        // we does not check if the gfid field already exists. We create
        // new connectivity so we must assign gfids from the begining and
        // not use any old gfid data.
        OGRLayer *curLayer = _poDataSet->GetLayerByName(CSLGetField(classes,i)); // The layer will exist
        newField1.Set(GNM_SYSFIELD_GFID, OFTInteger);
        if(curLayer->CreateField(&newField1) != OGRERR_NONE)
        {
            //CPLError
            // TODO: delete created system layers and all created fields.
            return GNMERR_UNABLE_CREATE_SYSFIELD;
        }

        curLayer->ResetReading();
        OGRFeature *feature;
        while ((feature = curLayer->GetNextFeature()) != NULL)
        {
            feature->SetField(GNM_SYSFIELD_GFID, gfidCounter);
            gfidCounter++;
            curLayer->SetFeature(feature);

            // TODO: transform the coordinates of feature (if the according
            // option is not set???)

            OGRFeature::DestroyFeature(feature);
        }
    }

    // TODO: save id counter to the variable or directly to _gnm_meta.

/* -------------------------------------------------------------------- */
/*            Save changes and free resources in dataset                */
/* -------------------------------------------------------------------- */

    // Save gfid counter.
    featureGfid->SetField(GNM_SYSFIELD_PARAMVALUE, gfidCounter);
    layerGfid->SetFeature(featureGfid);
    OGRFeature::DestroyFeature(featureGfid);

    CSLDestroy(classes);

    // TODO: Do we need FlushCache here?
    _poDataSet->FlushCache();

    return GNMERR_NONE;
}


/************************************************************************/
/*                             _open()                                  */
/************************************************************************/
GNMErr GNMConnectivity::_open(GDALDataset *poDS)
{
    // NOTE: The supported dataset was checked during HasConnectivity.

    _poDataSet = poDS;
    OGRLayer* layer = _poDataSet->GetLayerByName(GNM_SYSLAYER_META);
    OGRFeature *feature;

    // Load or check metadata parameters.
    layer->ResetReading();
    while ((feature = layer->GetNextFeature()) != NULL)
    {
        const char *paramName = feature->GetFieldAsString(GNM_SYSFIELD_PARAMNAME);
        const char *paramValue = NULL;
        paramValue = feature->GetFieldAsString(GNM_SYSFIELD_PARAMVALUE);

        if (strcmp(paramName,GNM_METAPARAM_VERSION) == 0
            && paramValue != NULL
            && strcmp(paramValue,"") != 0)
        {
            // TODO: somehow check the version as regular expression: X.X.X
            // TODO: Check if GNM version is equal to opening connectivity version.
            _meta_vrsn_i = feature->GetFID();
        }

        else if (strcmp(paramName,GNM_METAPARAM_SRS) == 0)
        {
            // Load SRS.
            _poSpatialRef = new OGRSpatialReference();
            if (paramValue == NULL
                || _poSpatialRef->SetFromUserInput(paramValue) != OGRERR_NONE)
            {
                delete _poSpatialRef;
                OGRFeature::DestroyFeature(feature);
                //CPLError
                return GNMERR_FAILURE;
            }
            _meta_srs_i = feature->GetFID();
        }

        else if (strcmp(paramName,GNM_METAPARAM_GFIDCNT) == 0)
        {
            // TODO: Check if the counter < 0.
            _meta_gfidcntr_i = feature->GetFID();
        }

        else if (strcmp(paramName,GNM_METAPARAM_NAME) == 0)
        {
            _meta_name_i = feature->GetFID();
        }

        else if (strcmp(paramName,GNM_METAPARAM_DESCR) == 0)
        {
            _meta_descr_i = feature->GetFID();
        }

        OGRFeature::DestroyFeature(feature);
    }

    return GNMERR_NONE;
}


/************************************************************************/
/*                             _remove()                                */
/************************************************************************/
/**
 * \brief Internal method to remove the connectivity
 *
 *
 * @since GDAL 2.0
 */
GNMErr GNMConnectivity::_remove(GDALDataset *poDS)
{
    if (poDS->TestCapability(ODsCDeleteLayer) == FALSE)
    {
        //CPLError
        return GNMERR_FAILURE;
    }

    if (poDS->GetLayerByName(GNM_SYSLAYER_META)
                  ->TestCapability(OLCDeleteField) == FALSE)
    {
        //CPLError
        return GNMERR_FAILURE;
    }

    // Delete system layers.
    int k = 0;
    do
    {
        int cnt = poDS->GetLayerCount();
        for (int i=0; i<cnt; i++)
        {
            // We have to find system layers this way, because there is
            // no capability to effectivly restore the layer id.
            OGRLayer *layer = poDS->GetLayer(i);
            if (strcmp(layer->GetName(),GNM_SYSLAYER_META) == 0
                || strcmp(layer->GetName(),GNM_SYSLAYER_GRAPH) == 0
                || strcmp(layer->GetName(),GNM_SYSLAYER_CLASSES) == 0)
            {
                poDS->DeleteLayer(i);
                k++;
                break;
            }
        }
    }
    while (k < GNM_CON_SYSLAYERS_COUNT);
    // TODO: make warnings if the layers have not been found.

    // Delete system fields.
    int count = poDS->GetLayerCount();
    for (int i=0; i<count; i++)
    {
        OGRLayer *layer = poDS->GetLayer(i);
        int ind = layer->FindFieldIndex(GNM_SYSFIELD_GFID,TRUE);
        if (ind != -1
            && layer->GetLayerDefn()->GetFieldDefn(ind)->GetType() == OFTInteger)
            layer->DeleteField(ind);
    }
    // TODO: make warnings if the fields have not been found.

    return GNMERR_NONE;
}


/************************************************************************/
/*                     _has()                               */
/************************************************************************/
bool GNMConnectivity::_has (GDALDataset *poDS)
{
    // Check for the supported vector formats.
    if (!IsDatasetFormatSupported(poDS))
        return false;

    // Check the existance of system layers and their correct fields.

    OGRLayer *layer;
    layer = poDS->GetLayerByName(GNM_SYSLAYER_META);
    if (layer == NULL
        || layer->GetLayerDefn()->GetFieldIndex(GNM_SYSFIELD_PARAMNAME) == -1
        || layer->GetLayerDefn()->GetFieldIndex(GNM_SYSFIELD_PARAMVALUE) == -1)
    {
        return false;
    }
    else
    {
        // Check for the obligated metadata.
        bool hasVersion = false;
        bool hasSRS = false;
        bool hasIdCounter = false;
        bool hasName = false;
        OGRFeature *feature;
        layer->ResetReading();
        while ((feature = layer->GetNextFeature()) != NULL)
        {
            const char *paramName = feature->GetFieldAsString(GNM_SYSFIELD_PARAMNAME);
            if (strcmp(paramName,GNM_METAPARAM_SRS) == 0) hasSRS = true;
            else if (strcmp(paramName,GNM_METAPARAM_VERSION) == 0) hasVersion = true;
            else if (strcmp(paramName,GNM_METAPARAM_GFIDCNT) == 0) hasIdCounter = true;
            else if (strcmp(paramName,GNM_METAPARAM_NAME) == 0) hasName = true;
            OGRFeature::DestroyFeature(feature);
        }
        if (!hasSRS || !hasVersion || !hasIdCounter || !hasName)
            return false;
    }

    layer = poDS->GetLayerByName(GNM_SYSLAYER_GRAPH);
    if (layer == NULL
        || layer->GetLayerDefn()->GetFieldIndex(GNM_SYSFIELD_SOURCE) == -1
        || layer->GetLayerDefn()->GetFieldIndex(GNM_SYSFIELD_TARGET) == -1
        || layer->GetLayerDefn()->GetFieldIndex(GNM_SYSFIELD_CONNECTOR) == -1
        || layer->GetLayerDefn()->GetFieldIndex(GNM_SYSFIELD_COST) == -1
        || layer->GetLayerDefn()->GetFieldIndex(GNM_SYSFIELD_INVCOST) == -1
        || layer->GetLayerDefn()->GetFieldIndex(GNM_SYSFIELD_DIRECTION) == -1)
    {
        return false;
    }

    layer = poDS->GetLayerByName(GNM_SYSLAYER_CLASSES);
    if (layer == NULL
        || layer->GetLayerDefn()->GetFieldIndex(GNM_SYSFIELD_LAYERNAME) == -1)
    {
        return false;
    }

    // Check if all classes have the gfid field.
    int classCount = poDS->GetLayerCount();
    for (int i=0; i<classCount; i++)
    {
        layer = poDS->GetLayer(i);
        if (strcmp(layer->GetName(),GNM_SYSLAYER_META) != 0
            && strcmp(layer->GetName(),GNM_SYSLAYER_GRAPH) != 0
            && strcmp(layer->GetName(),GNM_SYSLAYER_CLASSES) != 0
            && layer->GetLayerDefn()->GetFieldIndex(GNM_SYSFIELD_GFID) == -1)
        {
            return false;
        }
    }

    return true;
}


/************************************************************************/
/*                        FlushCache()                               */
/************************************************************************/
void GNMConnectivity::FlushCache ()
{
    _poDataSet->FlushCache();

    // TODO (?): save gfidCounter?
}


/************************************************************************/
/*                        GetMetaParamValues()                          */
/************************************************************************/
/**
 * \brief Returns all metaparams as a CSL Name-Value array of pairs. The
 * parameter names see in the gnm.h defines
 *
 * NOTE: Use CSLDestroy to delete returned string list
 *
 * @since GDAL 2.0
 */
char **GNMConnectivity::GetMetaParamValues ()
{
    char **ret = NULL;

    OGRLayer *layer = _poDataSet->GetLayerByName(GNM_SYSLAYER_META);
    OGRFeature *feature;
    layer->ResetReading();
    while ((feature = layer->GetNextFeature()) != NULL)
    {
        ret = CSLAddNameValue(ret,
                        feature->GetFieldAsString(GNM_SYSFIELD_PARAMNAME),
                        feature->GetFieldAsString(GNM_SYSFIELD_PARAMVALUE));
        OGRFeature::DestroyFeature(feature);
    }

    return ret;
}


/************************************************************************/
/*                       CreateLayer()                               */
/************************************************************************/
/**
 * \brief Creates layer
 *
 * NOTE: This method is based on GDALDataset::CreateLayer()
 *
 * @since GDAL 2.0
 */
GNMErr GNMConnectivity::CreateLayer (const char *pszName,
                                        OGRFeatureDefn *FeatureDefn,
                                        OGRwkbGeometryType eGType,
                                        char **papszOptions)
{
    // FORMAT NOTE: some datasets does not support names with "-" symbol.

    // Create void layer in a dataset.
    OGRLayer *newLayer;
    newLayer = _poDataSet->CreateLayer(pszName,_poSpatialRef,eGType,papszOptions);
    if (newLayer == NULL)
    {
        //CPLErr
        return GNMERR_FAILURE;
    }

    // Create fields for the layer.
    int count = FeatureDefn->GetFieldCount();
    for (int i=0; i<count; i++)
    {
        if (newLayer->CreateField(FeatureDefn->GetFieldDefn(i)) != OGRERR_NONE)
        {
            //CPLError
            // TODO: delete layer.
            return GNMERR_FAILURE;
        }
    }

    // Add system fields.
    // Check if the passed layer definition has the gfid field already.
    int ix = FeatureDefn->GetFieldIndex(GNM_SYSFIELD_GFID);
    OGRFieldDefn *fieldDefn = FeatureDefn->GetFieldDefn(ix);
    if (fieldDefn == NULL)
    {
        OGRFieldDefn newField(GNM_SYSFIELD_GFID,OFTInteger);
        if(newLayer->CreateField(&newField) != OGRERR_NONE)
        {
            //CPLError
            // TODO: delete layer.
            return GNMERR_FAILURE;
        }
    }
    else
    {
        // Assumed that user should firstly delete not-integer gfid column.
        if (fieldDefn->GetType() != OFTInteger)
        {
            //CPLError
            // TODO: delete layer.
            return GNMERR_FAILURE;
        }
    }

    // Register layer in the according system table.
    OGRLayer *layer = _poDataSet->GetLayerByName(GNM_SYSLAYER_CLASSES);
    OGRFeature *feature = OGRFeature::CreateFeature(layer->GetLayerDefn());
    feature->SetField(GNM_SYSFIELD_LAYERNAME, pszName);
    if(layer->CreateFeature(feature) != OGRERR_NONE)
    {
        OGRFeature::DestroyFeature(feature);
        //CPLError
        // TODO: delete layer.
        return GNMERR_FAILURE;
    }
    OGRFeature::DestroyFeature(feature);

    return GNMERR_NONE;
}


/************************************************************************/
/*                       CopyLayer()                               */
/************************************************************************/
/**
 * \brief Copies the layer, transforming its features' coordinates to the
 * connectivity's common one (if it is a layer from an external dataset).
 *
 * Useful when the connectivity has been created over the void dataset and
 * there is a need to import features.
 *
 * NOTE: This method does not use the GDALDataset::CopyLayer()
 *
 * @since GDAL 2.0
 */
GNMErr GNMConnectivity::CopyLayer (OGRLayer *poSrcLayer, const char *pszNewName)
{
    if(poSrcLayer == NULL || pszNewName == NULL)
    {
        //CPLErr
        return GNMERR_FAILURE;
    }

    // TODO: Ensure that the passed layer is not the system layer of
    // this connectivity (simply by name).

    // TODO: Check if this layer has a unique for the connectivity name.
    //setAttributeFilter()

    // TODO (?): construct the special unique name for new layer, accordingly
    // to the special naming conventions?

    // Create new layer in a connectivity.
    OGRFeatureDefn *fDefn = poSrcLayer->GetLayerDefn();
    if (this->CreateLayer(pszNewName, fDefn, poSrcLayer->GetGeomType()) != GNMERR_NONE)
    {
        //CPLErr
        return GNMERR_FAILURE;
    }

    // Prepare the coordinate transformation object.
    bool allowTransformation = false;
    OGRSpatialReference *sourceSRS = poSrcLayer->GetSpatialRef();
    OGRCoordinateTransformation *transform;
    if (sourceSRS != NULL)
    {
        transform = OGRCreateCoordinateTransformation(sourceSRS, _poSpatialRef);
        if (transform != NULL)
            allowTransformation = true;
    }

    // Copy all features.
    OGRLayer *gfidLayer = _poDataSet->GetLayerByName(GNM_SYSLAYER_META);
    OGRFeature *gfidFeature = gfidLayer->GetFeature(_meta_gfidcntr_i);
    GNMGFID gfidCounter = gfidFeature->GetFieldAsInteger(GNM_SYSFIELD_PARAMVALUE);
    OGRLayer *newLayer = _poDataSet->GetLayerByName(pszNewName);
    int count = fDefn->GetFieldCount();
    OGRFeature *sourceFeature;
    poSrcLayer->ResetReading();
    while((sourceFeature = poSrcLayer->GetNextFeature()) != NULL)
    {
        // Create new feature with new layer definition.
        OGRFeature *newFeature = OGRFeature::CreateFeature(newLayer->GetLayerDefn());
        if (newFeature == NULL)
        {
            //CPLErr
            // TODO: Delete all features copied earlier?
            return GNMERR_FAILURE;
        }

        // Get the geometry from the source feature. Transform it if
        // the feature exists and a transformation object has been defined.
        OGRGeometry *newGeom = sourceFeature->StealGeometry();
        if (newGeom != NULL)
        {
            if (allowTransformation)
                newGeom->transform(transform);
            newFeature->SetGeometryDirectly(newGeom);
        }

        // Copy source feature fid.
        newFeature->SetFID(sourceFeature->GetFID());

        // Assign new gfid.
        newFeature->SetField(GNM_SYSFIELD_GFID, gfidCounter);
        gfidCounter++;

        // Copy source feature attribute values.
        for (int i = 0; i < count; i++)
        {
            if (sourceFeature->IsFieldSet(i) == TRUE)
            {
                OGRFieldType fType = fDefn->GetFieldDefn(i)->GetType();
                const char *fName = fDefn->GetFieldDefn(i)->GetNameRef();
                switch (fType)
                {
                    case OFTInteger:
                        newFeature->SetField(fName, sourceFeature->GetFieldAsInteger(i));
                    break;
                    case OFTReal:
                        // TODO: understand why several fields like ..\..\data\belarus ->
                        // field OSM_ID are copied incorrectly (the values extends the
                        // type maximum value).
                        newFeature->SetField(fName, sourceFeature->GetFieldAsDouble(i));
                    break;
                    default:
                        newFeature->SetField(fName, sourceFeature->GetFieldAsString(i));
                    break;
                    // TODO: widen this list.
                    //...
                }
            }
        }

        // Add a feature to the new layer.
        if(newLayer->CreateFeature(newFeature) != OGRERR_NONE)
        {
            //CPLErr
            OGRFeature::DestroyFeature(newFeature);
            OGRFeature::DestroyFeature(sourceFeature);
            if (allowTransformation) OCTDestroyCoordinateTransformation(transform);
            return GNMERR_FAILURE;
        }

        OGRFeature::DestroyFeature(newFeature);
        OGRFeature::DestroyFeature(sourceFeature);
    }

    // Save gfid counter.
    gfidFeature->SetField(GNM_SYSFIELD_PARAMVALUE,gfidCounter);
    gfidLayer->SetFeature(gfidFeature);
    OGRFeature::DestroyFeature(gfidFeature);

    if (allowTransformation) OCTDestroyCoordinateTransformation(transform);

    return GNMERR_NONE;
}


/************************************************************************/
/*                     IsDatasetFormatSupported()                               */
/************************************************************************/
bool GNMConnectivity::IsDatasetFormatSupported (GDALDataset *poDS)
{
    // Check for the supported vector formats.
    const char *drName = poDS->GetDriverName();
    int i = 0;
    while(TRUE)
    {
        if (GNMSupportedDatasets[i] == NULL)
            return false;
        else if (strcmp(drName, GNMSupportedDatasets[i]) == 0)
            return true;
        i++;
    }
}


/************************************************************************/
/*                     GetDataset()                               */
/************************************************************************/
GDALDataset *GNMConnectivity::GetDataset ()
{
    return _poDataSet;
}


/************************************************************************/
/*                     ConnectFeatures()                               */
/************************************************************************/
GNMErr GNMConnectivity::ConnectFeatures(GNMGFID nSourceGFID,
                                        GNMGFID nTargetGFID,
                                        GNMGFID nConnectorGFID,
                                        double dCost,
                                        double dInvCost,
                                        GNMDirection dir)
{
    // TODO: Check if the features with these gfids exist.

    // Set direction if incorrect.
    if (dir != GNM_DIR_DOUBLE
        && dir != GNM_DIR_SRCTOTGT
        && dir != GNM_DIR_TGTTOSRC)
        dir = GNM_DIR_DOUBLE;

    OGRLayer* layer = _poDataSet->GetLayerByName(GNM_SYSLAYER_GRAPH);

    // There must not be a connection with the same source, target and
    // connector. Check it.
    OGRFeature *feature = _findConnection(layer,
                                          nSourceGFID,
                                          nTargetGFID,
                                          nConnectorGFID);
    if (feature != NULL)
    {
        OGRFeature::DestroyFeature(feature);
        //CPLErr
        return GNMERR_FAILURE;
    }
    OGRFeature::DestroyFeature(feature);

    // If the direction is bidirected we must check if there is a target-source
    // equivalent of this connection.
    if (dir == GNM_DIR_DOUBLE)
    {
        OGRFeature *feature = _findConnection(layer,
                                              nTargetGFID,
                                              nSourceGFID,
                                              nConnectorGFID);
        if (feature != NULL)
        {
            OGRFeature::DestroyFeature(feature);
            //CPLErr
            return GNMERR_FAILURE;
        }
        OGRFeature::DestroyFeature(feature);
    }

    // TODO: Form system vertexes/edges if the according options are set.

    // Add connection.
    feature = OGRFeature::CreateFeature(layer->GetLayerDefn());
    feature->SetField(GNM_SYSFIELD_SOURCE, nSourceGFID);
    feature->SetField(GNM_SYSFIELD_TARGET, nTargetGFID);
    feature->SetField(GNM_SYSFIELD_CONNECTOR, nConnectorGFID);
    feature->SetField(GNM_SYSFIELD_COST, dCost);
    feature->SetField(GNM_SYSFIELD_INVCOST, dInvCost);
    feature->SetField(GNM_SYSFIELD_DIRECTION, dir);
    if(layer->CreateFeature(feature) != OGRERR_NONE)
    {
        OGRFeature::DestroyFeature(feature);
        return GNMERR_FAILURE;
    }
    OGRFeature::DestroyFeature(feature);

    return GNMERR_NONE;
}


/************************************************************************/
/*                     DisconnectFeatures()                               */
/************************************************************************/
GNMErr GNMConnectivity::DisconnectFeatures(GNMGFID nSourceGFID,
                                           GNMGFID nTargetGFID,
                                           GNMGFID nConnectorGFID)
{

    OGRLayer* layer = _poDataSet->GetLayerByName(GNM_SYSLAYER_GRAPH);

    // Find connection and delete it from the graph.
    OGRFeature *feature = _findConnection(layer,
                                          nSourceGFID,
                                          nTargetGFID,
                                          nConnectorGFID);
    if (feature == NULL)
    {
        OGRFeature::DestroyFeature(feature);
        //CPLErr
        return GNMERR_FAILURE;
    }
    long fid = feature->GetFID();
    OGRFeature::DestroyFeature(feature);

    if (layer->DeleteFeature(fid) != OGRERR_NONE)
    {
        //CPLErr
        return GNMERR_FAILURE;
    }

    return GNMERR_NONE;
}


/************************************************************************/
/*                     ReconnectFeatures() #1                              */
/************************************************************************/
GNMErr GNMConnectivity::ReconnectFeatures (GNMGFID nSourceGFID,
                                  GNMGFID nTargetGFID,
                                  GNMGFID nConnectorGFID,
                                  GNMDirection newDir)
{
    OGRLayer* layer = _poDataSet->GetLayerByName(GNM_SYSLAYER_GRAPH);

    // Find connection and delete it from the graph.
    OGRFeature *feature = _findConnection(layer,
                                          nSourceGFID,
                                          nTargetGFID,
                                          nConnectorGFID);
    if (feature == NULL)
    {
        OGRFeature::DestroyFeature(feature);
        //CPLErr
        return GNMERR_FAILURE;
    }

    // Set direction if incorrect.
    if (newDir != GNM_DIR_DOUBLE
        && newDir != GNM_DIR_SRCTOTGT
        && newDir != GNM_DIR_TGTTOSRC)
        newDir = GNM_DIR_DOUBLE;

    feature->SetField(GNM_SYSFIELD_DIRECTION,newDir);
    layer->SetFeature(feature);
    OGRFeature::DestroyFeature(feature);

    return GNMERR_NONE;
}


/************************************************************************/
/*                     ReconnectFeatures() #2                              */
/************************************************************************/
GNMErr GNMConnectivity::ReconnectFeatures (GNMGFID nSourceGFID,
                                  GNMGFID nTargetGFID,
                                  GNMGFID nConnectorGFID,
                                  double dNewCost,
                                  double dNewInvCost)
{
    OGRLayer* layer = _poDataSet->GetLayerByName(GNM_SYSLAYER_GRAPH);

    // Find connection and delete it from the graph.
    OGRFeature *feature = _findConnection(layer,
                                          nSourceGFID,
                                          nTargetGFID,
                                          nConnectorGFID);
    if (feature == NULL)
    {
        OGRFeature::DestroyFeature(feature);
        //CPLErr
        return GNMERR_FAILURE;
    }

    feature->SetField(GNM_SYSFIELD_COST,dNewCost);
    feature->SetField(GNM_SYSFIELD_INVCOST,dNewInvCost);
    layer->SetFeature(feature);
    OGRFeature::DestroyFeature(feature);

    return GNMERR_NONE;
}



