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
#ifndef __otbDEMToOrthoImageGenerator_h
#define __otbDEMToOrthoImageGenerator_h

#include "itkIndent.h"
#include "otbDEMToImageGenerator.h"
#include "otbImage.h"
#include <iostream>
#include <stdio.h>
//#include "elevation/ossimElevManager.h"
#include "otbDEMHandler.h"
#include "base/ossimFilename.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkIndent.h"


namespace otb
{
/** \class DEMToOrthoImageGenerator
 *
 * \brief Class for Reading a DEM data
 *
 * This class is based on ossimElevManager. It takes in input the UL and LR map coordinates and the spacing.
 * Handle DTED and SRTM formats.
 * \ingroup Images
 *
 */
template <class TDEMImage, class TMapProjection>
class ITK_EXPORT DEMToOrthoImageGenerator:
      public otb::DEMToImageGenerator<TDEMImage>
{
public :
  /** Standard class typedefs. */
  typedef itk::Indent                  Indent;
  typedef TDEMImage                          DEMImageType;
  typedef typename DEMImageType::Pointer              DEMImagePointerType;
  typedef typename DEMImageType::PixelType                           PixelType;

  typedef TMapProjection                                                  MapProjectionType;
  typedef typename MapProjectionType::Pointer                             MapProjectionPointerType;

  typedef DEMToOrthoImageGenerator                                              Self;
  typedef otb::DEMToImageGenerator<DEMImageType> Superclass;
  typedef itk::SmartPointer<Self>                                    Pointer;
  typedef itk::SmartPointer<const Self>                              ConstPointer;
  typedef Image<PixelType,2>                         OutputImageType;

  typedef typename Superclass::Pointer                  OutputImagePointer;
  typedef typename OutputImageType::SpacingType               SpacingType;
  typedef typename OutputImageType::SizeType             SizeType;
  typedef typename OutputImageType::PointType             PointType;
  typedef typename OutputImageType::IndexType             IndexType;
  typedef typename Superclass::OutputImageRegionType           OutputImageRegionType;
  typedef itk::ImageRegionIteratorWithIndex< DEMImageType >      ImageIteratorType;

  typedef otb::DEMHandler                                                 DEMHandlerType;
  typedef typename DEMHandlerType::Pointer                               DEMHandlerPointerType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DEMToOrthoImageGenerator,ImageSource);

  /** Set/Get the Output Origin coordinates. */
  itkSetMacro(OutputOrigin,PointType);
  itkGetConstReferenceMacro(OutputOrigin,PointType);

  /** Set/Get the Output Size. */
  itkSetMacro(OutputSize,SizeType);
  itkGetConstReferenceMacro(OutputSize,SizeType);

  /** Set/Get the Output Spacing. */
  itkSetMacro(OutputSpacing,SpacingType);
  itkGetConstReferenceMacro(OutputSpacing,SpacingType);

  /** Set/Get the Default Unknown Value. */
  itkSetMacro(DefaultUnknownValue,PixelType);
  itkGetConstReferenceMacro(DefaultUnknownValue,PixelType);

  /** Set/Get the projection*/
  itkSetMacro(MapProjection, MapProjectionPointerType);
  itkGetMacro(MapProjection, MapProjectionPointerType);

protected:
  DEMToOrthoImageGenerator();
  ~DEMToOrthoImageGenerator();

  void PrintSelf(std::ostream& os, Indent indent) const;
  void GenerateData();
  virtual void GenerateOutputInformation();

  DEMHandlerPointerType m_DEMHandler;
  PointType m_OutputOrigin;
  SpacingType  m_OutputSpacing;
  SizeType m_OutputSize;
  PixelType m_DefaultUnknownValue;
  MapProjectionPointerType m_MapProjection;

private:
  DEMToOrthoImageGenerator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbDEMToOrthoImageGenerator.txx"
#endif

#endif
