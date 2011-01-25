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

#include "itkExceptionObject.h"

#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "otbMLCToCircularCoherencyDegreeImageFilter.h"
#include "otbSinclairImageFilter.h"
#include "otbSinclairToCovarianceFunctor.h"


int otbMLCToCircularCoherencyDegreeImageFilter(int argc, char * argv[])
{
  const char * inputFilename1  = argv[1];
  const char * inputFilename2  = argv[2];
  const char * inputFilename3  = argv[3];
  const char * inputFilename4  = argv[4];

  const char * outputFilename = argv[5];

  typedef double                   PixelType;
  typedef std::complex<PixelType>  InputPixelType;
  const unsigned int Dimension = 2;

  
  typedef otb::Image<InputPixelType,  Dimension>       InputImageType;
  typedef otb::VectorImage<InputPixelType, Dimension>  ImageType;
  typedef otb::VectorImage<PixelType, Dimension>       RealImageType;
  typedef otb::Functor::SinclairToCovarianceFunctor<
                      InputImageType::PixelType,
                      InputImageType::PixelType,
                      InputImageType::PixelType,
                      InputImageType::PixelType,
                      ImageType::PixelType>              FunctionType;

  typedef otb::SinclairImageFilter<InputImageType, InputImageType,
                      InputImageType, InputImageType,
                      ImageType, FunctionType >  SinclairToCovarianceFilterType;

  typedef otb::MLCToCircularCoherencyDegreeImageFilter<ImageType,RealImageType> FilterType;


  typedef otb::ImageFileReader<InputImageType>  ReaderType;
  typedef otb::ImageFileWriter<RealImageType> WriterType;

  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();
  ReaderType::Pointer reader3 = ReaderType::New();
  ReaderType::Pointer reader4 = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader1->SetFileName(inputFilename1);
  reader2->SetFileName(inputFilename2);
  reader3->SetFileName(inputFilename3);
  reader4->SetFileName(inputFilename4);

  SinclairToCovarianceFilterType::Pointer sinclairToCovarianceFilter
                                       = SinclairToCovarianceFilterType::New();
  sinclairToCovarianceFilter->SetInputHH(reader1->GetOutput());
  sinclairToCovarianceFilter->SetInputHV(reader2->GetOutput());
  sinclairToCovarianceFilter->SetInputVH(reader3->GetOutput());
  sinclairToCovarianceFilter->SetInputVV(reader4->GetOutput());

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(sinclairToCovarianceFilter->GetOutput());

  writer->SetFileName(outputFilename);
  writer->SetInput(filter->GetOutput());
  writer->Update();

  return EXIT_SUCCESS;
}