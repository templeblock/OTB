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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "otbTestMain.h"

void RegisterTests()
{
  REGISTER_TEST(otbMLCToCoherencyImageFilterNew);
  REGISTER_TEST(otbMLCToCoherencyImageFilter);
  REGISTER_TEST(otbMLCToCoherencyDegreeImageFilterNew);
  REGISTER_TEST(otbMLCToCoherencyDegreeImageFilter);
  REGISTER_TEST(otbMLCToCircularCoherencyDegreeImageFilterNew);
  REGISTER_TEST(otbMLCToCircularCoherencyDegreeImageFilter);
  REGISTER_TEST(otbReciprocalHAlphaImageFilterNew);
  REGISTER_TEST(otbReciprocalHAlphaImageFilter);
  REGISTER_TEST(otbReciprocalCoherencyToMuellerImageFilterNew);
  REGISTER_TEST(otbReciprocalCoherencyToMuellerImageFilter);
  REGISTER_TEST(otbMuellerToCircularPolarisationImageFilterNew);
  REGISTER_TEST(otbMuellerToCircularPolarisationImageFilter);
  REGISTER_TEST(otbMuellerToPolarisationDegreeAndPowerImageFilterNew);
  REGISTER_TEST(otbMuellerToPolarisationDegreeAndPowerImageFilter);
  REGISTER_TEST(otbMuellerToMLCImageFilterNew);
  REGISTER_TEST(otbMuellerToMLCImageFilter);
  REGISTER_TEST(otbHermitianEigenAnalysisTest);
}