/******************************************************************************
 * Project:  geography network utility
 * Purpose:  main source file
 * Authors:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 *            Mikhail Gusev, gusevmihs at gmail dot com
 *
 ******************************************************************************
 * Copyright (C) 2014 Dmitry Baryshnikov
 * Copyright (C) 2014 Mikhail Gusev
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
#include "commonutils.h"
#include "ogr_p.h"

enum operation
{
    op_unknown = 0,
    op_create,
    op_info,
    op_add_layer
};

/************************************************************************/
/*                               Usage()                                */
/************************************************************************/
static void Usage(const char* pszAdditionalMsg, int bShort = TRUE)
{
    OGRSFDriverRegistrar        *poR = OGRSFDriverRegistrar::GetRegistrar();


    printf("Usage: gnminfo [--help-general] [-progress] [-quiet]\n"
        "               [-create [-f format_name] [-t_srs srs_name] [-dsco NAME=VALUE]... ]\n"
        "               [-i]\n"
        "               [-cp src_dataset_name] [-l layer_name]\n"
        "               gnm_name\n");

    if (bShort)
    {
        printf("\nNote: gnminfo --long-usage for full help.\n");
        if (pszAdditionalMsg)
            fprintf(stderr, "\nFAILURE: %s\n", pszAdditionalMsg);
        exit(1);
    }

    printf("\n -f format_name: output file format name, possible values are:\n");

    // Show only supported drivers

    int i;
    for (i = 0; GNMSupportedDatasets[i] != NULL; ++i)
        printf("    [%s]\n", GNMSupportedDatasets[i]);

    printf(" -progress: Display progress on terminal. Only works if input layers have the \n"
        "                                          \"fast feature count\" capability\n"
        " -dsco NAME=VALUE: Dataset creation option (format specific)\n"
        " -cp src_datasource_name: Datasource to copy\n"
        " -l layer_name: Layer name in datasource (optional)\n"
        );

    if (pszAdditionalMsg)
        fprintf(stderr, "\nFAILURE: %s\n", pszAdditionalMsg);

    exit(1);
}

static void Usage(int bShort = TRUE)
{
    Usage(NULL, bShort);
}

/* -------------------------------------------------------------------- */
/*                  CheckDestDataSourceNameConsistency()                */
/* -------------------------------------------------------------------- */

static
void CheckDestDataSourceNameConsistency(const char* pszDestFilename,
                                        const char* pszDriverName)
{
    int i;
    char* pszDestExtension = CPLStrdup(CPLGetExtension(pszDestFilename));

    /* TODO: Would be good to have driver metadata like for GDAL drivers ! */
    static const char* apszExtensions[][2] = { { "shp"    , "ESRI Shapefile" },
                                               { "dbf"    , "ESRI Shapefile" },
                                               { "sqlite" , "SQLite" },
                                               { "db"     , "SQLite" },
                                               { "mif"    , "MapInfo File" },
                                               { "tab"    , "MapInfo File" },
                                               { "s57"    , "S57" },
                                               { "bna"    , "BNA" },
                                               { "csv"    , "CSV" },
                                               { "gml"    , "GML" },
                                               { "kml"    , "KML/LIBKML" },
                                               { "kmz"    , "LIBKML" },
                                               { "json"   , "GeoJSON" },
                                               { "geojson", "GeoJSON" },
                                               { "dxf"    , "DXF" },
                                               { "gdb"    , "FileGDB" },
                                               { "pix"    , "PCIDSK" },
                                               { "sql"    , "PGDump" },
                                               { "gtm"    , "GPSTrackMaker" },
                                               { "gmt"    , "GMT" },
                                               { "pdf"    , "PDF" },
                                               { NULL, NULL }
                                              };
    static const char* apszBeginName[][2] =  { { "PG:"      , "PG" },
                                               { "MySQL:"   , "MySQL" },
                                               { "CouchDB:" , "CouchDB" },
                                               { "GFT:"     , "GFT" },
                                               { "MSSQL:"   , "MSSQLSpatial" },
                                               { "ODBC:"    , "ODBC" },
                                               { "OCI:"     , "OCI" },
                                               { "SDE:"     , "SDE" },
                                               { "WFS:"     , "WFS" },
                                               { NULL, NULL }
                                             };

    for(i=0; apszExtensions[i][0] != NULL; i++)
    {
        if (EQUAL(pszDestExtension, apszExtensions[i][0]) && !EQUAL(pszDriverName, apszExtensions[i][1]))
        {
            fprintf(stderr,
                    "Warning: The target file has a '%s' extension, which is normally used by the %s driver,\n"
                    "but the requested output driver is %s. Is it really what you want ?\n",
                    pszDestExtension,
                    apszExtensions[i][1],
                    pszDriverName);
            break;
        }
    }

    for(i=0; apszBeginName[i][0] != NULL; i++)
    {
        if (EQUALN(pszDestFilename, apszBeginName[i][0], strlen(apszBeginName[i][0])) &&
            !EQUAL(pszDriverName, apszBeginName[i][1]))
        {
            fprintf(stderr,
                    "Warning: The target file has a name which is normally recognized by the %s driver,\n"
                    "but the requested output driver is %s. Is it really what you want ?\n",
                    apszBeginName[i][1],
                    pszDriverName);
            break;
        }
    }

    CPLFree(pszDestExtension);
}


/************************************************************************/
/*                                main()                                */
/************************************************************************/

#define CHECK_HAS_ENOUGH_ADDITIONAL_ARGS(nExtraArg) \
    do { if (iArg + nExtraArg >= nArgc) \
        Usage(CPLSPrintf("%s option requires %d argument(s)", papszArgv[iArg], nExtraArg)); } while(0)

int main( int nArgc, char ** papszArgv )

{
    OGRErr eErr = OGRERR_NONE;
    int bQuiet = FALSE;
    char *pszFormat = "ESRI Shapefile";
    char *pszSRS = "EPSG:4326";

    char *pszDataSource = NULL;
    char **papszDSCO = NULL;

    char *pszInputDataset = NULL;
    char *pszInputLayer = NULL;

    operation stOper = op_unknown;

    int bDisplayProgress = FALSE;

    /* Check strict compilation and runtime library version as we use C++ API */
    if (! GDAL_CHECK_VERSION(papszArgv[0]))
        exit(1);

    EarlySetConfigOptions(nArgc, papszArgv);

/* -------------------------------------------------------------------- */
/*      Register format(s).                                             */
/* -------------------------------------------------------------------- */
    //OGRRegisterAll();
    GDALAllRegister();

/* -------------------------------------------------------------------- */
/*      Processing command line arguments.                              */
/* -------------------------------------------------------------------- */
    nArgc = OGRGeneralCmdLineProcessor( nArgc, &papszArgv, 0 );

    if( nArgc < 1 )
        exit( -nArgc );

    for( int iArg = 1; iArg < nArgc; iArg++ )
    {
        if( EQUAL(papszArgv[iArg], "--utility_version") )
        {
            printf("%s was compiled against GDAL %s and is running against GDAL %s\n",
                   papszArgv[0], GDAL_RELEASE_NAME, GDALVersionInfo("RELEASE_NAME"));
            return 0;
        }

        else if( EQUAL(papszArgv[iArg],"--help") )
        {
            Usage();
        }

        else if ( EQUAL(papszArgv[iArg], "--long-usage") )
        {
            Usage(FALSE);
        }

        else if( EQUAL(papszArgv[iArg],"-q") || EQUAL(papszArgv[iArg],"-quiet") )
        {
            bQuiet = TRUE;
        }

        else if( EQUAL(papszArgv[iArg],"-f") )
        {
            CHECK_HAS_ENOUGH_ADDITIONAL_ARGS(1);
            pszFormat = papszArgv[++iArg];
        }

        else if( EQUAL(papszArgv[iArg],"-dsco") )
        {
            CHECK_HAS_ENOUGH_ADDITIONAL_ARGS(1);
            papszDSCO = CSLAddString(papszDSCO, papszArgv[++iArg] );
        }

        else if( EQUAL(papszArgv[iArg],"-create") )
        {
            stOper = op_create;
        }

        else if( EQUAL(papszArgv[iArg],"-t_srs") )
        {
            CHECK_HAS_ENOUGH_ADDITIONAL_ARGS(1);
            pszSRS = papszArgv[++iArg];
        }

        else if( EQUAL(papszArgv[iArg],"-i") )
        {
            stOper = op_info;
        }

        else if( EQUAL(papszArgv[iArg],"-cp") )
        {
            stOper = op_add_layer;
            pszInputDataset = papszArgv[++iArg];
        }

        else if( EQUAL(papszArgv[iArg],"-l") )
        {
            pszInputLayer = papszArgv[++iArg];
        }

        else if( papszArgv[iArg][0] == '-' )
        {
            Usage(CPLSPrintf("Unknown option name '%s'", papszArgv[iArg]));
        }

        else if( pszDataSource == NULL )
            pszDataSource = papszArgv[iArg];
    }

    if(stOper == op_create)
    {
        char** options = NULL;
        const char *pszGNMName = CSLFetchNameValue(papszDSCO,GNM_CREATE_OPTIONPAIR_NAME);

        // TODO: read other parameters.

        if( pszDataSource == NULL)
            Usage("no dataset name provided");
        else if(pszFormat == NULL)
            Usage("no driver name provided");
        else  if(pszGNMName == NULL)
            //Usage("no connectivity name provided");
            printf("\nWarning: No connectivity name provided, using default ...\n");
        else
        {
            options = CSLAddNameValue(options, GNM_CREATE_OPTIONPAIR_NAME, pszGNMName);
        }

        //Create new gnm and output status (success or failed)
        GNMConnectivity *poCon;
        poCon = GNMManager::CreateConnectivity(pszDataSource,
                                               pszFormat,
                                               pszSRS,
                                               options,
                                               NULL);
        if (poCon != NULL)
        {
            if (bQuiet == FALSE)
                printf("\nConnectivity created successfully in a "
                   "new dataset at %s\n",pszDataSource);
        }
        else
        {
            fprintf(stderr, "\nFAILURE: Failed to create connectivity in a new dataset at "
                   "%s and with driver %s\n",pszDataSource,pszFormat);
        }

        GNMManager::CloseConnectivity(poCon);
        CSLDestroy(options);
    }

    else if(stOper == op_info)
    {
        if(pszDataSource == NULL)
            Usage("no dataset provided");

        GDALDataset *poDS;
        poDS = (GDALDataset*) GDALOpenEx(pszDataSource,
                                         GDAL_OF_VECTOR | GDAL_OF_READONLY,
                                         NULL, NULL, NULL );
        if(poDS == NULL)
        {
            fprintf(stderr, "\nFAILURE: Failed to open connectivity at %s\n",pszDataSource);
            exit(1);
        }

        //Output info
        GNMConnectivity *poCon;
        poCon = GNMManager::OpenConnectivity(poDS);
        if (poCon != NULL)
        {
            if (bQuiet == FALSE)
                printf("\nConnectivity opened successfully at %s\n",pszDataSource);

            GDALDataset* poDS = poCon->GetDataset();
            if (poDS != NULL)
            {
                for (int iLayer = 0; iLayer < poDS->GetLayerCount(); iLayer++)
                {
                    OGRLayer        *poLayer = poDS->GetLayer(iLayer);

                    if (poLayer == NULL)
                    {
                        printf("FAILURE: Couldn't fetch advertised layer %d!\n",
                            iLayer);
                        exit(1);
                    }

                    printf("%d: %s",
                        iLayer + 1,
                        poLayer->GetName());

                    int nGeomFieldCount =
                        poLayer->GetLayerDefn()->GetGeomFieldCount();
                    if (nGeomFieldCount > 1)
                    {
                        printf(" (");
                        for (int iGeom = 0; iGeom < nGeomFieldCount; iGeom++)
                        {
                            if (iGeom > 0)
                                printf(", ");
                            OGRGeomFieldDefn* poGFldDefn =
                                poLayer->GetLayerDefn()->GetGeomFieldDefn(iGeom);
                            printf("%s",
                                OGRGeometryTypeToName(
                                poGFldDefn->GetType()));
                        }
                        printf(")");
                    }
                    else if (poLayer->GetGeomType() != wkbUnknown)
                        printf(" (%s)",
                        OGRGeometryTypeToName(
                        poLayer->GetGeomType()));

                    printf("\n");
                }
            }

            char** params = poCon->GetMetaParamValues();

            if (params == NULL)
            {
                fprintf(stderr, "\nFAILURE: Failed to read connectivity metadata at %s\n",pszDataSource);
            }
            else
            {
                printf("\nConnectivity metadata [Name=Value]:\n");

                // We don't know the actual count of parameters.
                int i = 0;
                while (params[i] != NULL)
                {
                    printf("    [%s]\n",params[i]);
                    i++;
                }
            }
            CSLDestroy(params);

            char    *pszWKT;
            const OGRSpatialReference* poSRS = poCon->GetSpatialReference();
            if (poSRS == NULL)
                pszWKT = CPLStrdup("(unknown)");
            else
            {
                poSRS->exportToPrettyWkt(&pszWKT);
            }

            printf("SRS WKT:\n%s\n", pszWKT);
            CPLFree(pszWKT);

            //TODO: output some stats about graph

            GNMManager::CloseConnectivity(poCon);
        }
        else
        {
            fprintf(stderr, "\nFAILURE: Failed to open connectivity at %s\n",pszDataSource);
        }

        GDALClose(poDS);
    }

    else if(stOper == op_add_layer)
    {
       if(pszDataSource == NULL)
            Usage("no dataset provided");
       if(pszInputDataset == NULL)
            Usage("no input dataset name provided");

        // Copy layer
        GDALDataset *poDS;
        poDS = (GDALDataset*) GDALOpenEx(pszDataSource,
                                         GDAL_OF_VECTOR | GDAL_OF_UPDATE,
                                         NULL, NULL, NULL );
        if(poDS == NULL)
        {
            fprintf(stderr, "\nFAILURE: Failed to open connectivity at %s\n",pszDataSource);
            exit(1);
        }

        //Output info
        GNMConnectivity *poCon;
        poCon = GNMManager::OpenConnectivity(poDS);
        if (poCon != NULL)
        {
            GDALDataset *poSrcDS;
            poSrcDS = (GDALDataset*) GDALOpenEx(pszInputDataset,
                                                GDAL_OF_VECTOR | GDAL_OF_READONLY,
                                                NULL, NULL, NULL );
            if(poSrcDS == NULL)
            {
                fprintf(stderr, "\nFAILURE: Can not open dataset at %s\n",
                        pszInputDataset);
                exit(1);
            }

            OGRLayer *poSrcLayer;
            if (pszInputLayer != NULL)
                poSrcLayer = poSrcDS->GetLayerByName(pszInputLayer);
            else
                poSrcLayer = poSrcDS->GetLayer(0);

            if (poSrcLayer == NULL)
            {
                if (pszInputLayer != NULL)
                    fprintf(stderr, "\nFAILURE: Can not open layer %s in %s\n",
                        pszInputLayer,pszInputDataset);
                else
                    fprintf(stderr, "\nFAILURE: Can not open layer in %s\n",
                    pszInputDataset);

                GDALClose(poSrcDS);
                exit(1);
            }

            GNMErr err = poCon->CopyLayer(poSrcLayer, poSrcLayer->GetName());
            if (err != GNMERR_NONE)
            {
                if (pszInputLayer != NULL)
                    fprintf(stderr, "\nFAILURE: Can not copy layer %s from %s\n",
                        pszInputLayer,pszInputDataset);
                else
                    fprintf(stderr, "\nFAILURE: Can not copy layer from %s\n",
                    pszInputDataset);
                GDALClose(poSrcDS);
                exit(1);
            }

            if (bQuiet == FALSE)
            {
                if (pszInputLayer != NULL)
                    printf("\nLayer %s successfully copied from %s and added to the connectivity at %s\n",
                    pszInputLayer, pszInputDataset, pszDataSource);
                else
                    printf("\nLayer successfully copied from %s and added to the connectivity at %s\n",
                    pszInputDataset, pszDataSource);
            }

            GDALClose(poSrcDS);
            GNMManager::CloseConnectivity(poCon);
        }
        else
        {
            printf("Failed to open connectivity at %s",pszDataSource);
        }

        GDALClose(poDS);
    }

    else
    {
        Usage("no operation provided");
    }

/* -------------------------------------------------------------------- */
/*      Close down.                                                     */
/* -------------------------------------------------------------------- */

    OGRCleanupAll();

    return eErr == OGRERR_NONE ? 0 : 1;
}

