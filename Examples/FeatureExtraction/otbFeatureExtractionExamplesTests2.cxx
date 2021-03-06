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

// this file defines the otbMultiScaleTest for the test driver
// and all it expects is that you have a function called RegisterTests

#include <iostream>
#include "otbTestMain.h"

void RegisterTests()
{
  REGISTER_TEST(ExtractRoadByStepsExampleTest);
  REGISTER_TEST(ExtractRoadExampleTest);
  REGISTER_TEST(SeamCarvingExampleTest);
  REGISTER_TEST(SeamCarvingOtherExampleTest);
  REGISTER_TEST(CloudDetectionExampleTest);
#ifdef OTB_USE_MAPNIK
  REGISTER_TEST(LineSegmentDetectorExampleTest);
  REGISTER_TEST(RightAngleDetectionExample);
#endif
}

#undef main
#define main ExtractRoadByStepsExampleTest
#include "ExtractRoadByStepsExample.cxx"

#undef main
#define main ExtractRoadExampleTest
#include "ExtractRoadExample.cxx"

#undef main
#define main SeamCarvingExampleTest
#include "SeamCarvingExample.cxx"

#undef main
#define main SeamCarvingOtherExampleTest
#include "SeamCarvingOtherExample.cxx"

#undef main
#define main CloudDetectionExampleTest
#include "CloudDetectionExample.cxx"

#ifdef OTB_USE_MAPNIK
#undef main
#define main LineSegmentDetectorExampleTest
#include "LineSegmentDetectorExample.cxx"
#undef main
#define main RightAngleDetectionExample
#include "RightAngleDetectionExample.cxx"
#endif
