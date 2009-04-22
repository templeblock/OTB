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
#ifndef __otbRadiometricNonWaterNonVegetationDetectionImageFilter_h
#define __otbRadiometricNonWaterNonVegetationDetectionImageFilter_h

#include "otbMath.h"
#include "otbImage.h"
#include "otbVegetationIndicesFunctor.h"
#include "otbWaterIndicesFunctor.h"
#include "otbMultiChannelRAndGAndNIRIndexImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageToImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkMaskImageFilter.h"

#include "itkSobelEdgeDetectionImageFilter.h"
#include "otbEdgeDetectorImageFilter.h"
#include "otbEdgeDensityImageFilter.h"
#include "otbBinaryImageDensityFunction.h"
#include "otbVectorImageToIntensityImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

// POUR TEST
#include "otbStreamingImageFileWriter.h"


namespace otb
{

namespace Functor
{


  /** \class RadiometricNonWaterNonVegetationIndexFunctor
   *  \brief This functor computes ndwi and ndvi of an image
   *  \brief in order to find urban areas and generate a grayscale image
   */
template<class TInput,class TOutput=double>
class RadiometricNonWaterNonVegetationIndexFunctor
{
 public:
  typedef Functor::NDVI< double, double, double>            VegetationFunctorType;
  typedef Functor::NDWI2< double, double, double>           WaterFunctorType;
  typedef TOutput                                           ValueType;

  VegetationFunctorType GetVegetationFunctor(){ return m_VegetationFunctor; };
  WaterFunctorType GetWaterFunctor(){ return m_WaterFunctor; };

  void SetRedIndex(int id){ m_VegetationFunctor.SetRedIndex(id); };
  void SetGreenIndex(int id){ m_WaterFunctor.SetGIndex(id); };
  void SetNIRIndex(int id){ m_VegetationFunctor.SetNIRIndex(id);
                            m_WaterFunctor.SetNIRIndex(id); };
  unsigned int GetGreenIndex(){ return m_WaterFunctor.GetGIndex(); };

  RadiometricNonWaterNonVegetationIndexFunctor(){}
  ~RadiometricNonWaterNonVegetationIndexFunctor(){};
  
  inline TOutput operator()(const TInput& pInPix)
    {
      TOutput lVIval,lWIval,lOutPix;
/*
      lVIval = static_cast<ValueType>(m_VegetationFunctor(pInPix));

      lWIval = static_cast<ValueType>(m_WaterFunctor(pInPix));

*/
      lVIval = static_cast<ValueType>(std::max(0.,m_VegetationFunctor(pInPix)));

      lWIval = static_cast<ValueType>(std::max(0.,m_WaterFunctor(pInPix)));

      lOutPix = vcl_sqrt(vcl_abs((1-lVIval)*(1-lWIval))); //sqrt : PAS MAL DU TOUT 

// OU
      //lOutPix = vcl_sqrt((1-lVIval)*(1-lVIval)+(1-lWIval)*(1-lWIval)); // eucl : bof bof
// OU
      //lOutPix = 1-vcl_sqrt((lVIval)*(lVIval)+(lWIval)*(lWIval)); // eucl2 : pas mal, un peu effet de flou par rappart a sqrt


      //lOutPix = vcl_sqrt(vcl_abs(1-lVIval)*vcl_abs(1-lWIval)); //sqrt2 : pareil que sqrt 
      //lOutPix = 1-vcl_sqrt(vcl_abs(lVIval)*vcl_abs(lWIval)); //sqrt2 : NUUUUUUL
      //lOutPix = 1. - (vcl_abs(lVIval)+vcl_abs(lWIval)/2.);//Div : PAS MAL
      //lOutPix = 1 - std::max(lVIval,lWIval); //MAX : PAS MAL DU TOUT

/* Tester avec le log
      if(lVIval*lWIval)
        lOutPix = 1 - 1 / ( lVIval*lWIval + 1 ); //Jordy's formula
      else 
        lOutPix = 0;
*/

      return lOutPix;
    }

 protected :
  VegetationFunctorType m_VegetationFunctor;
  WaterFunctorType      m_WaterFunctor;

};



  /** \class RadiometricNonWaterNonVegetationDetectionFunctor
   *  \brief This functor uses computes RadiometricNonWaterNonVegetationIndexFunctor
   *  \brief and appli a threshold to produce a binary image
   */
template<class TInput,class TOutput>
class RadiometricNonWaterNonVegetationDetectionFunctor
{
public:
  // utilise template double parsk sinon cast un truc entre 0 et 1 en unsigned char ce qui donne toujours 1... donc image constante a 1 a la sortie du filtre!!!!
  typedef RadiometricNonWaterNonVegetationIndexFunctor<TInput /*, TOutput*/> RadiometricNonWaterNonVegetationIndexFunctorType;

  RadiometricNonWaterNonVegetationDetectionFunctor()
  {
    m_LowerThreshold = 0.0;
    m_UpperThreshold = 1.0;
  };

  ~RadiometricNonWaterNonVegetationDetectionFunctor() {};
  inline TOutput operator()(const TInput& inPix)
  {

    // The vegetation and water areas are put to 0.
    double indexesVal = static_cast<double>(m_RadiometricNonWaterNonVegetationIndexFunctor(inPix));

// if(indexesVal>1e6)
// std::cout<<indexesVal<<std::endl;

    if ( (indexesVal > m_LowerThreshold) && (indexesVal <= m_UpperThreshold)  )
    {
      return static_cast<TOutput>(1);
    }
    else
    {
      return static_cast<TOutput>(0);
    }
  }

  void SetLowerThreshold(double threshold){ m_LowerThreshold = threshold; };
  void SetUpperThreshold(double threshold){ m_UpperThreshold = threshold; };
  double GetLowerThreshold(){ return m_LowerThreshold; };
  double GetUpperThreshold(){ return m_UpperThreshold; };

  void SetRedIndex(int id){ m_RadiometricNonWaterNonVegetationIndexFunctor.SetRedIndex(id); };
  void SetGreenIndex(int id){ m_RadiometricNonWaterNonVegetationIndexFunctor.SetGreenIndex(id); };
  void SetNIRIndex(int id){ m_RadiometricNonWaterNonVegetationIndexFunctor.SetNIRIndex(id); };

  unsigned int GetGreenIndex(){ return m_RadiometricNonWaterNonVegetationIndexFunctor.GetGreenIndex(); };


protected:
  RadiometricNonWaterNonVegetationIndexFunctorType m_RadiometricNonWaterNonVegetationIndexFunctor;
  double m_LowerThreshold;
  double m_UpperThreshold;

};

} //end namespace functor



  /** \class UrbanAreaDetectionImageFilter
   *  \brief This filter uses associated functors to produce a binary or grayscale image
   *  \brief where the white areas represents the urban areas.
   */
template <class TInputImage, class TOutputImage,
          class TFunction = Functor::RadiometricNonWaterNonVegetationIndexFunctor< 
                                                                 typename TInputImage::PixelType,
                                                                 typename TOutputImage::PixelType> >
class ITK_EXPORT UrbanAreaDetectionImageFilter :
        public itk::ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef UrbanAreaDetectionImageFilter                         Self;
  typedef itk::ImageToImageFilter< TInputImage,TOutputImage >   Superclass;
  typedef itk::SmartPointer<Self>                               Pointer;
  typedef itk::SmartPointer<const Self>                         ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(  UrbanAreaDetectionImageFilter, itk::ImageToImageFilter);

  /** Images typedefs */
  typedef TInputImage                                           VectorImageType;
  typedef TOutputImage                                          OutputImageType;
  typedef double                                                SingleImagePixelType;
  typedef Image<SingleImagePixelType, 2>                        SingleImageType;
  typedef SingleImageType::SizeType                             SizeType;
  typedef unsigned char                                         BinaryImagePixelType;
  typedef Image<BinaryImagePixelType, 2>                        BinaryImageType;
  typedef typename VectorImageType::PixelType                   VectorImagePixelType;
  typedef typename OutputImageType::PixelType                   OutputPixelType;

  /** Filters typedefs */
  // NonVegetationNonWaterIndexFilter
  typedef Functor::RadiometricNonWaterNonVegetationDetectionFunctor< VectorImagePixelType, BinaryImagePixelType > FunctorType;
  typedef MultiChannelRAndGAndNIRIndexImageFilter < VectorImageType,BinaryImageType, FunctorType >       UrbanAreaExtractionFilterType;
  typedef typename UrbanAreaExtractionFilterType::Pointer                                                UrbanAreaExtrationFilterPointerType;
  // Erode/Dilate Filters
  typedef typename itk::BinaryBallStructuringElement< BinaryImagePixelType, 2  >                         StructuringElementType;
  typedef typename itk::BinaryErodeImageFilter<BinaryImageType,BinaryImageType,StructuringElementType>   ErodeFilterType;
  typedef typename itk::BinaryDilateImageFilter<BinaryImageType,BinaryImageType,StructuringElementType>  DilateFilterType;
  // MaskImageFilter
  typedef typename itk::MaskImageFilter<VectorImageType,BinaryImageType>                                 MaskImageFilterType;
  typedef typename MaskImageFilterType::Pointer                                                          MaskImageFilterPointerType;
  // Intensity
  typedef VectorImageToIntensityImageFilter<VectorImageType, SingleImageType>                            IntensityFilterType;
  typedef typename IntensityFilterType::Pointer                                                          IntensityFilterPointerType;
  // EdgeDensityFilter
  typedef BinaryImageDensityFunction<SingleImageType>                                                    CountFunctionType;
  typedef itk::SobelEdgeDetectionImageFilter<SingleImageType , SingleImageType>                          SobelType;
  typedef EdgeDetectorImageFilter<SingleImageType, SingleImageType, SobelType>                           SobelDetectorType;
  typedef EdgeDensityImageFilter<SingleImageType, SingleImageType, SobelDetectorType, CountFunctionType> EdgeDensityFilterType;
  typedef typename EdgeDensityFilterType::Pointer                                                        EdgeDensityFilterPointerType;
  // Threshold
  typedef itk::BinaryThresholdImageFilter<SingleImageType,BinaryImageType>                               ThresholdFilterType;
  typedef ThresholdFilterType::Pointer                                                                   ThresholdFilterPointerType;



  /** Get/Set indices */
  void SetRedIndex(int id){ m_UrbanAreaExtractionFilter->SetRedIndex( id ); }; /*GetFunctor().*/
  void SetGreenIndex(int id){ m_UrbanAreaExtractionFilter->SetGreenIndex( id ); };
  void SetNIRIndex(int id){ m_UrbanAreaExtractionFilter->SetNIRIndex( id ); };
  unsigned int GetRedIndex(){ return m_UrbanAreaExtractionFilter->GetRedIndex(); };
  unsigned int GetGreenIndex(){ return m_UrbanAreaExtractionFilter->GetGreenIndex(); };
  unsigned int GetNIRIndex(){ return m_UrbanAreaExtractionFilter->GetNIRIndex(); };
  /** Get/Set threshold value */
  itkGetMacro(ThresholdValue, double);
  void SetThreshold(double pThreshold)
  {
    m_ThresholdValue = pThreshold;
    m_UrbanAreaExtractionFilter->GetFunctor().SetLowerThreshold( m_ThresholdValue );
  }

  /** Methods */
  virtual void GenerateData();

protected:
  UrbanAreaDetectionImageFilter();
  virtual ~UrbanAreaDetectionImageFilter(){};

private:
  UrbanAreaDetectionImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Filters */
  UrbanAreaExtrationFilterPointerType m_UrbanAreaExtractionFilter;
  MaskImageFilterPointerType          m_MaskImageFilter;
  ErodeFilterType::Pointer            m_ErodeFilter;
  DilateFilterType::Pointer           m_DilateFilter;
  ErodeFilterType::Pointer            m_ErodeFilter2;
  DilateFilterType::Pointer           m_DilateFilter2;


  /** MaxThreshold */
  double m_ThresholdValue;
  double m_ThresholdValue2;

}; // end class RadiometricNonWaterNonVegetationDetectionImageFilter

} // end namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbUrbanAreaDetectionImageFilter.txx"
#endif

#endif
