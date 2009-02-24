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
#ifndef __otbImageViewerModel_h
#define __otbImageViewerModel_h

#include "otbMVCModel.h"
#include "otbLayer.h"
#include "otbObjectList.h"
#include "otbImageViewerModelListener.h"
#include "otbBlendingImageFilter.h"

namespace otb
{
/** \class ImageViewerModel
*   \brief This class is the model for ImageViewer.
*   It is in charge of rendering to the screen a set of Layer.
*   Each visible layer is rendered separately, and the resulting
*   rendered layers are rasterized using the blending function
*   associated to each layer.
* 
* \sa Layer
* \sa BlendingFunction
*
*/

template <class TOutputImage = otb::Image<itk::RGBPixel<unsigned char>,2 >  > 
class ImageViewerModel
  : public MVCModel<ImageViewerModelListener>
{
public:
  /** Standard class typedefs */
  typedef ImageViewerModel                     Self;
  typedef MVCModel<ImageViewerModelListener>   Superclass;
  typedef itk::SmartPointer<Self>              Pointer;
  typedef itk::SmartPointer<const Self>        ConstPointer;
    
  /** Runtime information */
  itkTypeMacro(ImageViewerModel,MVCModel);

  /** New macro */
  itkNewMacro(Self);

  /** Output image typedef */
  typedef TOutputImage                         OutputImageType;
  typedef typename OutputImageType::Pointer    OutputImagePointerType;
  
  /** Layer typedef */
  typedef otb::Layer<OutputImageType>          LayerType;
  typedef typename LayerType::RegionType       RegionType;
  
  /** Layer list typedef */
  typedef otb::ObjectList<LayerType>           LayerListType;
  typedef typename LayerListType::Pointer      LayerListPointerType;
  typedef typename LayerListType::Iterator     LayerIteratorType;

  /** Listener typedef */
  typedef ImageViewerModelListener             ListenerType;

  /** Blending filter typedef */
  typedef otb::BlendingImageFilter<OutputImageType> BlendingFilterType;

  /** Add a new layer
   *  \param layer The layer to add.
   *  \return The location of the added layer.
   */
  virtual unsigned int AddLayer(LayerType * layer);

  /** Get the layer at the current index
   *  \param index The index of the layer to get.
   *  \return a pointer to the layer or NULL if no layer was found at
   * this location.
   */
  virtual LayerType *  GetLayer(unsigned int index);

  /** Remove the layer at the current index
   *  \param index The index of the layer to remove.
   *  \return true if a layer was actually deleted, false otherwise.
   * this location.
   */
  virtual bool         DeleteLayer(unsigned int index);

  /** Returns the first layer whose name matches the given name.
   *  \param name The name of the layer.
   *  \return a pointer to the layer or NULL if no layer was found.
   */
  virtual LayerType * GetLayerByName(std::string name);

  /** Delete the first layer whose name matches the given name.
   *  \param name The name of the layer.
   *  \return true if a layer was actually deleted, false otherwise.
   * this location.
   */
  virtual bool         DeleteLayerByName(std::string name);

  /** Clear all layers */
  virtual void         ClearLayers(void);

  /** \return The number of layers */
  virtual unsigned int GetNumberOfLayers(void);

  /** Get/Set the viewer name */
  itkGetStringMacro(Name);
  itkSetStringMacro(Name);
  
  /** Get the rasterized views */
  itkGetObjectMacro(RasterizedQuicklook,OutputImageType);
  itkGetObjectMacro(RasterizedExtract,OutputImageType);
  itkGetObjectMacro(RasterizedScaledExtract,OutputImageType);

  /** Set/Get the Extract Region */
  itkSetMacro(ExtractRegion,RegionType);
  itkGetConstReferenceMacro(ExtractRegion,RegionType);
  
  /** Set/Get the Scaled Extract Region */
  itkSetMacro(ScaledExtractRegion,RegionType);
  itkGetConstReferenceMacro(ScaledExtractRegion,RegionType);

  /** Update will render all visible layers, rasterize all visible
   * layers and notify all listeners. */
  virtual void Update(void);

protected:
  /** Constructor */
  ImageViewerModel();
  /** Destructor */
  ~ImageViewerModel();

  /** Printself method */
  void                PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Renders all visible layers */
  virtual void         RenderVisibleLayers(void);

  /** Rasterize visible layers */
  virtual void         RasterizeVisibleLayers(void);

  /** Notify a registered listener */
  virtual void         Notify(ListenerType * listener);

private:
  ImageViewerModel(const Self&);     // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  /** Viewer name */
  std::string m_Name;
  
  /** Layer list */
  LayerListPointerType  m_Layers;

  /** Rasterized quicklook */
  OutputImagePointerType m_RasterizedQuicklook;
  bool                   m_HasQuicklook;

  /** Rendered extract */
  OutputImagePointerType m_RasterizedExtract;
  bool                   m_HasExtract;
  RegionType             m_ExtractRegion;

  /** Rendered scaled extract */
  OutputImagePointerType m_RasterizedScaledExtract;
  bool                   m_HasScaledExtract;
  RegionType             m_ScaledExtractRegion;
}; // end class 
} // end namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbImageViewerModel.txx"
#endif

#endif

