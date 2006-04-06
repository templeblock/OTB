#ifndef _otbVectorImageView_txx
#define _otbVectorImageView_txx

#include <otbVectorImageView.h>

namespace otb
{
  

template <class TPixel>
VectorImageView<TPixel>::VectorImageView(/*int x, int y, int w, int h, const char **/ )
{

  this->SetNumberOfRequiredInputs(1);
  this->SetNumberOfRequiredOutputs(0);
    
}

template <class TPixel>
void
VectorImageView<TPixel>::Init(int x, int y, int w, int h, const char * )
{
  this->SetNumberOfRequiredInputs(1);
  this->SetNumberOfRequiredOutputs(0);
  cX = x;
  cY = y;
  cW = w;
  cH = h;
  
  cValidImData = false;
  cViewImData = false;
  cViewImDataCallBack = NULL;
  cViewImDataArg = NULL;
  cViewImDataArgCallBack = NULL;
  
  cIWMin = 0;
  cIWMax = 0;
  cWinZoom = 1;
  
  cWinSizeX = 0;
  cWinSizeY = 0;
  cWinDataSizeX = 0;
  cWinDataSizeY = 0;
  cWinImData = NULL;
  
}

//
//
//
template <class TPixel>
VectorImageView<TPixel>::~VectorImageView()
  {
  if(cWinImData != NULL)
    delete [] cWinImData;
  cWinImData = NULL;
  }



template <class TPixel>
void
VectorImageView<TPixel>
::SetInput( const ImageType * image )
{
  // Process object is not const-correct so the const_cast is required here
  this->itk::ProcessObject::SetNthInput(0, 
                                   const_cast< ImageType *>( image ) );
}


//
//
template <class TPixel>
const typename VectorImageView<TPixel>::ImageType *
VectorImageView<TPixel>
::GetInput(void) const
  {
  if (this->GetNumberOfInputs() < 1)
    {
    return 0;
    }

  return ( (const_cast< ImageType * >
     (this->itk::ProcessObject::GetInput(0) ) ) );

  }




template <class TPixel>
typename VectorImageView<TPixel>::ImageType * 
VectorImageView<TPixel>
::GetInput(void)
  {
  if (this->GetNumberOfInputs() < 1)
    {
    return 0;
    }

  return ( static_cast< ImageType * >
     (this->itk::ProcessObject::GetInput(0) ) ) ;
  }


//
//
template <class TPixel>
unsigned char* VectorImageView<TPixel>::winImData(void)
  {
  return cWinImData;
  }


//
//

template <class TPixel>
unsigned int VectorImageView<TPixel>::winDataSizeX(void)
  {
  
  return cWinDataSizeX;
  
  }


//
//
template <class TPixel>
unsigned int VectorImageView<TPixel>::winDataSizeY(void)

  {
  
  return cWinDataSizeY;
  
  }

template <class TPixel>
double VectorImageView<TPixel>::
dataMin(void) const
  {
  return this->cDataMin;
  }

template <class TPixel>
double VectorImageView<TPixel>::
dataMax(void) const
  {
  return this->cDataMax;
  }


//
//
//
template <class TPixel>
void VectorImageView<TPixel>::iwMin(float newIWMin)
  {
  cIWMin = newIWMin;
  if(cIWCallBack != NULL)
    cIWCallBack();
  if(cIWArgCallBack != NULL)
    cIWArgCallBack(cIWArg);
  }




template <class TPixel>
float VectorImageView<TPixel>::iwMin(void)
  {
  return cIWMin;
  }




//
//
//
template <class TPixel>
void VectorImageView<TPixel>::iwMax(float newIWMax)
  {
  cIWMax = newIWMax;
  if(cIWCallBack != NULL)
    cIWCallBack();
  if(cIWArgCallBack != NULL)
    cIWArgCallBack(cIWArg);
  }




template <class TPixel>
float VectorImageView<TPixel>::iwMax(void)
  {
  return cIWMax;
  }

//
//
template <class TPixel>
void VectorImageView<TPixel>::iwCallBack(void (* newIWCallBack)(void))
  {
  cIWCallBack = newIWCallBack;
  }




template <class TPixel>
void VectorImageView<TPixel>::iwCallBack(void (* newIWArgCallBack)(void *), 
                                   void * newIWArg)
  {
  cIWArg = newIWArg;
  cIWArgCallBack = newIWArgCallBack;
  }




//
//
//
template <class TPixel>
void VectorImageView<TPixel>::winZoom(float newWinZoom)
  {
//  if(newWinZoom>0.1 && newWinZoom<cDimSize[cWinOrder[0]])
//On interdit le zoom < 1 
  if(newWinZoom>=1 && newWinZoom<cDimSize[0])
    {
    cWinZoom = newWinZoom;
    }
  }




template <class TPixel>
float VectorImageView<TPixel>::winZoom(void)
  {
  return cWinZoom;
  }


//
//
template <class TPixel>
void VectorImageView<TPixel>::size(int w, int h)
  {
  cW = w;
  cH = h;
  }

template <class TPixel>
void VectorImageView<TPixel>::resize(int x, int y, int w, int h)
  {
  cX = x;
  cY = y;
  cW = w;
  cH = h;
  }

}; //namespace


#endif
