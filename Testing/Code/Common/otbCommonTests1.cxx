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
  REGISTER_TEST(otbSystemTest);
  REGISTER_TEST(otbConfigurationTest);
  REGISTER_TEST(otbConfigurationTestDEM);
  REGISTER_TEST(otbTestCommandLineArgumentParser);
//REGISTER_TEST(otbTestCommandLineArgumentParserGUI);
  REGISTER_TEST(otbTestCommandLineArgumentParserHelp);
  REGISTER_TEST(otbTestCommandLineArgumentParserList);
  REGISTER_TEST(otbTestCommandLineArgumentParserWithError);
  REGISTER_TEST(otbParseHdfSubsetName);
  REGISTER_TEST(otbParseHdfFileName);
}
