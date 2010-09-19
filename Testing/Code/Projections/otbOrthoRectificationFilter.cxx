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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

// iostream is used for general output
#include <iostream>
#include <stdlib.h>

#include "otbMacro.h"
#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbStreamingImageFileWriter.h"
#include "otbMapProjections.h"

#include "otbOrthoRectificationFilter.h"
#include "otbMapProjections.h"

#include "init/ossimInit.h"

int otbOrthoRectificationFilter(int argc, char* argv[])
{
  ossimInit::instance()->initialize(argc, argv);
  if (argc != 12)
    {
    std::cout << argv[0] <<
    " <input filename> <output filename> <origin easting> <origin northing> <x size> <y size> <x spacing> <y spacing> <UTM zone> <UTM hemisphere>"
              << std::endl;

    return EXIT_FAILURE;
    }

  typedef otb::Image<double, 2>                                                     ImageType;
  typedef otb::ImageFileReader<ImageType>                                           ReaderType;
  typedef otb::StreamingImageFileWriter<ImageType>                                  WriterType;
  typedef otb::UtmInverseProjection                                                 UtmMapProjectionType;
  typedef otb::OrthoRectificationFilter<ImageType, ImageType, UtmMapProjectionType> OrthoRectifFilterType;

  //Allocate pointer
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  OrthoRectifFilterType::Pointer orthoRectifFilter = OrthoRectifFilterType::New();
  UtmMapProjectionType::Pointer  utmMapProjection = UtmMapProjectionType::New();

  // Set parameters ...
  reader->SetFileName(argv[1]);
  writer->SetFileName(argv[2]);

  reader->GenerateOutputInformation();
  std::cout << reader->GetOutput() << std::endl;

  orthoRectifFilter->SetInput(reader->GetOutput());

  ImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;
  orthoRectifFilter->SetOutputStartIndex(start);

  ImageType::SizeType size;
  size[0] = atoi(argv[5]);      // X size
  size[1] = atoi(argv[6]);            //Y size
  orthoRectifFilter->SetOutputSize(size);

  ImageType::SpacingType spacing;
  spacing[0] = atof(argv[7]);
  spacing[1] = atof(argv[8]);
  orthoRectifFilter->SetOutputSpacing(spacing);

  ImageType::PointType origin;
  origin[0] = strtod(argv[3], NULL);         //Origin easting
  origin[1] = strtod(argv[4], NULL);         //Origin northing
  orthoRectifFilter->SetOutputOrigin(origin);

  utmMapProjection->SetZone(atoi(argv[9]));
  utmMapProjection->SetHemisphere(argv[10][0]);
  orthoRectifFilter->SetMapProjection(utmMapProjection);
 
  // Deformation Field spacing
  ImageType::SpacingType  gridSpacing;
  gridSpacing[0] = atof(argv[11]);
  gridSpacing[1] = -atof(argv[11]);
  orthoRectifFilter->SetDeformationFieldSpacing(gridSpacing);
  
  writer->SetInput(orthoRectifFilter->GetOutput());
  writer->SetTilingStreamDivisions(4);
  writer->Update();

  return EXIT_SUCCESS;
}
