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

// this file defines the otbCommonTest for the test driver
// and all it expects is that you have a function called RegisterTests


#include "otbTestMain.h"

void RegisterTests()
{
  REGISTER_TEST(otbRAndBAndNIRVegetationIndexImageFilterNew);
  REGISTER_TEST(otbRAndBAndNIRVegetationIndexImageFilter);
  REGISTER_TEST(otbMultiChannelRAndBAndNIRVegetationIndexImageFilterNew);
  REGISTER_TEST(otbMultiChannelRAndBAndNIRVegetationIndexImageFilter);
  REGISTER_TEST(otbImageToLuminanceImageFilterNew);
  REGISTER_TEST(otbImageToLuminanceImageFilter);
  REGISTER_TEST(otbImageToLuminanceImageFilterAuto);
  REGISTER_TEST(otbLuminanceToReflectanceImageFilterNew);
  REGISTER_TEST(otbLuminanceToReflectanceImageFilter);
  REGISTER_TEST(otbLuminanceToReflectanceImageFilterAuto);
  REGISTER_TEST(otbImageToReflectanceImageFilterNew);
  REGISTER_TEST(otbImageToReflectanceImageFilter);
  REGISTER_TEST(otbImageToReflectanceImageFilterAuto);
}
