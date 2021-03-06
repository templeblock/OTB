/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNarrowBandCurvesLevelSetImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2009-01-26 21:45:53 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNarrowBandCurvesLevelSetImageFilter_h
#define __itkNarrowBandCurvesLevelSetImageFilter_h

#include "itkNarrowBandLevelSetImageFilter.h"
#include "itkCurvesLevelSetFunction.h"

namespace itk {

/** \class NarrowBandCurvesLevelSetImageFilter
 *  \brief Segments structures in images based on user supplied edge potential map.
 *
 *   \par IMPORTANT
 *   The NarrowBandLevelSetImageFilter class and the
 *   CurvesLevelSetFunction class contain additional information necessary
 *   to the full understanding of how to use this filter.
 *
 *    \par OVERVIEW
 *    This class is a level set method segmentation filter. An initial contour
 *    is propagated outwards (or inwards) until it sticks to the shape boundaries.
 *    This is done by using a level set speed function based on a user supplied
 *    edge potential map.
 *
 *    \par INPUTS
 *    This filter requires two inputs.  The first input is a initial level set.
 *    The initial level set is a real image which contains the initial contour/surface
 *    as the zero level set. For example, a signed distance function from the initial
 *    contour/surface is typically used. Unlike the simpler ShapeDetectionLevelSetImageFilter
 *    the initial contour does not have to lie wholly within the shape to be segmented.
 *    The intiial contour is allow to overlap the shape boundary. The extra advection term
 *    in the update equation behaves like a doublet and attracts the contour to the boundary.
 *    This approach for segmentation follows that of Lorigo et al (2001).
 *
 *    \par
 *    The second input is the feature image.  For this filter, this is the edge
 *    potential map. General characteristics of an edge potential map is that
 *    it has values close to zero in regions near the edges and values close
 *    to one inside the shape itself. Typically, the edge potential map is compute
 *    from the image gradient, for example:
 *
 *    \f[ g(I) = 1 / ( 1 + | (\nabla * G)(I)| ) \f]
 *    \f[ g(I) = \exp^{-|(\nabla * G)(I)|} \f]
 * 
 *    where \f$ I \f$ is image intensity and
 *    \f$ (\nabla * G) \f$ is the derivative of Gaussian operator. 
 *
 *    \par
 *    See NarrowBandLevelSetImageFilter and NarrowBandImageFilterBase 
 *    for more information on Inputs.
 *
 *    \par PARAMETERS
 *    The method SetUseNegatiiveFeatures() can be used to switch from propagating inwards (false)
 *    versus propagting outwards (true). 
 *
 *    This implementation allows the user to set the weights between the propagation, advection
 *    and curvature term using methods SetPropagationScaling(), SetAdvectionScaling(),
 *    SetCurvatureScaling(). In general, the larger the CurvatureScaling, the smoother the
 *    resulting contour. To follow the implementation in Caselles's paper,
 *    set the PropagationScaling to \f$ c \f$ (the inflation or ballon force) and
 *    AdvectionScaling and CurvatureScaling both to 1.0.
 *
 *    \par OUTPUTS
 *    The filter outputs a single, scalar, real-valued image.
 *    Negative values in the output image are inside the segmentated region
 *    and positive values in the image are outside of the inside region.  The
 *    zero crossings of the image correspond to the position of the level set
 *    front.
 *
 *    \par REFERENCES
 *    L. Lorigo, O. Faugeras, W.E.L. Grimson, R. Keriven, R. Kikinis, A. Nabavi,
 *    and C.-F. Westin, Curves: Curve evolution for vessel segmentation.
 *    Medical Image Analysis, 5:195-206, 2001.
 *
 *   \par
 *   See NarrowBandImageFilterBase and
 *   NarrowBandLevelSetImageFilter for more information.
 *
 *   \sa NarrowBandLevelSetImageFilter
 *   \sa CurvesLevelSetFunction 
 *
 *   \ingroup LevelSetSegmentation
 */
template <class TInputImage,
          class TFeatureImage,
          class TOutputPixelType = float >
class ITK_EXPORT NarrowBandCurvesLevelSetImageFilter
  : public NarrowBandLevelSetImageFilter<TInputImage, TFeatureImage, TOutputPixelType, Image<TOutputPixelType, ::itk::GetImageDimension<TInputImage>::ImageDimension> >
{
public:
   /** Standard class typedefs */
  typedef NarrowBandCurvesLevelSetImageFilter Self;
  typedef  NarrowBandLevelSetImageFilter<TInputImage, TFeatureImage, TOutputPixelType, Image<TOutputPixelType, ::itk::GetImageDimension<TInputImage>::ImageDimension> >
                                              Superclass;
  typedef SmartPointer<Self>                  Pointer;
  typedef SmartPointer<const Self>            ConstPointer;

  /** Inherited typedef from the superclass. */
  typedef typename Superclass::ValueType        ValueType;
  typedef typename Superclass::OutputImageType  OutputImageType;
  typedef typename Superclass::FeatureImageType FeatureImageType;
  
  /** Type of the segmentation function */
  typedef CurvesLevelSetFunction<OutputImageType,
                                                FeatureImageType> CurvesFunctionType;
  typedef typename CurvesFunctionType::Pointer
    CurvesFunctionPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(NarrowBandCurvesLevelSetImageFilter, NarrowBandLevelSetImageFilter);

  /** Method for creation through the object factory */
  itkNewMacro(Self);
     
  /** Set the value of sigma used to compute derivatives */
  void SetDerivativeSigma( float value )
    {
    m_CurvesFunction->SetDerivativeSigma( value ); 
    this->Modified();
    }
  float GetDerivativeSigma() const
    { return m_CurvesFunction->GetDerivativeSigma(); }

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(OutputHasNumericTraitsCheck,
    (Concept::HasNumericTraits<TOutputPixelType>));
  /** End concept checking */
#endif

protected:
  ~NarrowBandCurvesLevelSetImageFilter() {}
  NarrowBandCurvesLevelSetImageFilter();

  virtual void PrintSelf(std::ostream &os, Indent indent) const; 

  NarrowBandCurvesLevelSetImageFilter(const Self &); // purposely not implemented
   void operator=(const Self&); //purposely not implemented
   
  /** Overridden from Superclass to handle the case when Propagation
   *  Scaling is zero.*/
  void GenerateData();
     
private:
  CurvesFunctionPointer m_CurvesFunction;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNarrowBandCurvesLevelSetImageFilter.txx"
#endif

#endif
