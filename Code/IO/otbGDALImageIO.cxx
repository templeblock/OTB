/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <iostream>
#include <fstream>
#include <vector>

#include "otbGDALImageIO.h"
#include "otbMacro.h"
#include "otbSystem.h"
#include "otbImage.h"
#include "itkVariableLengthVector.h"

#include "itkMetaDataObject.h"
#include "otbMetaDataKey.h"

#include "itkExceptionObject.h"
#include "itkMacro.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"
#include "itkTimeProbe.h"

/* GDAL Libraries */
#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_string.h"
#include "cpl_conv.h"
#include "ogr_spatialref.h"
#include "ogr_srs_api.h"

namespace otb
{

/*
template<class InputType>
void printOutputData(InputType *pData, int nbBands, int nbPixelToRead)
{
  for (unsigned int itPxl = 0; itPxl < (unsigned int) (nbPixelToRead * nbBands); itPxl++)
    {
    std::cout << "Buffer["<< itPxl << "] = " << *(pData + itPxl) << std::endl;
    }
};

void printDataBuffer(unsigned char *pData, GDALDataType pxlType, int nbBands, int nbPixelToRead)
{
  if (pxlType == GDT_Int16)
    {
    printOutputData( static_cast<short*>( static_cast<void*>(pData) ), nbBands, nbPixelToRead);
    }
  else if (pxlType == GDT_Int32)
    {
    printOutputData( static_cast<int*>( static_cast<void*>(pData) ), nbBands, nbPixelToRead);
    }
  else if (pxlType == GDT_Float32)
    {
    printOutputData( static_cast<float*>( static_cast<void*>(pData) ), nbBands, nbPixelToRead);
    }
  else if (pxlType == GDT_Float64)
    {
    printOutputData( static_cast<double*>( static_cast<void*>(pData) ), nbBands, nbPixelToRead);
    }
  else if (pxlType == GDT_CInt16)
    {
    printOutputData( static_cast<std::complex<short>*>( static_cast<void*>(pData) ), nbBands, nbPixelToRead);
    }
  else if (pxlType == GDT_CInt32)
    {
    printOutputData( static_cast<std::complex<int>*>( static_cast<void*>(pData) ), nbBands, nbPixelToRead);
    }
  else if (pxlType == GDT_CFloat32)
    {
    printOutputData( static_cast<std::complex<float>*>( static_cast<void*>(pData) ), nbBands, nbPixelToRead);
    }
  else if (pxlType == GDT_CFloat64)
    {
    printOutputData( static_cast<std::complex<double>*>( static_cast<void*>(pData) ), nbBands, nbPixelToRead);
    }
  else
    {
    std::cerr << "Pixel type unknown" << std::endl;
    }
};
*/

// only two states : the Pointer is Null or GetDataSet() returns a valid dataset
class GDALDatasetWrapper : public itk::LightObject
{
  friend class GDALDriverManagerWrapper;

public:
  typedef GDALDatasetWrapper      Self;
  typedef itk::LightObject        Superclass;
  typedef itk::SmartPointer<Self> Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(GDALImageIO, itk::LightObject);

  /** Easy access to the internal GDALDataset object.
   *  Don't close it, it will be automatic */
  GDALDataset* GetDataSet() const
    {
    return m_Dataset;
    }

protected :
  GDALDatasetWrapper()
   : m_Dataset(NULL)
  {
  }

  ~GDALDatasetWrapper()
  {
    if (m_Dataset)
      {
      GDALClose(m_Dataset);
      }
  }

private:
  GDALDataset* m_Dataset;
}; // end of GDALDatasetWrapper


// Wraps the GdalDriverManager so that GDALAllRegister is called automatically
class GDALDriverManagerWrapper
{
public:
  // GetInstance returns a reference to a GDALDriverManagerWrapper
  // This is the only entry point to interact with this class
  static GDALDriverManagerWrapper& GetInstance()
  {

    // Declare a static method variable of type GDALDriverManagerWrapper
    // so that it is constructed and initialized only on the first call
    // to GetInstance(), and so try to avoid static initialization order problems

    static GDALDriverManagerWrapper theUniqueInstance;
    return theUniqueInstance;
  }

  // Open the file for reading and returns a smart dataset pointer
  GDALDatasetWrapper::Pointer Open( std::string filename ) const
  {
    GDALDatasetWrapper::Pointer datasetWrapper;
    GDALDatasetH dataset = GDALOpen(filename.c_str(), GA_ReadOnly);

    if (dataset != NULL)
      {
      datasetWrapper = GDALDatasetWrapper::New();
      datasetWrapper->m_Dataset = static_cast<GDALDataset*>(dataset);
      }
    return datasetWrapper;
  }

  // Open the new  file for writing and returns a smart dataset pointer
  GDALDatasetWrapper::Pointer Create( std::string driverShortName, std::string filename,
                                      int nXSize, int nYSize, int nBands,
                                      GDALDataType eType, char ** papszOptions ) const
  {
    GDALDatasetWrapper::Pointer datasetWrapper;

    GDALDriver*  driver = GetDriverByName( driverShortName );
    if(driver != NULL)
      {
      GDALDataset* dataset = driver->Create(filename.c_str(),
                                            nXSize, nYSize,
                                            nBands, eType,
                                            papszOptions );

      if (dataset != NULL)
        {
        datasetWrapper = GDALDatasetWrapper::New();
        datasetWrapper->m_Dataset = dataset;
        }
      }
    return datasetWrapper;
  }


  GDALDriver* GetDriverByName( std::string driverShortName ) const
  {
    return GetGDALDriverManager()->GetDriverByName(driverShortName.c_str());
  }

private :
  // private constructor so that this class is allocated only inside GetInstance
  GDALDriverManagerWrapper()
  {
    GDALAllRegister();

#ifndef CHECK_HDF4OPEN_SYMBOL
    // Get rid of the HDF4 driver when it is buggy
    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName( "hdf4" );
    if (driver)
      GetGDALDriverManager()->DeregisterDriver( driver );
#endif
  }

  ~GDALDriverManagerWrapper()
  {
    GDALDestroyDriverManager();
  }
}; // end of GDALDriverManagerWrapper

class GDALDataTypeWrapper
{
public:
  GDALDataTypeWrapper(): pixType(GDT_Byte) {}
  ~GDALDataTypeWrapper() {}
  GDALDataTypeWrapper(const GDALDataTypeWrapper& w)
  {
    pixType = w.pixType;
  }
  GDALDataTypeWrapper& operator=(GDALDataTypeWrapper w)
  {
    pixType = w.pixType;
    return *this;
  }
  GDALDataType pixType;
}; // end of GDALDataTypeWrapper


GDALImageIO::GDALImageIO()
{
  // By default set number of dimensions to two.
  this->SetNumberOfDimensions(2);

  // By default set pixel type to scalar.
  m_PixelType = SCALAR;

  // By default set component type to unsigned char
  m_ComponentType = UCHAR;
  m_UseCompression = false;
  m_CompressionLevel = 4; // Range 0-9; 0 = no file compression, 9 = maximum file compression

  // Set default spacing to one
  m_Spacing[0] = 1.0;
  m_Spacing[1] = 1.0;
  // Set default origin to zero
  m_Origin[0] = 0.0;
  m_Origin[1] = 0.0;

  m_IsIndexed   = false;
  m_DatasetNumber = 0;
  //m_poBands     = NULL;
  //m_hDriver     = NULL;
  //m_poDataset   = NULL;

  m_NbBands = 0;
  m_FlagWriteImageInformation = true;

  m_CanStreamWrite = false;
  m_IsComplex = false;
  m_IsVectorImage = false;

  m_PxType = new GDALDataTypeWrapper;
}

GDALImageIO::~GDALImageIO()
{
  delete m_PxType;
}

// Tell only if the file can be read with GDAL.
bool GDALImageIO::CanReadFile(const char* file)
{
  // First check the extension
  if (file == NULL)
    {
    itkDebugMacro(<< "No filename specified.");
    return false;
    }
  m_Dataset = GDALDriverManagerWrapper::GetInstance().Open(file);
  return m_Dataset.IsNotNull();
}

// Used to print information about this object
void GDALImageIO::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Compression Level : " << m_CompressionLevel << "\n";
  os << indent << "IsComplex (otb side) : " << m_IsComplex << "\n";
  os << indent << "Byte per pixel : " << m_BytePerPixel << "\n";
}

// Read a 3D image (or event more bands)... not implemented yet
void GDALImageIO::ReadVolume(void*)
{
}

// Read image with GDAL
void GDALImageIO::Read(void* buffer)
{
  // Convert buffer from void * to unsigned char *
  unsigned char *p = static_cast<unsigned char *>(buffer);

  // Check if conversion succeed
  if (p == NULL)
    {
    itkExceptionMacro(<< "GDAL : Bad alloc");
    return;
    }

  // Get nb. of lines and columns of the region to read
  int lNbLines     = this->GetIORegion().GetSize()[1];
  int lNbColumns   = this->GetIORegion().GetSize()[0];
  int lFirstLine   = this->GetIORegion().GetIndex()[1]; // [1... ]
  int lFirstColumn = this->GetIORegion().GetIndex()[0]; // [1... ]

  GDALDataset* dataset = m_Dataset->GetDataSet();

  // This special case is due to the fact the CINT/CLONG types
  // do not exists in ITK. In this case we only report the first band
  // TODO This should be fixed
  /*if (GDALDataTypeIsComplex(m_PxType->pixType)
      && (m_PxType->pixType != GDT_CFloat32)
      && (m_PxType->pixType != GDT_CFloat64))
    {
    int pixelOffset = m_BytePerPixel * m_NbBands;
    int lineOffset  = m_BytePerPixel * m_NbBands * lNbColumns;
    int bandOffset  = m_BytePerPixel;
    int nbBands     = m_NbBands;

    int nbPixelToRead = lNbColumns *  lNbLines;
    std::streamoff nbBytes = static_cast<std::streamoff>(m_NbBands) * static_cast<std::streamoff>(nbPixelToRead) * static_cast<std::streamoff>(m_BytePerPixel);
    unsigned char *pBufferTemp = new unsigned char[static_cast<unsigned int>(nbBytes)];

    // keep it for the moment
    otbMsgDevMacro(<< "Parameters RasterIO (case CInt and CShort):"
                   << "\n indX = " << lFirstColumn
                   << "\n indY = " << lFirstLine
                   << "\n sizeX = " << lNbColumns
                   << "\n sizeY = " << lNbLines
                   << "\n GDAL Data Type = " << GDALGetDataTypeName(m_PxType->pixType)
                   << "\n pixelOffset = " << pixelOffset
                   << "\n lineOffset = " << lineOffset
                   << "\n bandOffset = " << bandOffset);

    CPLErr lCrGdal = m_Dataset->GetDataSet()->RasterIO(GF_Read,
                                                       lFirstColumn,
                                                       lFirstLine,
                                                       lNbColumns,
                                                       lNbLines,
                                                       pBufferTemp, //p, // pData
                                                       lNbColumns,
                                                       lNbLines,
                                                       m_PxType->pixType,
                                                       nbBands,
                                                       // We want to read all bands
                                                       NULL,
                                                       pixelOffset,
                                                       lineOffset,
                                                       bandOffset);
    // Check for gdal error
    if (lCrGdal == CE_Failure)
      {
      itkExceptionMacro(<< "Error while reading image (GDAL format) " << m_FileName );
      delete[] pBufferTemp;
      return;
      }
    //std::cout << "RAW BUFFER:" <<std::endl;
    //printDataBuffer(pBufferTemp, m_PxType->pixType, m_NbBands, lNbColumns*lNbLines);

    // Convert the buffer to GDT_Float64 type
    typedef std::complex<float>           RealType;
    typedef double                         ScalarRealType;

    if (m_PxType->pixType == GDT_CInt32)
      {
      //std::cout << "Convert input File from GDT_CInt32 to GDT_CFloat32" << std::endl;
      typedef std::complex<int>              ComplexIntType;

      for (unsigned int itPxl = 0; itPxl < (unsigned int) (nbPixelToRead * m_NbBands); itPxl++)
        {
        ComplexIntType pxlValue = *(static_cast<ComplexIntType*>( static_cast<void*>(pBufferTemp)) + itPxl );

        RealType    pxlValueReal( static_cast<ScalarRealType>(pxlValue.real()), static_cast<ScalarRealType>(pxlValue.imag()) );

        memcpy((void*) (&(p[itPxl*sizeof(RealType)])), (const void*) (&(pxlValueReal)), (size_t) (sizeof(RealType)));
        }
      }
    else if (m_PxType->pixType == GDT_CInt16)
      {
      //std::cout << "Convert input File from GDT_CInt16 to GDT_CFloat32" << std::endl;
      typedef std::complex<short>            ComplexShortType;

      for (unsigned int itPxl = 0; itPxl < (unsigned int) (nbPixelToRead * m_NbBands); itPxl++)
        {
        ComplexShortType pxlValue = *(static_cast<ComplexShortType*>( static_cast<void*>(pBufferTemp)) + itPxl );

        RealType    pxlValueReal( static_cast<ScalarRealType>(pxlValue.real()), static_cast<ScalarRealType>(pxlValue.imag()) );

        memcpy((void*) (&(p[itPxl*sizeof(RealType)])), (const void*) (&(pxlValueReal)), (size_t) (sizeof(RealType)));
        }
      }
    //std::cout << "CONVERTED BUFFER:" <<std::endl;
    //printDataBuffer(p, GDT_CFloat64, m_NbBands, lNbColumns*lNbLines);
    delete[] pBufferTemp;
    }

  // In the indexed case, one has to retrieve the index image and the
  // color table, and translate p to a 4 components color values buffer
  else*/ if (m_IsIndexed)
    {
    // TODO: This is a very special case and seems to be working only
    // for unsigned char pixels. There might be a gdal method to do
    // the work in a cleaner way
    std::streamoff lNbPixels = (static_cast<std::streamoff>(lNbColumns))
                             * (static_cast<std::streamoff>(lNbLines));
    std::streamoff lBufferSize = static_cast<std::streamoff>(m_BytePerPixel) * lNbPixels;
    itk::VariableLengthVector<unsigned char> value(lBufferSize);

   std::streamoff step = static_cast<std::streamoff>(this->GetNumberOfComponents())
                       * static_cast<std::streamoff>(m_BytePerPixel);

    CPLErr lCrGdal = dataset->GetRasterBand(1)->RasterIO(GF_Read,
                                     lFirstColumn,
                                     lFirstLine,
                                     lNbColumns,
                                     lNbLines,
                                     const_cast<unsigned char*>(value.GetDataPointer()),
                                     lNbColumns,
                                     lNbLines,
                                     m_PxType->pixType,
                                     0,
                                     0);
    if (lCrGdal == CE_Failure)
      {
      itkExceptionMacro(<< "Error while reading image (GDAL format) " << m_FileName.c_str() << ".");
      }
    // Interpret index as color
    std::streamoff cpt(0);
    GDALColorTable* colorTable = dataset->GetRasterBand(1)->GetColorTable();
    for (std::streamoff i = 0; i < lBufferSize; i = i + static_cast<std::streamoff>(m_BytePerPixel))
      {
      GDALColorEntry color;
      colorTable->GetColorEntryAsRGB(value[i], &color);
      p[cpt] = color.c1;
      p[cpt + 1] = color.c2;
      p[cpt + 2] = color.c3;
      p[cpt + 3] = color.c4;
      cpt += step;
      }
    }
  else
    {
    /********  Nominal case ***********/
    int pixelOffset = m_BytePerPixel * m_NbBands;
    int lineOffset  = m_BytePerPixel * m_NbBands * lNbColumns;
    int bandOffset  = m_BytePerPixel;
    int nbBands     = m_NbBands;

    // In some cases, we need to change some parameters for RasterIO
    if(!GDALDataTypeIsComplex(m_PxType->pixType) && m_IsComplex && m_IsVectorImage && (m_NbBands > 1))
      {
      pixelOffset = m_BytePerPixel * 2;
      lineOffset  = pixelOffset * lNbColumns;
      bandOffset  = m_BytePerPixel;
      }

    // keep it for the moment
    //otbMsgDevMacro(<< "Number of bands inside input file: " << m_NbBands);
    otbMsgDevMacro(<< "Parameters RasterIO : \n"
                   << " indX = " << lFirstColumn << "\n"
                   << " indY = " << lFirstLine << "\n"
                   << " sizeX = " << lNbColumns << "\n"
                   << " sizeY = " << lNbLines << "\n"
                   << " GDAL Data Type = " << GDALGetDataTypeName(m_PxType->pixType) << "\n"
                   << " pixelOffset = " << pixelOffset << "\n"
                   << " lineOffset = " << lineOffset << "\n"
                   << " bandOffset = " << bandOffset);

    itk::TimeProbe chrono;
    chrono.Start();
    CPLErr lCrGdal = m_Dataset->GetDataSet()->RasterIO(GF_Read,
                                                       lFirstColumn,
                                                       lFirstLine,
                                                       lNbColumns,
                                                       lNbLines,
                                                       p,
                                                       lNbColumns,
                                                       lNbLines,
                                                       m_PxType->pixType,
                                                       nbBands,
                                                       // We want to read all bands
                                                       NULL,
                                                       pixelOffset,
                                                       lineOffset,
                                                       bandOffset);
    chrono.Stop();
    otbMsgDevMacro(<< "RasterIO Read took " << chrono.GetTotal() << " sec")

    // Check if gdal call succeed
    if (lCrGdal == CE_Failure)
      {
      itkExceptionMacro(<< "Error while reading image (GDAL format) " << m_FileName.c_str() << ".");
      return;
      }
    //printDataBuffer(p, m_PxType->pixType, m_NbBands, lNbColumns*lNbLines);
    }
}

bool GDALImageIO::GetSubDatasetInfo(std::vector<std::string> &names, std::vector<std::string> &desc)
{
  // Note: we assume that the subdatasets are in order : SUBDATASET_ID_NAME, SUBDATASET_ID_DESC, SUBDATASET_ID+1_NAME, SUBDATASET_ID+1_DESC
  char** papszMetadata;
  papszMetadata = m_Dataset->GetDataSet()->GetMetadata("SUBDATASETS");

  // Have we find some dataSet ?
  // This feature is supported only for hdf4 and hdf5 file (regards to the bug 270)
  if ( (CSLCount(papszMetadata) > 0) &&
       ( (strcmp(m_Dataset->GetDataSet()->GetDriver()->GetDescription(),"HDF4") == 0) ||
         (strcmp(m_Dataset->GetDataSet()->GetDriver()->GetDescription(),"HDF5") == 0) ) )
    {
    for (int cpt = 0; papszMetadata[cpt] != NULL; cpt++)
      {
      std::string key, name;
      if (System::ParseHdfSubsetName(papszMetadata[cpt], key, name))
        {
        otbMsgDevMacro(<< "- key:  " << key);
        otbMsgDevMacro(<< "- name: " << name);
        // check if this is a dataset name
        if (key.find("_NAME") != std::string::npos) names.push_back(name);
        // check if this is a dataset descriptor
        if (key.find("_DESC") != std::string::npos) desc.push_back(name);
        }
      }
    }
  else
    {
    return false;
    }
  if (names.empty() || desc.empty()) return false;
  if (names.size() != desc.size())
    {
    names.clear();
    desc.clear();
    return false;
    }

  return true;
}

bool GDALImageIO::GDALPixelTypeIsComplex()
{
  return GDALDataTypeIsComplex(m_PxType->pixType);
}

void GDALImageIO::ReadImageInformation()
{
  //std::ifstream file;
  this->InternalReadImageInformation();
}

void GDALImageIO::InternalReadImageInformation()
{
  // Detecting if we are in the case of an image with subdatasets
  // example: hdf Modis data
  // in this situation, we are going to change the filename to the
  // supported gdal format using the m_DatasetNumber value
  // HDF4_SDS:UNKNOWN:"myfile.hdf":2
  // and make m_Dataset point to it.
  if (m_Dataset->GetDataSet()->GetRasterCount() == 0)
    {
    // this happen in the case of a hdf file with SUBDATASETS
    // Note: we assume that the datasets are in order
    char** papszMetadata;
    papszMetadata = m_Dataset->GetDataSet()->GetMetadata("SUBDATASETS");
    //TODO: we might want to keep the list of names somewhere, at least the number of datasets
    std::vector<std::string> names;
    if( CSLCount(papszMetadata) > 0 )
      {
      for( int cpt = 0; papszMetadata[cpt] != NULL; cpt++ )
        {
        std::string key, name;
        if (System::ParseHdfSubsetName(papszMetadata[cpt], key, name))
          {
          otbMsgDevMacro(<< "- key:  " << key);
          otbMsgDevMacro(<< "- name: " << name);
          // check if this is a dataset name
          if (key.find("_NAME") != std::string::npos) names.push_back(name);
          }
        }
      }
    if (m_DatasetNumber < names.size())
      {
      otbMsgDevMacro(<< "Reading: " << names[m_DatasetNumber]);
      m_Dataset = GDALDriverManagerWrapper::GetInstance().Open(names[m_DatasetNumber]);
      }
    else
      {
      itkExceptionMacro(<< "Dataset requested does not exist (" << names.size() << " datasets)");
      }
    }

  GDALDataset* dataset = m_Dataset->GetDataSet();

  // Get image dimensions
  if ( dataset->GetRasterXSize() == 0 || dataset->GetRasterYSize() == 0 )
    {
    itkExceptionMacro(<< "Dimension is undefined.");
    }

  // Set image dimensions into IO
  m_Dimensions[0] = dataset->GetRasterXSize();
  m_Dimensions[1] = dataset->GetRasterYSize();

  // Get Number of Bands
  m_NbBands = dataset->GetRasterCount();

  otbMsgDevMacro(<< "Input file dimension: " << m_Dimensions[0] << ", " << m_Dimensions[1]);
  otbMsgDevMacro(<< "Number of bands inside input file: " << m_NbBands);

  this->SetNumberOfComponents(m_NbBands);

  // Set the number of dimensions (verify for the dim )
  this->SetNumberOfDimensions(2);

  otbMsgDevMacro(<< "Nb of Dimensions of the input file: " << m_NumberOfDimensions);

  // Automatically set the Type to Binary for GDAL data
  this->SetFileTypeToBinary();

  // Get Data Type
  // Consider only the data type given by the first band
  // Maybe be could changed (to check)
  m_PxType->pixType = dataset->GetRasterBand(1)->GetRasterDataType();
  otbMsgDevMacro(<< "PixelType inside input file: "<< GDALGetDataTypeName(m_PxType->pixType) );
  if (m_PxType->pixType == GDT_Byte)
    {
    SetComponentType(UCHAR);
    }
  else if (m_PxType->pixType == GDT_UInt16)
    {
    SetComponentType(USHORT);
    }
  else if (m_PxType->pixType == GDT_Int16)
    {
    SetComponentType(SHORT);
    }
  else if (m_PxType->pixType == GDT_UInt32)
    {
    SetComponentType(UINT);
    }
  else if (m_PxType->pixType == GDT_Int32)
    {
    SetComponentType(INT);
    }
  else if (m_PxType->pixType == GDT_Float32)
    {
    SetComponentType(FLOAT);
    }
  else if (m_PxType->pixType == GDT_Float64)
    {
    SetComponentType(DOUBLE);
    }
  else if (m_PxType->pixType == GDT_CInt16)
    {
    SetComponentType(CSHORT);
    }
  else if (m_PxType->pixType == GDT_CInt32)
    {
    SetComponentType(CINT);
    }
  else if (m_PxType->pixType == GDT_CFloat32)
    {
    SetComponentType(CFLOAT);
    }
  else if (m_PxType->pixType == GDT_CFloat64)
    {
    SetComponentType(CDOUBLE);
    }
  else
    {
    itkExceptionMacro(<< "Pixel type unknown");
    }

  if (this->GetComponentType() == CHAR)
    {
    m_BytePerPixel = 1;
    }
  else if (this->GetComponentType() == UCHAR)
    {
    m_BytePerPixel = 1;
    }
  else if (this->GetComponentType() == USHORT)
    {
    m_BytePerPixel = 2;
    }
  else if (this->GetComponentType() == SHORT)
    {
    m_BytePerPixel = 2;
    }
  else if (this->GetComponentType() == INT)
    {
    m_BytePerPixel = 4;
    }
  else if (this->GetComponentType() == UINT)
    {
    m_BytePerPixel = 4;
    }
  else if (this->GetComponentType() == FLOAT)
    {
    m_BytePerPixel = 4;
    }
  else if (this->GetComponentType() == DOUBLE)
    {
    m_BytePerPixel = 8;
    }
  else if (this->GetComponentType() == CSHORT)
    {
    m_BytePerPixel = sizeof(std::complex<short>);
    }
  else if (this->GetComponentType() == CINT)
    {
    m_BytePerPixel = sizeof(std::complex<int>);
    }
  else if (this->GetComponentType() == CFLOAT)
    {
    /*if (m_PxType->pixType == GDT_CInt16)
      m_BytePerPixel = sizeof(std::complex<short>);
    else if (m_PxType->pixType == GDT_CInt32)
      m_BytePerPixel = sizeof(std::complex<int>);
    else*/
      m_BytePerPixel = sizeof(std::complex<float>);
    }
  else if (this->GetComponentType() == CDOUBLE)
    {
      m_BytePerPixel = sizeof(std::complex<double>);
    }
  else
    {
    itkExceptionMacro(<< "Component type unknown");
    }

  /******************************************************************/
  // Set the pixel type with some special cases linked to the fact
  //  we read some data with complex type.
  if ( GDALDataTypeIsComplex(m_PxType->pixType) ) // Try to read data with complex type with GDAL
    {
    if ( !m_IsComplex && m_IsVectorImage )
      {
      // we are reading a complex data set into an image where the pixel
      // type is Vector<real>: we have to double the number of component
      // for that to work
      otbMsgDevMacro( << "GDALtypeIO= Complex and IFReader::InternalPixelType= Scalar and IFReader::PixelType= Vector");
      this->SetNumberOfComponents(m_NbBands*2);
      this->SetPixelType(VECTOR);
      }
    else
      {
      this->SetPixelType(COMPLEX);
      }
    }
  else // Try to read data with scalar type with GDAL
    {
    this->SetNumberOfComponents(m_NbBands);
    if (this->GetNumberOfComponents() == 1)
      {
      this->SetPixelType(SCALAR);
      }
    else
      {
      this->SetPixelType(VECTOR);
      }
    }

  /*** Parameters set by Internal Read function ***/
  otbMsgDevMacro( << "Pixel Type IFReader = " << GetPixelTypeAsString(this->GetPixelType()) )
  otbMsgDevMacro( << "Number of component IFReader = " << this->GetNumberOfComponents() )
  otbMsgDevMacro( << "Byte per pixel set = " << m_BytePerPixel )
  otbMsgDevMacro( << "Component Type set = " << GetComponentTypeAsString(this->GetComponentType()) );

  /*----------------------------------------------------------------------*/
  /*-------------------------- METADATA ----------------------------------*/
  /*----------------------------------------------------------------------*/

  // Now initialize the itk dictionary
  itk::MetaDataDictionary& dict = this->GetMetaDataDictionary();

  /* -------------------------------------------------------------------- */
  /*  Get Spacing                */
  /* -------------------------------------------------------------------- */

  // Default Spacing
  m_Spacing[0] = 1;
  m_Spacing[1] = 1;
  if (m_NumberOfDimensions == 3) m_Spacing[2] = 1;

  char** papszMetadata;
  papszMetadata =  dataset->GetMetadata(NULL);

  /* -------------------------------------------------------------------- */
  /*      Report general info.                                            */
  /* -------------------------------------------------------------------- */
  GDALDriverH hDriver;

  hDriver = dataset->GetDriver();

  std::string driverShortName =  static_cast<std::string>(GDALGetDriverShortName(hDriver));
  std::string driverLongName  =  static_cast<std::string>(GDALGetDriverLongName(hDriver));

  itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::DriverShortNameKey, driverShortName);
  itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::DriverLongNameKey,  driverLongName);

  /* -------------------------------------------------------------------- */
  /* Get the projection coordinate system of the image : ProjectionRef  */
  /* -------------------------------------------------------------------- */
  if (dataset->GetProjectionRef() != NULL && !std::string(dataset->GetProjectionRef()).empty())
    {
    OGRSpatialReferenceH pSR = OSRNewSpatialReference(NULL);

    const char *         pszProjection = NULL;
    pszProjection =  dataset->GetProjectionRef();

    if (OSRImportFromWkt(pSR, (char **) (&pszProjection)) == OGRERR_NONE)
      {
      char * pszPrettyWkt = NULL;
      OSRExportToPrettyWkt(pSR, &pszPrettyWkt, FALSE);

      itk::EncapsulateMetaData<std::string> (dict, MetaDataKey::ProjectionRefKey,
                                             static_cast<std::string>(pszPrettyWkt));

      CPLFree(pszPrettyWkt);
      }
    else
      {
      itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::ProjectionRefKey,
                                            static_cast<std::string>(dataset->GetProjectionRef()));
      }

    if (pSR != NULL)
      {
      OSRRelease(pSR);
      pSR = NULL;
      }
    }

  /* -------------------------------------------------------------------- */
  /* Get the GCP projection coordinates of the image : GCPProjection  */
  /* -------------------------------------------------------------------- */

  unsigned int gcpCount = 0;
  gcpCount = dataset->GetGCPCount();
  if (gcpCount > 0)
    {
    std::string gcpProjectionKey = static_cast<std::string>(dataset->GetGCPProjection());
    itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::GCPProjectionKey, gcpProjectionKey);

    if (gcpProjectionKey.empty())
      {
      gcpCount = 0; //fix for uninitialized gcpCount in gdal (when
                    //reading Palsar image)
      }

    std::string key;

    itk::EncapsulateMetaData<unsigned int>(dict, MetaDataKey::GCPCountKey, gcpCount);

    for (unsigned int cpt = 0; cpt < gcpCount; cpt++)
      {

      const GDAL_GCP *psGCP;
      psGCP = dataset->GetGCPs() + cpt;

      OTB_GCP pOtbGCP;
      pOtbGCP.m_Id = std::string(psGCP->pszId);
      pOtbGCP.m_Info = std::string(psGCP->pszInfo);
      pOtbGCP.m_GCPRow = psGCP->dfGCPLine;
      pOtbGCP.m_GCPCol = psGCP->dfGCPPixel;
      pOtbGCP.m_GCPX = psGCP->dfGCPX;
      pOtbGCP.m_GCPY = psGCP->dfGCPY;
      pOtbGCP.m_GCPZ = psGCP->dfGCPZ;

      // Complete the key with the GCP number : GCP_i
      std::ostringstream lStream;
      lStream << MetaDataKey::GCPParametersKey << cpt;
      key = lStream.str();

      itk::EncapsulateMetaData<OTB_GCP>(dict, key, pOtbGCP);

      }

    }

  /* -------------------------------------------------------------------- */
  /*  Get the six coefficients of affine geoTtransform      */
  /* -------------------------------------------------------------------- */

  double     adfGeoTransform[6];
  MetaDataKey::VectorType VadfGeoTransform;

  if (dataset->GetGeoTransform(adfGeoTransform) == CE_None)
    {
    for (int cpt = 0; cpt < 6; cpt++)
      VadfGeoTransform.push_back(adfGeoTransform[cpt]);

    itk::EncapsulateMetaData<MetaDataKey::VectorType>(dict, MetaDataKey::GeoTransformKey, VadfGeoTransform);

    /// retrieve origin and spacing from the geo transform
    m_Origin[0] = VadfGeoTransform[0];
    m_Origin[1] = VadfGeoTransform[3];
    m_Spacing[0] = VadfGeoTransform[1];
    m_Spacing[1] = VadfGeoTransform[5];

    }

  // Dataset info
  otbMsgDevMacro(<< "**** ReadImageInformation() DATASET INFO: ****" );
  otbMsgDevMacro(<< "Projection Ref: "<< dataset->GetProjectionRef() );
  double GT[6];
  if (dataset->GetGeoTransform(GT) == CE_None)
    {
    otbMsgDevMacro( <<"Geo Transform: "<< GT[0] << ", " << GT[1] << ", "
                                 << GT[2] << ", " << GT[3] << ", "
                                 << GT[4] << ", " << GT[5] );
    }
  else
    {
    otbMsgDevMacro( << "No Geo Transform: ");
    }
  otbMsgDevMacro(<< "GCP Projection Ref: "<< dataset->GetGCPProjection() );
  otbMsgDevMacro(<< "GCP Count: " << dataset->GetGCPCount() );

  /* -------------------------------------------------------------------- */
  /*      Report metadata.                                                */
  /* -------------------------------------------------------------------- */

  papszMetadata = dataset->GetMetadata(NULL);
  if (CSLCount(papszMetadata) > 0)
    {
    std::string key;

    for (int cpt = 0; papszMetadata[cpt] != NULL; cpt++)
      {
      std::ostringstream lStream;
      lStream << MetaDataKey::MetadataKey << cpt;
      key = lStream.str();

      itk::EncapsulateMetaData<std::string>(dict, key,
                                            static_cast<std::string>(papszMetadata[cpt]));
      }
    }

  /* -------------------------------------------------------------------- */
  /*      Report subdatasets.                                             */
  /* -------------------------------------------------------------------- */

  papszMetadata = dataset->GetMetadata("SUBDATASETS");
  if (CSLCount(papszMetadata) > 0)
    {
    std::string key;

    for (int cpt = 0; papszMetadata[cpt] != NULL; cpt++)
      {
      std::ostringstream lStream;
      lStream << MetaDataKey::SubMetadataKey << cpt;
      key = lStream.str();

      itk::EncapsulateMetaData<std::string>(dict, key,
                                            static_cast<std::string>(papszMetadata[cpt]));
      }
    }

  /* -------------------------------------------------------------------- */
  /* Report corners              */
  /* -------------------------------------------------------------------- */

  double     GeoX(0), GeoY(0);
  MetaDataKey::VectorType VGeo;

  GDALInfoReportCorner("Upper Left", 0.0, 0.0, GeoX, GeoY);
  VGeo.push_back(GeoX);
  VGeo.push_back(GeoY);

  itk::EncapsulateMetaData<MetaDataKey::VectorType>(dict, MetaDataKey::UpperLeftCornerKey, VGeo);

  VGeo.clear();

  GDALInfoReportCorner("Upper Right", m_Dimensions[0], 0.0, GeoX, GeoY);
  VGeo.push_back(GeoX);
  VGeo.push_back(GeoY);

  itk::EncapsulateMetaData<MetaDataKey::VectorType>(dict, MetaDataKey::UpperRightCornerKey, VGeo);

  VGeo.clear();

  GDALInfoReportCorner("Lower Left", 0.0, m_Dimensions[1], GeoX, GeoY);
  VGeo.push_back(GeoX);
  VGeo.push_back(GeoY);

  itk::EncapsulateMetaData<MetaDataKey::VectorType>(dict, MetaDataKey::LowerLeftCornerKey, VGeo);

  VGeo.clear();

  GDALInfoReportCorner("Lower Right", m_Dimensions[0], m_Dimensions[1], GeoX, GeoY);
  VGeo.push_back(GeoX);
  VGeo.push_back(GeoY);

  itk::EncapsulateMetaData<MetaDataKey::VectorType>(dict, MetaDataKey::LowerRightCornerKey, VGeo);

  VGeo.clear();

  /* -------------------------------------------------------------------- */
  /* Color Table                                                          */
  /* -------------------------------------------------------------------- */

  for (int iBand = 0; iBand < dataset->GetRasterCount(); iBand++)
    {
    GDALColorTableH hTable;
    GDALRasterBandH hBand;
    hBand = GDALGetRasterBand(dataset, iBand + 1);
    if ((GDALGetRasterColorInterpretation(hBand) == GCI_PaletteIndex)
        && (hTable = GDALGetRasterColorTable(hBand)) != NULL)
      {
      m_IsIndexed = true;

      unsigned int ColorEntryCount = GDALGetColorEntryCount(hTable);

      itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::ColorTableNameKey,
                                            static_cast<std::string>(GDALGetPaletteInterpretationName(
                                                                       GDALGetPaletteInterpretation(hTable))));

      itk::EncapsulateMetaData<unsigned int>(dict, MetaDataKey::ColorEntryCountKey, ColorEntryCount);

      for (int i = 0; i < GDALGetColorEntryCount(hTable); ++i)
        {
        GDALColorEntry sEntry;
        MetaDataKey::VectorType VColorEntry;

        GDALGetColorEntryAsRGB(hTable, i, &sEntry);

        VColorEntry.push_back(sEntry.c1);
        VColorEntry.push_back(sEntry.c2);
        VColorEntry.push_back(sEntry.c3);
        VColorEntry.push_back(sEntry.c4);

        itk::EncapsulateMetaData<MetaDataKey::VectorType>(dict, MetaDataKey::ColorEntryAsRGBKey, VColorEntry);

        }
      }
    }
  if (m_IsIndexed)
    {
    m_NbBands *= 4;
    this->SetNumberOfComponents(m_NbBands);
    this->SetPixelType(VECTOR);
    }
}

bool GDALImageIO::CanWriteFile(const char* name)
{
  m_FileName = name;

  // First check the filename
  if (name == NULL)
    {
    itkDebugMacro(<< "No filename specified.");
    return false;
    }

  // Get the GDAL format ID from the name
  std::string gdalDriverShortName = FilenameToGdalDriverShortName(name);
  if (gdalDriverShortName == "NOT-FOUND")
    {
    return false;
    }

  // Check the driver for support of Create or at least CreateCopy
  GDALDriver* driver = GDALDriverManagerWrapper::GetInstance().GetDriverByName(gdalDriverShortName);
  if ( GDALGetMetadataItem( driver, GDAL_DCAP_CREATE, NULL ) == NULL
       && GDALGetMetadataItem( driver, GDAL_DCAP_CREATECOPY, NULL ) == NULL )
    {
    itkDebugMacro(<< "The driver " << GDALGetDriverShortName(driver) << " does not support writing");
    return false;
    }
  return true;
}

bool GDALImageIO::CanStreamWrite()
{
  // Get the GDAL format ID from the name
  std::string gdalDriverShortName = FilenameToGdalDriverShortName(m_FileName);
  GDALDriver* driver = GDALDriverManagerWrapper::GetInstance().GetDriverByName(gdalDriverShortName);

  if (driver == NULL)
    {
    itkDebugMacro(<< "Unable to instantiate driver " << gdalDriverShortName);
    m_CanStreamWrite = false;
    }

  if ( GDALGetMetadataItem( driver, GDAL_DCAP_CREATE, NULL ) != NULL )
    {
    m_CanStreamWrite = true;
    }
  else
    {
    m_CanStreamWrite = false;
    }
  return m_CanStreamWrite;
}

void GDALImageIO::Write(const void* buffer)
{
  // Check if we have to write the image information
  if (m_FlagWriteImageInformation == true)
    {
    this->InternalWriteImageInformation(buffer);
    m_FlagWriteImageInformation = false;
    }

  // Check if conversion succeed
  if (buffer == NULL)
    {
    itkExceptionMacro(<< "GDAL : Bad alloc");
    return;
    }

  // Compute offset and size
  unsigned int lNbLines = this->GetIORegion().GetSize()[1];
  unsigned int lNbColumns = this->GetIORegion().GetSize()[0];
  int lFirstLine = this->GetIORegion().GetIndex()[1]; // [1... ]
  int lFirstColumn = this->GetIORegion().GetIndex()[0]; // [1... ]

  // Particular case: checking that the written region is the same size
  // of the entire image
  // starting at offset 0 (when no streaming)
  if ((lNbLines == m_Dimensions[1]) && (lNbColumns == m_Dimensions[0]))
    {
    lFirstLine = 0;
    lFirstColumn = 0;
    }

  // Convert buffer from void * to unsigned char *
  //unsigned char *p = static_cast<unsigned char*>( const_cast<void *>(buffer));
  //printDataBuffer(p,  m_PxType->pixType, m_NbBands, 10*2); // Buffer incorrect

  // If driver supports streaming
  if (m_CanStreamWrite)
    {

    otbMsgDevMacro(<< "RasterIO Write requested region : " << this->GetIORegion() <<
                 "\n, lNbColumns =" << lNbColumns <<
                 "\n, lNbLines =" << lNbLines <<
                 "\n, m_PxType =" << GDALGetDataTypeName(m_PxType->pixType) <<
                 "\n, m_NbBands =" << m_NbBands <<
                 "\n, m_BytePerPixel ="<< m_BytePerPixel <<
                 "\n, Pixel offset =" << m_BytePerPixel * m_NbBands <<  // is nbComp * BytePerPixel
                 "\n, Line offset =" << m_BytePerPixel * m_NbBands * lNbColumns << // is pixelOffset * nbColumns
                 "\n, Band offset =" <<  m_BytePerPixel) //  is BytePerPixel

                 itk::TimeProbe chrono;
    chrono.Start();
    CPLErr lCrGdal = m_Dataset->GetDataSet()->RasterIO(GF_Write,
                                                       lFirstColumn,
                                                       lFirstLine,
                                                       lNbColumns,
                                                       lNbLines,
                                                       const_cast<void *>(buffer),
                                                       lNbColumns,
                                                       lNbLines,
                                                       m_PxType->pixType,
                                                       m_NbBands,
                                                       // We want to write all bands
                                                       NULL,
                                                       // Pixel offset
                                                       // is nbComp * BytePerPixel
                                                       m_BytePerPixel * m_NbBands,
                                                       // Line offset
                                                       // is pixelOffset * nbColumns
                                                       m_BytePerPixel * m_NbBands * lNbColumns,
                                                       // Band offset is BytePerPixel
                                                       m_BytePerPixel);
    chrono.Stop();
    otbMsgDevMacro(<< "RasterIO Write took " << chrono.GetTotal() << " sec")

    // Check if writing succeed
    if (lCrGdal == CE_Failure)
      {
      itkExceptionMacro(<< "Error while writing image (GDAL format) " << m_FileName.c_str() << ".");
      }
    // Flush dataset cache
    m_Dataset->GetDataSet()->FlushCache();
    }
  else
    {
    // We only wrote data to the memory dataset
    // Now write it to the real file with CreateCopy()
    std::string gdalDriverShortName = FilenameToGdalDriverShortName(m_FileName);
    std::string realFileName = GetGdalWriteImageFileName(gdalDriverShortName, m_FileName);

    GDALDriver* driver = GDALDriverManagerWrapper::GetInstance().GetDriverByName(gdalDriverShortName);
    if (driver == NULL)
      {
      itkExceptionMacro(<< "Unable to instantiate driver " << gdalDriverShortName << " to write " << m_FileName);
      }

    // If JPEG, set the JPEG compression quality to 95.
    char * option[2];
    option[0] = NULL;
    option[1] = NULL;
    // If JPEG, set the image quality
    if( gdalDriverShortName.compare("JPEG") == 0 )
      {
      option[0] = const_cast<char *>("QUALITY=95");
 
      }
    
    GDALDataset* hOutputDS = driver->CreateCopy( realFileName.c_str(), m_Dataset->GetDataSet(), FALSE,
                                                 option, NULL, NULL );
    GDALClose(hOutputDS);
    }
}

/** TODO : Methode WriteImageInformation non implementee */
void GDALImageIO::WriteImageInformation()
{
}

void GDALImageIO::InternalWriteImageInformation(const void* buffer)
{
  char **     papszOptions = NULL;
  std::string driverShortName;
  m_NbBands = this->GetNumberOfComponents();

  if ((m_Dimensions[0] == 0) && (m_Dimensions[1] == 0))
    {
    itkExceptionMacro(<< "Dimensions are not defined.");
    }

  if ((this->GetPixelType() == COMPLEX) /*&& (m_NbBands / 2 > 0)*/)
    {
    //m_NbBands /= 2;

    if (this->GetComponentType() == CSHORT)
      {
      m_BytePerPixel = 4;
      m_PxType->pixType = GDT_CInt16;
      }
    else if (this->GetComponentType() == CINT)
      {
      m_BytePerPixel = 8;
      m_PxType->pixType = GDT_CInt32;
      }
    else if (this->GetComponentType() == CFLOAT)
      {
      m_BytePerPixel = 8;
      m_PxType->pixType = GDT_CFloat32;
      }
    else if (this->GetComponentType() == CDOUBLE)
      {
      m_BytePerPixel = 16;
      m_PxType->pixType = GDT_CFloat64;
      }
    else
      {
      itkExceptionMacro(<< "This complex type is not defined :" << this->GetPixelTypeAsString(this->GetPixelType()) );
      }
    }
  else
    {
    if (this->GetComponentType() == CHAR)
      {
      m_BytePerPixel = 1;
      m_PxType->pixType = GDT_Byte;
      }
    else if (this->GetComponentType() == UCHAR)
      {
      m_BytePerPixel = 1;
      m_PxType->pixType = GDT_Byte;
      }
    else if (this->GetComponentType() == USHORT)
      {
      m_BytePerPixel = 2;
      m_PxType->pixType = GDT_UInt16;
      }
    else if (this->GetComponentType() == SHORT)
      {
      m_BytePerPixel = 2;
      m_PxType->pixType = GDT_Int16;
      }
    else if (this->GetComponentType() == INT)
      {
      m_BytePerPixel = 4;
      m_PxType->pixType = GDT_Int32;
      }
    else if (this->GetComponentType() == UINT)
      {
      m_BytePerPixel = 4;
      m_PxType->pixType = GDT_UInt32;
      }
    else if (this->GetComponentType() == FLOAT)
      {
      m_BytePerPixel = 4;
      m_PxType->pixType = GDT_Float32;
      }
    else if (this->GetComponentType() == DOUBLE)
      {
      m_BytePerPixel = 8;
      m_PxType->pixType = GDT_Float64;
      }
    else
      {
      m_BytePerPixel = 1;
      m_PxType->pixType = GDT_Byte;
      }
    }

  // Automatically set the Type to Binary for GDAL data
  this->SetFileTypeToBinary();

  driverShortName = FilenameToGdalDriverShortName(m_FileName);
  if (driverShortName == "NOT-FOUND")
    {
    itkExceptionMacro(
      << "GDAL Writing failed : the image file name '" << m_FileName.c_str() << "' is not recognized by GDAL.");
    }

  if (m_CanStreamWrite)
    {

    // Force tile mode for TIFF format. Tile mode is a lot more
    // efficient when writing huge tiffs
    if( driverShortName.compare("GTiff") == 0 )
      {
      otbMsgDevMacro(<< "Enabling TIFF Tiled mode")
      papszOptions = CSLAddNameValue( papszOptions, "TILED", "YES" );

      // Use a fixed tile size
      // Take as reference is a 256*256 short int 4 bands tile
      const unsigned int ReferenceTileSizeInBytes = 256 * 256 * 4 * 2;

      unsigned int nbPixelPerTile = ReferenceTileSizeInBytes / m_BytePerPixel / m_NbBands;
      unsigned int tileDimension = static_cast<unsigned int>( vcl_sqrt(static_cast<float>(nbPixelPerTile)) );

      // align the tile dimension to the next multiple of 16 (needed by TIFF spec)
      tileDimension = ( tileDimension + 15 ) / 16 * 16;

      otbMsgDevMacro(<< "Tile dimension : " << tileDimension << " * " << tileDimension)

      std::ostringstream oss;
      oss << tileDimension;
      papszOptions = CSLAddNameValue( papszOptions, "BLOCKXSIZE", oss.str().c_str() );
      papszOptions = CSLAddNameValue( papszOptions, "BLOCKYSIZE", oss.str().c_str() );
      }

    m_Dataset = GDALDriverManagerWrapper::GetInstance().Create(
                     driverShortName,
                     GetGdalWriteImageFileName(driverShortName, m_FileName),
                     m_Dimensions[0], m_Dimensions[1],
                     m_NbBands, m_PxType->pixType,
                     papszOptions);
    }
  else
    {
    // buffer casted in unsigned long cause under Win32 the adress
    // don't begin with 0x, the adress in not interpreted as
    // hexadecimal but alpha numeric value, then the conversion to
    // integer make us pointing to an non allowed memory block => Crash.
    std::ostringstream stream;
    stream << "MEM:::"
           <<  "DATAPOINTER=" << (unsigned long)(buffer) << ","
           <<  "PIXELS=" << m_Dimensions[0] << ","
           <<  "LINES=" << m_Dimensions[1] << ","
           <<  "BANDS=" << m_NbBands << ","
           <<  "DATATYPE=" << GDALGetDataTypeName(m_PxType->pixType) << ","
           <<  "PIXELOFFSET=" << m_BytePerPixel * m_NbBands << ","
           <<  "LINEOFFSET=" << m_BytePerPixel * m_NbBands * m_Dimensions[0] << ","
           <<  "BANDOFFSET=" << m_BytePerPixel;

    m_Dataset = GDALDriverManagerWrapper::GetInstance().Open(stream.str());
    }

  if (m_Dataset.IsNull())
    {
    itkExceptionMacro(
      << "GDAL Writing failed : Impossible to create the image file name '" << m_FileName << "'.");
    }

  /*----------------------------------------------------------------------*/
  /*-------------------------- METADATA ----------------------------------*/
  /*----------------------------------------------------------------------*/

  // Now initialize the itk dictionary
  itk::MetaDataDictionary& dict = this->GetMetaDataDictionary();
  std::ostringstream oss;
  GDALDataset* dataset = m_Dataset->GetDataSet();

  std::string projectionRef;
  itk::ExposeMetaData<std::string>(dict, MetaDataKey::ProjectionRefKey, projectionRef);

  /* -------------------------------------------------------------------- */
  /* Set the GCPs                                                          */
  /* -------------------------------------------------------------------- */
  const double Epsilon = 1E-10;
  if (projectionRef.empty()
      &&  (vcl_abs(m_Origin[0]) > Epsilon
           || vcl_abs(m_Origin[1]) > Epsilon
           || vcl_abs(m_Spacing[0] - 1) > Epsilon
           || vcl_abs(m_Spacing[1] - 1) > Epsilon) )
    {
    // See issue #303 :
    // If there is no ProjectionRef, and the GeoTransform is not the identity,
    // then saving also GCPs is undefined behavior for GDAL, and a WGS84 projection crs
    // is assigned arbitrarily
    itkWarningMacro(<< "Skipping GCPs saving to prevent GDAL from assigning a WGS84 projection ref to the file")
    }
  else
    {
    unsigned int gcpCount = 0;
    itk::ExposeMetaData<unsigned int>(dict, MetaDataKey::GCPCountKey, gcpCount);

    if (gcpCount > 0)
      {

      GDAL_GCP * gdalGcps = new GDAL_GCP[gcpCount];

      for (unsigned int gcpIndex = 0; gcpIndex < gcpCount; ++gcpIndex)
        {
        //Build the GCP string in the form of GCP_n
        std::ostringstream lStream;
        lStream << MetaDataKey::GCPParametersKey << gcpIndex;
        std::string key = lStream.str();

        OTB_GCP gcp;
        itk::ExposeMetaData<OTB_GCP>(dict, key, gcp);

        gdalGcps[gcpIndex].pszId = const_cast<char *>(gcp.m_Id.c_str());
        gdalGcps[gcpIndex].pszInfo = const_cast<char *>(gcp.m_Info.c_str());
        gdalGcps[gcpIndex].dfGCPPixel = gcp.m_GCPCol;
        gdalGcps[gcpIndex].dfGCPLine = gcp.m_GCPRow;
        gdalGcps[gcpIndex].dfGCPX = gcp.m_GCPX;
        gdalGcps[gcpIndex].dfGCPY = gcp.m_GCPY;
        gdalGcps[gcpIndex].dfGCPZ = gcp.m_GCPZ;
        }

      std::string gcpProjectionRef;
      itk::ExposeMetaData<std::string>(dict, MetaDataKey::GCPProjectionKey, gcpProjectionRef);

      dataset->SetGCPs(gcpCount, gdalGcps, gcpProjectionRef.c_str());

      delete[] gdalGcps;
      }
    }

  /* -------------------------------------------------------------------- */
  /* Set the projection coordinate system of the image : ProjectionRef    */
  /* -------------------------------------------------------------------- */
  if (!projectionRef.empty())
    {
    dataset->SetProjection(projectionRef.c_str());
    }

  /* -------------------------------------------------------------------- */
  /*  Set the six coefficients of affine geoTransform                     */
  /* -------------------------------------------------------------------- */
  itk::VariableLengthVector<double> geoTransform(6);
  /// Reporting origin and spacing
  geoTransform[0] = m_Origin[0];
  geoTransform[3] = m_Origin[1];
  geoTransform[1] = m_Spacing[0];
  geoTransform[5] = m_Spacing[1];

  // FIXME: Here component 1 and 4 should be replaced by the orientation parameters
  geoTransform[2] = 0.;
  geoTransform[4] = 0.;
  dataset->SetGeoTransform(const_cast<double*>(geoTransform.GetDataPointer()));

  /* -------------------------------------------------------------------- */
  /*      Report metadata.                                                */
  /* -------------------------------------------------------------------- */

  std::string              svalue = "";
  std::vector<std::string> keys = dict.GetKeys();
  MetaDataKey              key;

  for (unsigned int itkey = 0; itkey < keys.size(); ++itkey)
    {
    if (keys[itkey].compare(0, key.MetadataKey.length(), key.MetadataKey) == 0)
      {
      itk::ExposeMetaData<std::string>(dict, keys[itkey], svalue);
      unsigned int equalityPos = svalue.find_first_of('=');
      std::string  tag = svalue.substr(0, equalityPos);
      std::string  value = svalue.substr(equalityPos + 1);
      otbMsgDevMacro(<< "Metadata: " << tag << "=" << value);
      dataset->SetMetadataItem(tag.c_str(), value.c_str(), NULL);
      }
    }
  // END

  // Dataset info
  otbMsgDevMacro( << "**** WriteImageInformation() DATASET INFO: ****" );
  otbMsgDevMacro( << "Projection Ref: "<<dataset->GetProjectionRef() );
  double GT[6];
  if (dataset->GetGeoTransform(GT) == CE_None)
    {
    otbMsgDevMacro( <<"Geo Transform: "<< GT[0] << ", " << GT[1] << ", "
                                 << GT[2] << ", " << GT[3] << ", "
                                 << GT[4] << ", " << GT[5] );
    }
  else
    {
    otbMsgDevMacro( << "No Geo Transform: ");
    }

  otbMsgDevMacro( << "GCP Projection Ref: "<< dataset->GetGCPProjection() );
  otbMsgDevMacro( << "GCP Count: " << dataset->GetGCPCount() );

}

std::string GDALImageIO::FilenameToGdalDriverShortName(const std::string& name) const
{
  std::string extension;
  std::string gdalDriverShortName;

  // Get extension in lowercase
  extension = otb::System::SetToLower( System::GetExtension(name) );

  if      ( extension == "tif" || extension == "tiff" )
    gdalDriverShortName = "GTiff";
  else if ( extension == "hdr" )
    gdalDriverShortName = "ENVI";
  else if ( extension == "img" )
    gdalDriverShortName = "HFA";
  else if ( extension == "ntf" )
    gdalDriverShortName = "NITF";
  else if ( extension == "png" )
    gdalDriverShortName="PNG";
  else if ( extension == "jpg" || extension=="jpeg" )
    gdalDriverShortName="JPEG";
  else if ( extension == "pix" )
    gdalDriverShortName="PCIDSK";
  else
    gdalDriverShortName = "NOT-FOUND";

  return gdalDriverShortName;
}

std::string GDALImageIO::GetGdalWriteImageFileName(const std::string& gdalDriverShortName, const std::string& filename) const
{
  std::string gdalFileName;

  gdalFileName = filename;
  // Suppress hdr extension for ENVI format
  if (gdalDriverShortName == "ENVI")
    {
    gdalFileName = System::GetRootName(filename);
    }
  return gdalFileName;
}

bool GDALImageIO::GDALInfoReportCorner(const char * /*corner_name*/, double x, double y, double& GeoX, double& GeoY) const
{
  const char *pszProjection;
  double      adfGeoTransform[6];

  bool IsTrue;

  /* -------------------------------------------------------------------- */
  /*      Transform the point into georeferenced coordinates.             */
  /* -------------------------------------------------------------------- */
  if (m_Dataset->GetDataSet()->GetGeoTransform(adfGeoTransform) == CE_None)
    {
    pszProjection = m_Dataset->GetDataSet()->GetProjectionRef();

    GeoX = adfGeoTransform[0] + adfGeoTransform[1] * x
           + adfGeoTransform[2] * y;
    GeoY = adfGeoTransform[3] + adfGeoTransform[4] * x
           + adfGeoTransform[5] * y;
    IsTrue = true;
    }

  else
    {
    GeoX = x;
    GeoY = y;
    IsTrue = false;
    }

  return IsTrue;
}


} // end namespace otb
