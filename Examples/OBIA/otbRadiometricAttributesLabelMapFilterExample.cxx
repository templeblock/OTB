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

//  Software Guide : BeginCommandLineArgs
//    INPUTS: {qb_RoadExtract2.tif}
//    OUTPUTS: {OBIARadiometricAttribute1.tif}
//    STATS::Ndvi::Mean 0 -0.3 16 16 10 1.0
//  Software Guide : EndCommandLineArgs

//  Software Guide : BeginLatex
//
//  This example shows the basic approach to perform object based analysis on a image.
//  The input image is firstly segmented using the \doxygen{otb}{MeanShiftImageFilter}
//  Then each segmented region is converted to a Map of labeled objects.
//  After the \doxygen{otb}{RadiometricAttributesLabelMapFilter}  computes 
//  radiometric attributes for each object.
//  Images are supposed to be standard 4-bands image (B,G,R,NIR). The
//  index of each channel can be set via the Set***ChannelIndex()
//  accessors.
//  
//  This filter internally applies the
//  StatisticsAttributesLabelMapFilter to the following features: 
  //  \begin{itemize}
  //  \item GEMI
  //  \item NDVI
  //  \item IR
  //  \item IC
  //  \item IB
  //  \item NDWI2
  //  \item Intensity
  //  \item and original B, G, R and NIR channels
  //  \end{itemize}
//  Here we use the  \doxygen{otb}{AttributesMapOpeningLabelMapFilter} to extract vegetated areas.
//  Let's get to the source code explanation.  
//
//  Software Guide : EndLatex


#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

#include "otbMeanShiftImageFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "otbAttributesMapLabelObject.h"
#include "itkLabelMap.h"
#include "otbShapeAttributesLabelMapFilter.h"
#include "otbStatisticsAttributesLabelMapFilter.h"
#include "otbRadiometricAttributesLabelMapFilter.h"
#include "otbAttributesMapOpeningLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"

int main(int argc, char * argv[])
{
  if(argc != 10)
    {
      std::cerr<<"Usage: "<<argv[0]<<" reffname outfname attribute_name lowerThan tresh spatialRadius rangeRadius minregionsize scale"<<std::endl;
      return EXIT_FAILURE;
    }

  const char * reffname = argv[1];
  const char * outfname = argv[2];
  const char * attr     = argv[3];
  bool  lowerThan       = atoi(argv[4]);
  double thresh         = atof(argv[5]);
  
  const unsigned int spatialRadius          = atoi(argv[6]);
  const double       rangeRadius            = atof(argv[7]);
  const unsigned int minRegionSize          = atoi(argv[8]);
  const double       scale                  = atoi(argv[9]);

  const unsigned int Dimension = 2;

  // Labeled image type
  typedef unsigned short                                                 LabelType;
  typedef double                                                         PixelType;
  typedef otb::Image<LabelType,Dimension>                                LabeledImageType;
  typedef otb::Image<PixelType,Dimension>                                ImageType;
  typedef otb::VectorImage<PixelType,Dimension>                          VectorImageType;
  typedef otb::ImageFileReader<LabeledImageType>                         LabeledReaderType;
  typedef otb::ImageFileReader<ImageType>                                ReaderType;
  typedef otb::ImageFileReader<VectorImageType>                          VectorReaderType;
  typedef otb::ImageFileWriter<LabeledImageType>                         WriterType;
  // Label map typedef 
  typedef otb::AttributesMapLabelObject<LabelType,Dimension,double>      LabelObjectType;
  typedef itk::LabelMap<LabelObjectType>                                 LabelMapType;
  typedef itk::LabelImageToLabelMapFilter<LabeledImageType,LabelMapType> LabelMapFilterType;
  typedef otb::ShapeAttributesLabelMapFilter<LabelMapType>               ShapeLabelMapFilterType;
  typedef otb::StatisticsAttributesLabelMapFilter<LabelMapType,ImageType> StatisticsLabelMapFilterType;
  typedef otb::RadiometricAttributesLabelMapFilter<LabelMapType,VectorImageType> RadiometricLabelMapFilterType;
  typedef otb::AttributesMapOpeningLabelMapFilter<LabelMapType>          OpeningLabelMapFilterType;
  typedef itk::LabelMapToLabelImageFilter<LabelMapType,LabeledImageType> LabelMapToLabeledImageFilterType;


  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(reffname);

  VectorReaderType::Pointer vreader = VectorReaderType::New();
  vreader->SetFileName(reffname);
    //  Software Guide : BeginLatex
  //
  // Firstly, segment the input image by using the Mean Shift algorithm (see \ref{sec:MeanShift} for deeper
  // explanations).
  //
  //  Software Guide : EndLatex
  
  // Software Guide : BeginCodeSnippet
  typedef otb::MeanShiftImageFilter<ImageType,ImageType, LabeledImageType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetSpatialRadius(spatialRadius);
  filter->SetRangeRadius(rangeRadius);
  filter->SetMinimumRegionSize(minRegionSize);
  filter->SetScale(scale);
  // Software Guide : EndCodeSnippet
  
  //  Software Guide : BeginLatex
  //
  // The \doxygen{otb}{MeanShiftImageFilter} type is instantiated using the images
  // types.
  //
  //  Software Guide : EndLatex
  
  // Software Guide : BeginCodeSnippet
  filter->SetInput(reader->GetOutput());       
  // Software Guide : EndCodeSnippet

  //  Software Guide : BeginLatex
  //
  // The \doxygen{itk}{LabelImageToLabelMapFilter} type is instantiated using the output
  // of the \doxygen{otb}{MeanShiftImageFilter}. This filter produces a labeled image 
  // where each segmented region have a unique label.
  //
  //  Software Guide : EndLatex
  
  // Software Guide : BeginCodeSnippet
  LabelMapFilterType::Pointer labelMapFilter = LabelMapFilterType::New();
  labelMapFilter->SetInput(filter->GetLabeledClusteredOutput());
  labelMapFilter->SetBackgroundValue(itk::NumericTraits<LabelType>::min());
  // Software Guide : EndCodeSnippet
  
  ShapeLabelMapFilterType::Pointer shapeLabelMapFilter = ShapeLabelMapFilterType::New();
  shapeLabelMapFilter->SetInput(labelMapFilter->GetOutput());
  
  StatisticsLabelMapFilterType::Pointer statisticsLabelMapFilter = StatisticsLabelMapFilterType::New();
  statisticsLabelMapFilter->SetInput1(shapeLabelMapFilter->GetOutput());
  statisticsLabelMapFilter->SetInput2(reader->GetOutput());
  
  statisticsLabelMapFilter->Update();
  //  Software Guide : BeginLatex
  //
  // Instantiate the  \doxygen{otb}{RadiometricAttributesLabelMapFilter} to
  // compute radiometric valuee on each label object.
  //
  //  Software Guide : EndLatex
  
  // Software Guide : BeginCodeSnippet
  RadiometricLabelMapFilterType::Pointer radiometricLabelMapFilter = RadiometricLabelMapFilterType::New();
  radiometricLabelMapFilter->SetInput1(statisticsLabelMapFilter->GetOutput());
  radiometricLabelMapFilter->SetInput2(vreader->GetOutput());
  // Software Guide : EndCodeSnippet

  //  Software Guide : BeginLatex
  // 
  // Then, we specify the red and the near infrared channels 
  // By default, images are supposed to be standard 4-bands 
  // image (B,G,R,NIR). The index of each channel can 
  // be set via the Set***ChannelIndex() accessors.
  //
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  radiometricLabelMapFilter->SetRedChannelIndex(2);
  radiometricLabelMapFilter->SetNIRChannelIndex(3);
  radiometricLabelMapFilter->Update();
  // Software Guide : EndCodeSnippet

  //  Software Guide : BeginLatex
  // 
  // The \doxygen{otb}{AttributesMapOpeningLabelMapFilter} will proceed the selection. 
  // There are three parameters. \code{AttributeName} specifies the radiometric attribute, \code{Lambda} 
  // controls the thresholding of the input and \code{ReverseOrdering} make this filter to remove the 
  // object with an attribute value greater than \code{Lambda} instead.   
  // 
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  OpeningLabelMapFilterType::Pointer opening = OpeningLabelMapFilterType::New();
  opening->SetInput(radiometricLabelMapFilter->GetOutput());
  opening->SetAttributeName(attr);
  opening->SetLambda(thresh);
  opening->SetReverseOrdering(lowerThan);
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  // 
  //  Then, Label objects selected are transform in a Label Image using the 
  //  \doxygen{itk}{LabelMapToLabelImageFilter}.  
  // 
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  LabelMapToLabeledImageFilterType::Pointer labelMap2LabeledImage = LabelMapToLabeledImageFilterType::New();
  labelMap2LabeledImage->SetInput(opening->GetOutput());
  // Software Guide : EndCodeSnippet


  // Software Guide : BeginLatex
  //
  // And finally, we declare the writer and call its \code{Update()} method to
  // trigger the full pipeline execution.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outfname);
  writer->SetInput(labelMap2LabeledImage->GetOutput());
  writer->Update();
  // Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}

  // Software Guide : BeginLatex
  //
  // Figure~\ref{fig:RADIOMETRIC_LABEL_MAP_FILTER} shows the result of applying
  // the object selection based on radiometric attributes. 
  // \begin{figure} \center
  // \includegraphics[width=0.44\textwidth]{qb_RoadExtract2.eps}
  // \includegraphics[width=0.44\textwidth]{OBIARadiometricAttribute1.eps}
  // \itkcaption[Object based extraction based on ]{From left to right : original image, vegetation mask resulting from processing.}
  // \label{fig:RADIOMETRIC_LABEL_MAP_FILTER}
  // \end{figure}
  //
  // Software Guide : EndLatex
