#include "otbVectorDataFileReader.h"
#include "otbImageFileWriter.h"
#include "otbVectorData.h"
#include "otbVectorDataProjectionFilter.h"
#include "otbVectorDataExtractROI.h"
#include <fstream>
#include <iostream>

#include "itkRGBAPixel.h"
#include "otbImage.h"
#include "otbVectorDataToImageFilter.h"

//  ./mapnikOTBClasses /home/christop/OTB/trunk/OTB-Data/Input/waterways.shp output.png
//  ./mapnikOTBClasses ~/OTB/trunk/OTB-Data/Input/ToulouseRoad-examples.shp output.png
//  ./mapnikOTBClasses ~/OTB/trunk/OTB-Data/LargeInput/VECTOR/MidiPyrenees/roads.shp output.png

int main(int argc, char * argv[])
{

  //Read the vector data
  typedef otb::VectorData<> VectorDataType;
  typedef otb::VectorDataFileReader<VectorDataType> VectorDataFileReaderType;
  VectorDataFileReaderType::Pointer reader = VectorDataFileReaderType::New();

  reader->SetFileName(argv[1]);

  //Reproject the vector data in the proper projection
  typedef otb::VectorDataProjectionFilter<VectorDataType, VectorDataType> ProjectionFilterType;
  ProjectionFilterType::Pointer projection = ProjectionFilterType::New();
  projection->SetInput(reader->GetOutput());
//   projection->Update();
  std::string projectionRefWkt ="PROJCS[\"UTM Zone 31, Northern Hemisphere\",GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",3],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1]]";

  projection->SetOutputProjectionRef(projectionRefWkt);




  //Convert the vector data into an image
  typedef itk::RGBAPixel< unsigned char > PixelType;
  typedef otb::Image<PixelType,2> ImageType;

  ImageType::SizeType size;
//   size[0] = 1000;
//   size[1] = 1000;
  size[0] = 500;
  size[1] = 500;

  ImageType::PointType origin;
//   origin[0] = 1.3769;//UL lon
//   origin[1] = 43.5455;//UL lat
  origin[0] = 374149.980555821;//UL easting
  origin[1] = 4829183.99443839;//UL northing

  ImageType::SpacingType spacing;
//   spacing[0] = 0.00002;
//   spacing[1] = -0.00002;
  spacing[0] = 0.6;
  spacing[1] = -0.6;

  typedef otb::RemoteSensingRegion<double> RegionType;
  RegionType region;
  RegionType::SizeType sizeInUnit;
  sizeInUnit[0] = size[0]*spacing[0];
  sizeInUnit[1] = size[1]*spacing[1];
  region.SetSize(sizeInUnit);
  region.SetOrigin(origin);
  region.SetRegionProjection(projectionRefWkt);

  typedef otb::VectorDataExtractROI<VectorDataType> ExtractROIType;
  ExtractROIType::Pointer extractROI = ExtractROIType::New();
  extractROI->SetRegion(region);
  extractROI->SetInput(reader->GetOutput());

  typedef otb::VectorDataToImageFilter<VectorDataType, ImageType> VectorDataToImageFilterType;
  VectorDataToImageFilterType::Pointer vectorDataRendering = VectorDataToImageFilterType::New();
  vectorDataRendering->SetInput(extractROI->GetOutput());

  vectorDataRendering->SetSize(size);
  vectorDataRendering->SetOrigin(origin);
  vectorDataRendering->SetSpacing(spacing);


  //Save the image in a file
  typedef otb::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(vectorDataRendering->GetOutput());
  writer->SetFileName(argv[2]);
  writer->Update();



  return EXIT_SUCCESS;
}