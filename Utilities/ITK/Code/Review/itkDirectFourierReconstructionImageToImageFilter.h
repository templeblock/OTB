/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDirectFourierReconstructionImageToImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2010-06-14 15:01:05 $
  Version:   $Revision: 1.4 $

  Copyright ( c ) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkDirectFourierReconstructionImageToImageFilter_h
#define __itkDirectFourierReconstructionImageToImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"

#include "itkVnlFFTRealToComplexConjugateImageFilter.h"
#include "itkVnlFFTComplexConjugateToRealImageFilter.h"

#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageSliceConstIteratorWithIndex.h"

#include "itkComplexBSplineInterpolateImageFunction.h"

#include <math.h>

namespace itk
{
/** 
 * \class DirectFourierReconstructionImageToImageFilter
 * \brief Direct fourier reconstruction filter of a tomographic volume.
 *
 * The algorithm is detailed in the Insight Journal publication on
 * "Direct Fourier Tomographic Reconstruction Image-to-Image Filter"
 * by D. Zosso, M. Bach Cuadra and J. Thiran, August 2007
 *
 * This implementation was taken from the Insight Journal paper:
 * http://hdl.handle.net/1926/585
 *
 * \ingroup ImageFilters
 */
template< class TInputPixelType = double, class TOutputPixelType = double >
 class ITK_EXPORT DirectFourierReconstructionImageToImageFilter :
  public ImageToImageFilter< Image< TInputPixelType, 3 >, Image< TOutputPixelType, 3 > >
 {
public:
   /** Standard Self typedef */
  typedef DirectFourierReconstructionImageToImageFilter Self;
  /** InputImageType from TInputPixelType */
  typedef Image< TInputPixelType, 3 > InputImageType;
  /** OutputImageType from TOutputPixelType */
  typedef Image< TOutputPixelType, 3 > OutputImageType;
  
  /** Standard Superclass typedef */
  typedef ImageToImageFilter< InputImageType, OutputImageType > Superclass;
  
  /** Standard Pointer typedef */
  typedef SmartPointer< Self > Pointer;
  /** Standard ConstPointer typedef */
  typedef SmartPointer< const Self > ConstPointer;
  
  itkNewMacro( Self );
  itkTypeMacro( DirectFourierReconstructionImageToImageFilter, ImageToImageFilter );
  
  /** Class RegionType */
  typedef typename InputImageType::RegionType RegionType;
  /** Class IndexType */
  typedef typename InputImageType::IndexType IndexType;
  /** Class SizeType */
  typedef typename InputImageType::SizeType SizeType;
  /** Class PointType */
  typedef typename InputImageType::PointType PointType;
  /** Class SpacingType */
  typedef typename InputImageType::SpacingType SpacingType;
  
  /** Standard (const) InputImagePointer */
  typedef typename InputImageType::ConstPointer ConstInputImagePointer;
  /** Special (non-const) InputImagePointer */
  typedef typename InputImageType::Pointer InputImagePointer;
  /** OutputImagePointer */
  typedef typename OutputImageType::Pointer OutputImagePointer;
  
  itkSetMacro( ZeroPadding, unsigned short int );
  itkGetConstMacro( ZeroPadding, unsigned short int );
  itkSetMacro( OverSampling, unsigned short int );
  itkGetConstMacro( OverSampling, unsigned short int );
  itkSetMacro( Cutoff, double );
  itkGetConstMacro( Cutoff, double );
  itkSetMacro( AlphaRange, double );
  itkGetConstMacro( AlphaRange, double );
  itkSetMacro( AlphaDirection, unsigned short int );
  itkGetConstMacro( AlphaDirection, unsigned short int );
  itkSetMacro( ZDirection, unsigned short int );
  itkGetConstMacro( ZDirection, unsigned short int );
  itkSetMacro( RDirection, unsigned short int );
  itkGetConstMacro( RDirection, unsigned short int );
  itkSetMacro( RadialSplineOrder, unsigned short int );
  itkGetConstMacro( RadialSplineOrder, unsigned short int ); 
  
protected:
  /** Constructor */
  DirectFourierReconstructionImageToImageFilter();
  /** Destructor */
  ~DirectFourierReconstructionImageToImageFilter() {};
  
  /** Output class information */
  void PrintSelf( std::ostream &os, Indent indent ) const;
  
  /** Generate metadata for output image */
  void GenerateOutputInformation();
  /** Calculate the required input region */
  void GenerateInputRequestedRegion();
  /** Actual filter computation */
  void GenerateData();
  
private:
  /** Const slice iterator type of the input image */
  typedef ImageSliceConstIteratorWithIndex< InputImageType > InputSliceIteratorType;

  /** 1D FFT filter type */
  typedef VnlFFTRealToComplexConjugateImageFilter< double, 1 > FFTLineFilterType;
  /** Derived 1D FFT image type */
  typedef FFTLineFilterType::OutputImageType FFTLineType;
  /** Derived 1D input image type */
  typedef FFTLineFilterType::InputImageType ProjectionLineType;
  /** 1D FFT line iterator */
  typedef ImageRegionIteratorWithIndex< FFTLineType > FFTLineIteratorType;
  /** 1D FFT line B-Spline interpolator */
  typedef ComplexBSplineInterpolateImageFunction< FFTLineType, double, double > FFTLineInterpolatorType;
  
  /** 2D inverse FFT filter type */
  typedef VnlFFTComplexConjugateToRealImageFilter< double, 2 > IFFTSliceFilterType;
  /** Derived 2D FFT image type */
  typedef IFFTSliceFilterType::InputImageType FFTSliceType;
  /** Derived 2D output slice type */
  typedef IFFTSliceFilterType::OutputImageType OutputSliceType;
  /** 2D FFT slice iterator */
  typedef ImageRegionIteratorWithIndex< FFTSliceType > FFTSliceIteratorType;
  /** 2D output slice iterator */
  typedef ImageRegionIteratorWithIndex< OutputSliceType > OutputSliceIteratorType;
 
 
  unsigned short int m_ZeroPadding;       /**< n-fold zero-padding */
  unsigned short int m_OverSampling;      /**< n-fold oversampling */
  double             m_Cutoff;            /**< Radial lowpass cut-off frequency */
  double             m_AlphaRange;        /**< Covered angular range */
  
  unsigned short int m_ZDirection;        /**< Axial index in the input image  */
  unsigned short int m_AlphaDirection;    /**< Angular index in the input image */
  unsigned short int m_RDirection;        /**< Radial index in the input image  */
  unsigned short int m_RadialSplineOrder; /**< Spline order for the radial BSpline interpolation  */
  
  double m_PI; /**< The constant pi....  */
  
  RegionType m_InputRequestedRegion; /**< The region requested from* the input image   */
 
  DirectFourierReconstructionImageToImageFilter(const Self&); /**< purposely not implemented  */
  void operator=(const Self&); /**< purposely not implemented  */
};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDirectFourierReconstructionImageToImageFilter.txx"
#endif

#endif /* __itkDirectFourierReconstructionImageToImageFilter_h */
