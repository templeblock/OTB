/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Copyright (c) CS Systemes d'information. All rights reserved.
  See CSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbSarRadiometricCalibrationFunction_h
#define __otbSarRadiometricCalibrationFunction_h

#include "itkNumericTraits.h"
#include "itkImageFunction.h"
#include "otbSarRadiometricCalibrationFunctor.h"
#include "otbSarParametricMapFunction.h"

namespace otb
{

/**
 * \class SarRadiometricCalibrationFunction
 * \brief Calculate the density pixel of  in the neighborhood of a pixel
 *
 * Calculate the backscatter for the given pixel 
 * 
 * If called with a ContinuousIndex or Point, the calculation is performed
 * at the nearest neighbor.
 *
 * This class is templated over the input image type and the
 * coordinate representation type (e.g. float or double ).
 *
 * \ingroup ImageFunctions
 */
 
template <class TInputImage, class TCoordRep = float>
class ITK_EXPORT SarRadiometricCalibrationFunction :
  public itk::ImageFunction<TInputImage, typename itk::NumericTraits<typename TInputImage::PixelType>::RealType,
      TCoordRep>
{
public:
  /** Standard class typedefs. */
  typedef SarRadiometricCalibrationFunction Self;
  typedef itk::ImageFunction<TInputImage, typename itk::NumericTraits<typename TInputImage::PixelType>::RealType,
      TCoordRep>                                          Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(SarRadiometricCalibrationFunction, itk::ImageFunction);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** InputImageType typedef support. */
  typedef TInputImage                              InputImageType;
  typedef typename InputImageType::PixelType       InputPixelType;
  typedef typename Superclass::OutputType          OutputType;
  typedef typename Superclass::IndexType           IndexType;
  typedef typename Superclass::ContinuousIndexType ContinuousIndexType;
  typedef typename Superclass::PointType           PointType;

  itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);


  /** Datatype used for the evaluation */
  typedef typename itk::NumericTraits<InputPixelType>::RealType                       RealType;
  typedef otb::Functor::SarRadiometricCalibrationFunctor<InputPixelType,RealType>     FunctorType;
  typedef typename FunctorType::RealType											  FunctorRealType;

  typedef otb::SarParametricMapFunction<InputImageType>               ParametricFunctionType;
  typedef typename ParametricFunctionType::Pointer                    ParametricFunctionPointer;  

  /** Evalulate the function at specified index */
  virtual RealType EvaluateAtIndex(const IndexType& index) const;

  /** Evaluate the function at non-integer positions */
  virtual RealType Evaluate(const PointType& point) const
  {
    IndexType index;
    this->ConvertPointToNearestIndex(point, index);
    return this->EvaluateAtIndex(index);
  }
  virtual RealType EvaluateAtContinuousIndex(
    const ContinuousIndexType& cindex) const
  {
    IndexType index;
    this->ConvertContinuousIndexToNearestIndex(cindex, index);
    return this->EvaluateAtIndex(index);
  }

  /** Set the input image.
   * \warning this method caches BufferedRegion information.
   * If the BufferedRegion has changed, user must call
   * SetInputImage again to update cached values. */
  virtual void SetInputImage( const InputImageType * ptr );


  /** Get/Set the Offset value */
  itkSetMacro(Offset, FunctorRealType);
  itkGetConstReferenceMacro(Offset, FunctorRealType);

  /** Get/Set the Scale value */
  itkSetMacro(Scale, FunctorRealType);
  itkGetConstReferenceMacro(Scale, FunctorRealType);

  /** Get/Set the AntennaPatternNewGain value */
  itkSetMacro(AntennaPatternNewGain, ParametricFunctionPointer);
  itkGetConstReferenceMacro(AntennaPatternNewGain, ParametricFunctionPointer);

  /** Get/Set the AntennaPatternOldGain value */
  itkSetMacro(AntennaPatternOldGain, ParametricFunctionPointer);
  itkGetConstReferenceMacro(AntennaPatternOldGain, ParametricFunctionPointer);

  /** Get/Set the IncidenceAngle value */
  itkSetMacro(IncidenceAngle, ParametricFunctionPointer);
  itkGetConstReferenceMacro(IncidenceAngle, ParametricFunctionPointer);

  /** Get/Set the RangeSpreadLoss value */
  itkSetMacro(RangeSpreadLoss, ParametricFunctionPointer);
  itkGetConstReferenceMacro(RangeSpreadLoss, ParametricFunctionPointer);

protected:
  SarRadiometricCalibrationFunction();
  virtual ~SarRadiometricCalibrationFunction(){}
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
  SarRadiometricCalibrationFunction(const Self &);  //purposely not implemented
  void operator =(const Self&);  //purposely not implemented

  FunctorRealType   m_Offset;
  FunctorRealType   m_Scale;
  ParametricFunctionPointer   m_AntennaPatternNewGain;
  ParametricFunctionPointer   m_AntennaPatternOldGain;
  ParametricFunctionPointer   m_IncidenceAngle;
  ParametricFunctionPointer   m_RangeSpreadLoss;
};

} // end namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
# include "otbSarRadiometricCalibrationFunction.txx"
#endif

#endif