/******************************************************************************
 * $Id$
 *
 * Name:     gnmconnectivity.cpp
 * Project:  GDAL/OGR Geography Network support (Geographic Network Model)
 * Purpose:  GNMManager class.
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
/*                      CreateVoidConnectivity()                            */
/************************************************************************/
/**
 * \brief Convenient method to create a connectivity on an instantly
 * created void dataset
 *
 * NOTE: the created GNMConnectivity must be freed via GNMManager::
 * CloseConnectivity()
 * NOTE: the created according dataset remains owned by the GNMConnectivity
 *
 * @param pszName The path and the name of the creted dataset
 * @param pszFormat The name of the dataset format (e.g. ESRI Shapefile)
 * @param pszSrsInput The name of the SRS in order to set it from user
 * input (via SetFromUserInput() method)
 * @param papszConOptions Various creation option-pairs formed with
 * CSLAddNameValue(), for example:
 * 1. "con_name" "my_connectivity" // Inner connectivty name.
 * 2. "con_descr" "This is my connectivity" // Connectivity description.
 * @param papszDatasetOptions List of driver specific control parameters.
 * @return GNMConnectivity object or NULL if failed
 *
 * @since GDAL 2.0
 */
GNMConnectivity *GNMManager::CreateConnectivity (const char *pszName,
                                                const char *pszFormat,
                                                char *pszSrsInput,
                                                char **papszConOptions,
                                                char **papszDatasetOptions)
{
    //GDALAllRegister();

    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    if(poDriver == NULL)
    {
        //CPLErr
        return NULL;
    }

    GDALDataset *poDS;
    poDS = poDriver->Create(pszName, 0, 0, 0, GDT_Unknown, papszDatasetOptions);
    if(poDS == NULL)
    {
        //CPLErr
        return NULL;
    }

    GNMConnectivity *poCon;
    poCon = CreateConnectivity(poDS,pszSrsInput,papszConOptions,FALSE);
    if (poCon == NULL)
    {
        //CPLErr
        return NULL;
    }

    poCon->_ownDataset = true;
    return poCon;
}


/************************************************************************/
/*                      CreateConnectivity()                            */
/************************************************************************/
/**
 * \brief Creates the connectivity of the defined format over the data set
 *
 * NOTE: After the successfull creation the passed dataset must not be
 * mofified outside (but can be readed as usual).
 *
 * @param poDS ...
 * @param pszSrsInput ...
 * @param papszOptions ...
 * @param bNative ...
 * @return ...
 *
 * @since GDAL 2.0
 */
GNMConnectivity *GNMManager::CreateConnectivity (GDALDataset *poDS,
                                                 char *pszSrsInput,
                                                 char **papszOptions,
                                                 int bNative)
{
    if (poDS != NULL)
    {
        GNMConnectivity* poCon;

        // Determine if there is a need to use a native connectivity *driver*.
        if (bNative == FALSE) // Initialize a GDAL connectivity.
        {
            poCon = new GNMConnectivity();
        }
        else // Initialize a native connectivity.
        {
            // TODO: Try to use the special connectivity format if it is found
            // among registered connectivity-formats.
            //TEMP-------------------------
            return NULL;
            //-----------------------------
        }

        if (poCon->_create(poDS,pszSrsInput,papszOptions) == GNMERR_NONE)
            return poCon;
        else
            delete poCon;
    }

    return NULL;
}


/************************************************************************/
/*                      OpenConnectivity()                            */
/************************************************************************/
/**
 * \brief Openes the connectivity
 *
 *
 * @since GDAL 2.0
 */
GNMConnectivity *GNMManager::OpenConnectivity(GDALDataset *poDS, int bNative)
{
    if (poDS != NULL)
    {
        if (HasConnectivity(poDS,bNative))
        {
            GNMConnectivity* poCon;

            if (bNative == FALSE)
            {
                poCon = new GNMConnectivity();
            }
            else
            {
                // TODO: ...
                //TEMP-------------------------
                return NULL;
                //-----------------------------
            }

            if (poCon->_open(poDS) == GNMERR_NONE)
                return poCon;
            else
                delete poCon;
        }
    }
    return NULL;
}


/************************************************************************/
/*                      CloseConnectivity()                            */
/************************************************************************/
void GNMManager::CloseConnectivity(GNMConnectivity *poCon)
{
    if (poCon != NULL)
    {
        // TODO: what about reference/dereference?
        delete poCon; // Destructor must provide correct resources freeing.
        poCon = NULL;
    }
}


/************************************************************************/
/*                      RemoveConnectivity()                            */
/************************************************************************/
/**
 * \brief Removes the connectivity
 *
 * Tries to remove system layers/fields leaving the "pure of connectivity"
 * dataset and frees the the allocated resources.
 *
 * @param poCon ...
 * @return ...
 *
 * @since GDAL 2.0
 */
/*
GNMErr GNMManager::RemoveConnectivity(GNMConnectivity *poCon)
{
    if (poCon != NULL && poCon->_remove() == GNMERR_NONE)
    {
        delete poCon;
        poCon = NULL;
        return GNMERR_NONE;
    }
    else
    {
        return GNMERR_FAILURE;
    }
}
*/
GNMErr GNMManager::RemoveConnectivity(GDALDataset *poDS, int bNative)
{
    if (poDS != NULL)
    {
        if (bNative == FALSE)
        {
            return GNMConnectivity::_remove(poDS);
        }
        else
        {
            //...
        }
    }
    return GNMERR_FAILURE;
}


/************************************************************************/
/*                      HasConnectivity()                               */
/************************************************************************/
/**
 * \brief Checkes whether the passed dataset has the necessary and correct
 * fields/layers or smth else.
 *
 * @since GDAL 2.0
 */
bool GNMManager::HasConnectivity (GDALDataset *poDS, int bNative)
{
    if (poDS != NULL)
    {
        if (bNative == FALSE)
        {
            return GNMConnectivity::_has(poDS);
        }
        else
        {
            // TODO: ...
        }
    }
    return false;
}


