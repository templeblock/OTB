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

#include "otbImageListToVectorImageFilter.h"
#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbImageList.h"

int otbImageListToVectorImageFilterNew(int argc, char * argv[])
{
  try
    {
      const unsigned int Dimension = 2;
      typedef unsigned char PixelType;
     
      typedef otb::Image<PixelType,Dimension> ImageType;
      typedef otb::VectorImage<PixelType,Dimension> VectorImageType;
      typedef otb::ImageList<ImageType> ImageListType;
      
      typedef otb::ImageListToVectorImageFilter<ImageListType,VectorImageType> ImageListToVectorImageFilterType;

      // Instantiating object
      ImageListToVectorImageFilterType::Pointer object = ImageListToVectorImageFilterType::New();
    }

  catch( itk::ExceptionObject & err ) 
    { 
    std::cout << "Exception itk::ExceptionObject thrown !" << std::endl; 
    std::cout << err << std::endl; 
    return EXIT_FAILURE;
    } 

  catch( ... ) 
    { 
    std::cout << "Unknown exception thrown !" << std::endl; 
    return EXIT_FAILURE;
    } 
  return EXIT_SUCCESS;
}
