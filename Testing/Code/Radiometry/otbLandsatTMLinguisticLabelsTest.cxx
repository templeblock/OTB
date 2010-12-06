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
#include "itkExceptionObject.h"
#include "itkFixedArray.h"
#include "otbLandsatTMIndices.h"

int otbLandsatTMLinguisticLabels(int argc, char * argv[])
{
  typedef double                           OutputPixelType;
  typedef itk::FixedArray< double, 8 >     InputPixelType;

  typedef otb::Functor::LandsatTM::LinguisticVariables<InputPixelType> LVFunctorType;

  LVFunctorType lvFunct = LVFunctorType();

  double TM1 = (::atof(argv[1]));
  double TM2 = (::atof(argv[2]));
  double TM3 = (::atof(argv[3]));
  double TM4 = (::atof(argv[4]));
  double TM5 = (::atof(argv[5]));
  double TM61 = (::atof(argv[6]));
  double TM62 = (::atof(argv[7]));
  double TM7 = (::atof(argv[8]));

  unsigned int lo = LVFunctorType::Low;
  unsigned int me = LVFunctorType::Medium;
  unsigned int hi = LVFunctorType::High;


  InputPixelType pixel;
  pixel[0] = TM1;
  pixel[1] = TM2;
  pixel[2] = TM3;
  pixel[3] = TM4;
  pixel[4] = TM5;
  pixel[5] = TM61;
  pixel[6] = TM62;
  pixel[7] = TM7;


  OutputPixelType bright = otb::Functor::LandsatTM::Bright<InputPixelType, OutputPixelType>()( pixel );
  OutputPixelType vis = otb::Functor::LandsatTM::Vis<InputPixelType, OutputPixelType>()( pixel );
  OutputPixelType nir = otb::Functor::LandsatTM::NIR<InputPixelType, OutputPixelType>()( pixel );
  OutputPixelType mir1 = otb::Functor::LandsatTM::MIR1<InputPixelType, OutputPixelType>()( pixel );
  OutputPixelType mir2 = otb::Functor::LandsatTM::MIR2<InputPixelType, OutputPixelType>()( pixel );
  OutputPixelType tir = otb::Functor::LandsatTM::TIR<InputPixelType, OutputPixelType>()( pixel );
  OutputPixelType mirtir = otb::Functor::LandsatTM::MIRTIR<InputPixelType, OutputPixelType>()( pixel );
  OutputPixelType ndsivis = otb::Functor::LandsatTM::NDSIVis<InputPixelType, OutputPixelType>()( pixel );
  OutputPixelType ndbbbi = otb::Functor::LandsatTM::NDBBBI<InputPixelType, OutputPixelType>()( pixel );
  OutputPixelType ndvi = otb::Functor::LandsatTM::NDVI<InputPixelType, OutputPixelType>()( pixel );
  OutputPixelType ndbsi = otb::Functor::LandsatTM::NDBSI<InputPixelType, OutputPixelType>()( pixel );

  LVFunctorType::OutputPixelType result = lvFunct( pixel );

  

  // double goodResult = (TM1-TM5)/(TM1+TM5+ndbbbiFunct.GetEpsilonToBeConsideredAsZero());

  // std::cout << goodResult ;


  // double result = ndbbbiFunct(pixel);

  // std::cout << " " << result << std::endl;

  // if( result!=goodResult ) return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}
