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

#ifndef __otbThresholdImageToPointSetFilter_txx
#define __otbThresholdImageToPointSetFilter_txx

#include "otbThresholdImageToPointSetFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkProgressReporter.h"
#include "otbMacro.h"

namespace otb
{

template <class TInputImage, class TOutputPointSet>
ThresholdImageToPointSetFilter<TInputImage, TOutputPointSet>
::ThresholdImageToPointSetFilter()
{
  m_LowerThreshold = itk::NumericTraits<InputPixelType>::NonpositiveMin();
  m_UpperThreshold = itk::NumericTraits<InputPixelType>::max();
}


//template <class TInputImage, class TOutputPointSet>
//void
//ThresholdImageToPointSetFilter<TInputImage, TOutputPointSet>
//::GenerateData()
//{
//  InputImageConstPointer   inputPtr  = this->GetInput(0);
//  OutputPointSetPointer    outputPtr = this->GetOutput();
//
//  unsigned int pointId = 0;
//  typename OutputPointSetType::PointType  position;
//
//  outputPtr->Initialize();
//
//  typedef itk::ImageRegionConstIterator<TInputImage> InputIterator;
//  InputIterator  inIt(inputPtr, inputPtr->GetRequestedRegion() );
//
//  // walk the regions, threshold each pixel
//  while ( !inIt.IsAtEnd() )
//  {
//
//    const InputPixelType value = inIt.Get();
//    const IndexType index = inIt.GetIndex();
//
//    if ((value >= m_LowerThreshold) && (value <= m_UpperThreshold))
//    {
//      position[0] = index[0];
//      position[1] = index[1];
//
//      outputPtr->SetPoint(pointId,position);
//
//      pointId++;
//
//    }
//    ++inIt;
//  }
//}


template <class TInputImage, class TOutputPointSet>
void
ThresholdImageToPointSetFilter<TInputImage, TOutputPointSet>
::ThreadedGenerateData(const InputImageRegionType &inputRegionForThread, int threadId)
{
  otbMsgDevMacro(<< "Processing thread: " << threadId);
  this->m_PointContainerPerThread[threadId] = PointsContainerType::New();
  InputImageConstPointer  inputPtr = this->GetInput();

  // Define the iterators
  itk::ImageRegionConstIterator<TInputImage>  inputIt(inputPtr, inputRegionForThread);

  itk::ProgressReporter progress(this, threadId, inputRegionForThread.GetNumberOfPixels());

  typename OutputPointSetType::PointType  position;

  inputIt.GoToBegin();

  while( !inputIt.IsAtEnd() )
  {
    const InputPixelType value = inputIt.Get();
    if ((value >= m_LowerThreshold) && (value <= m_UpperThreshold))
    {
      //FIXME: non valid for image with dim > 2
      const IndexType index = inputIt.GetIndex();
      position[0] = index[0];
      position[1] = index[1];
      this->m_PointContainerPerThread[threadId]->push_back(position);

    }
    ++inputIt;
    progress.CompletedPixel();  // potential exception thrown here
  }
}

/**
 * Standard "PrintSelf" method
 */
template <class TInputImage, class TOutputPointSet>
void
ThresholdImageToPointSetFilter<TInputImage, TOutputPointSet>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "LowerThreshold : " << m_LowerThreshold << std::endl;
  os << indent << "UpperThreshold : " << m_UpperThreshold << std::endl;
}


} // end namespace otb


#endif
