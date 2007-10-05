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
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbMatrixTransposeMatrixImageFilter.h"


int otbMatrixTransposeMatrixImageFilterNew( int argc, char * argv[] )
{
  try 
    { 
      const unsigned int Dimension = 2;
      typedef unsigned char InputPixelType;
      typedef unsigned char OutputPixelType;
      
      typedef otb::VectorImage<InputPixelType,Dimension>       InputImage1Type;
      typedef otb::VectorImage<InputPixelType,Dimension>       InputImage2Type;
      typedef otb::MatrixTransposeMatrixImageFilter<InputImage1Type, InputImage2Type >  MatrixTransposeMatrixImageFilterType;
      
      // Instantiation
      MatrixTransposeMatrixImageFilterType::Pointer filter = MatrixTransposeMatrixImageFilterType::New();
      
    } 
  
  catch( itk::ExceptionObject & err ) 
    { 
      std::cout << "Exception itk::ExceptionObject levee !" << std::endl; 
      std::cout << err << std::endl; 
      return EXIT_FAILURE;
    } 
  catch( ... ) 
    { 
      std::cout << "Exception levee inconnue !" << std::endl; 
      return EXIT_FAILURE;
    } 
  
  
  return EXIT_SUCCESS;
}


