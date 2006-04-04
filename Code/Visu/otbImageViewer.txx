#ifndef _otbImageViewer_txx
#define _otbImageViewer_txx

#include "otbImageViewer.h"

#define MAX(_A,_B) ((_A) > (_B) ? (_A) : (_B))
#define MIN(_A,_B) ((_A) < (_B) ? (_A) : (_B))



namespace otb
{
  
template <class TPixel, class TOverlayPixel>
ImageViewer<TPixel,TOverlayPixel>
::ImageViewer()
{
        this->ClearSelectChannels();
        m_ShrinkFactors = 0;
        m_PrincipalImage = NULL;
        m_ScrollImage = NULL;

//        m_ScrollWinSizeMax = 2000;
//        m_ScrollWinSizeMax = 100;
        m_ScrollImageView = false;

        m_InitScrollWinSizeMax = 300;

        m_InitPrincipalWinSizeMax = 300;
        m_ExtractROIFilter = ExtractROIFilterType::New();
        m_ShrinkImageFilter = ShrinkImageFilterType::New();
        
        CreateGUI();
}

  
template <class TPixel, class TOverlayPixel>
ImageViewer<TPixel,TOverlayPixel>
::~ImageViewer()
{

}
template <class TPixel, class TOverlayPixel>
void
ImageViewer<TPixel,TOverlayPixel>
::ZoomAction(void) 
{

//	m_PrincipalView->DrawRectangle( m_ZoomView->GetViewImageRegion() );
}

template <class TPixel, class TOverlayPixel>
void
ImageViewer<TPixel,TOverlayPixel>
::ScrollAction(const IndexType & index) 
{
	//Synchronise l'image principal � partir de la zone rectangle de la fenetre scroll
        IndexType lIndex;
        lIndex[0] = index[0] * m_ShrinkFactors;
        lIndex[1] = index[1] * m_ShrinkFactors;
//std::cout << "lIndex image : "<<lIndex<<std::endl;
//std::cout << "m_InputImage->GetRequestedRegion(): "<<m_InputImage->GetRequestedRegion()<<std::endl;
//std::cout << "m_InputImage->GetLargestPossibleRegion(): "<<m_InputImage->GetLargestPossibleRegion()<<std::endl;
//	RegionType lNewRegion = m_PrincipalView->GetViewRegion( m_InputImage->GetRequestedRegion(), lIndex );
	RegionType lNewRegion = m_PrincipalView->GetViewRegion( m_InputImage->GetLargestPossibleRegion(), lIndex );
//std::cout << "Nouvelle region active sur Scroll : "<<lNewRegion<<std::endl;
//std::cout << "m_InputImage->GetRequestedRegion(): "<<m_InputImage->GetRequestedRegion()<<std::endl;
//std::cout << "Avant Extract"<<std::endl;
// THOMAS
        ExtractImagePrincipal( lNewRegion );
        m_PrincipalImage = m_ExtractROIFilter->GetOutput();

//std::cout << "Nouvelle region requested sur Principal : "<<m_PrincipalImage->GetRequestedRegion()<<std::endl;
        
        m_PrincipalView->SetInput( m_PrincipalImage );
//std::cout << "m_InputImage->GetRequestedRegion(): "<<m_InputImage->GetRequestedRegion()<<std::endl;
//std::cout << "m_InputImage->GetLargestPossibleRegion(): "<<m_InputImage->GetLargestPossibleRegion()<<std::endl;


        //Reconstruit la fenetre principal
        m_PrincipalView->Show();
//        m_PrincipalView->BuildWithImageRegion();
        
        //Maj de la fenetre Zoom
	m_ZoomView->SetInput( m_PrincipalImage );
        IndexType lNewCenter = m_PrincipalView->GetCenterRegion( m_PrincipalView->GetViewImageRegion() );
        this->PrincipalAction( lNewCenter );

//std::cout << "m_InputImage->GetRequestedRegion(): "<<m_InputImage->GetRequestedRegion()<<std::endl;

//	m_PrincipalView->SetCenterPointImage( lNewCenter );
        //Maj de la region / au centre
//        m_PrincipalView->MajViewRegion();
//	m_PrincipalView->update();
        

//  	m_ZoomView->Show();

//std::cout << "m_InputImage->GetRequestedRegion(): "<<m_InputImage->GetRequestedRegion()<<std::endl;


        //Dessine le rectangle sur l'image Scroll
        m_ScrollView->SetDrawViewRectangle( ImageViewType::ShrinkRegion(m_PrincipalView->GetViewImageRegion(),(float)1./(float)m_ShrinkFactors ) );

//	m_ScrollView->update();
	m_ScrollView->Show();

//	m_PrincipalView->DrawRectangle( m_ScrollView->GetViewImageRegion() );
//	m_PrincipalView->DrawRectangle( m_ZoomView->GetViewImageRegion() );
//	m_ScrollView->PrintInfos();
//	m_PrincipalView->PrintInfos();
//	m_ZoomView->PrintInfos();

//std::cout << "FIN scroll Action : m_InputImage->GetRequestedRegion(): "<<m_InputImage->GetRequestedRegion()<<std::endl;

}
template <class TPixel, class TOverlayPixel>
void
ImageViewer<TPixel,TOverlayPixel>
::PrincipalAction(const IndexType & index) 
{
//std::cout << "   ---------------   PrincipalAction  ------------------------------- "<< std::endl;
	//Si une action est faite sur la fenetre principale (ex : agrandir al fenetre, dans le cas ou il la fenetre scroll existe) 
//	m_ZoomView->winCenter( x, y );
	m_ZoomView->SetCenterPointImage( index );
        m_ZoomView->MajViewRegion();
	m_ZoomView->update();
        m_PrincipalView->SetDrawViewRectangle( m_ZoomView->GetViewImageRegion() );
	m_PrincipalView->update();

//	m_ScrollView->DrawRectangle( m_PrincipalView->GetViewImageRegion() );
}


template <class TPixel, class TOverlayPixel>
void
ImageViewer<TPixel,TOverlayPixel>
::ExtractImagePrincipal(const RegionType & zone) 
{
        m_ExtractROIFilter->SetSizeX( zone.GetSize()[0] );
        m_ExtractROIFilter->SetSizeY( zone.GetSize()[1] );
        m_ExtractROIFilter->SetStartX( zone.GetIndex()[0] );
        m_ExtractROIFilter->SetStartY( zone.GetIndex()[1] );
//      m_ExtractROIFilter->SetInput( m_InputImage );
        m_ExtractROIFilter->UpdateLargestPossibleRegion();
        m_ExtractROIFilter->Update();
//        m_PrincipalImage = m_ExtractROIFilter->GetOutput();
}
template <class TPixel, class TOverlayPixel>
bool
ImageViewer<TPixel,TOverlayPixel>
::GenereImageScroll(unsigned int & pShrinkFactors) 
{
        bool lSousEchWindow(false);
        double lShrinkFactors(0.);
        pShrinkFactors = 0;
        //Si pas besoin de Scroll, alors on dit que l'image Principal n'est pas "resizable" !!!
        SizeType lInputImageSize = m_InputImage->GetLargestPossibleRegion().GetSize();
//        if ( MIN(lInputImageSize[0], lInputImageSize[1]) > m_ScrollWinSizeMax )
        // Si la dimension Min de l'image est plus grande que la fenetre Principal, alors on cr��e la fenetre Scroll
        if ( MIN(lInputImageSize[0], lInputImageSize[1]) > MAX( m_PrincipalView->sizeX(), m_PrincipalView->sizeY() ) ) 
        {
                //La taille
                lShrinkFactors = (double)(MAX(lInputImageSize[0], lInputImageSize[1])) / (double)(MAX( m_ScrollView->sizeX(), m_ScrollView->sizeY() )) ;
                if( lShrinkFactors < 1. )
                {
                        lSousEchWindow = false;
                }
                else
                {
                        lSousEchWindow = true;
                }
        }
        pShrinkFactors = (unsigned int )lShrinkFactors;
        return (lSousEchWindow);
}


template <class TPixel, class TOverlayPixel>
void
ImageViewer<TPixel,TOverlayPixel>
::PrepareIHM(void) 
{
        //D�tection si l'image est grande
        
        
        m_ScrollImageView = GenereImageScroll(m_ShrinkFactors);
//std::cout << "Image Scroll ? "<< m_ScrollImageView <<std::endl;
m_InputImage->Update();

//std::cout << "m_InputImage->GetRequestedRegion(): "<<m_InputImage->GetRequestedRegion()<<std::endl;
      
        if ( m_ScrollImageView == false )
        {
                m_PrincipalImage = m_InputImage;
        }
        else
        {
        	// Traitement de la fenetre Principal
//                ImageType::Pointer lTempInputImage = ImageType::New();
//                ImagePointer lTempInputImage = ImageType::New();
//                m_PrincipalImage = static_cast<ImageType *>(lTempInputImage);
        	
        	m_ExtractROIFilter->SetSizeX( m_PrincipalView->sizeX() );
        	m_ExtractROIFilter->SetSizeY( m_PrincipalView->sizeY() );
        	m_ExtractROIFilter->SetStartX( (m_InputImage->GetLargestPossibleRegion().GetSize()[0] - m_PrincipalView->sizeX())/2 );
        	m_ExtractROIFilter->SetStartY( (m_InputImage->GetLargestPossibleRegion().GetSize()[1] - m_PrincipalView->sizeY())/2 );
/*        	m_ExtractROIFilter->SetSizeX( 300 );
        	m_ExtractROIFilter->SetSizeY( 300 );
        	m_ExtractROIFilter->SetStartX( 100 );
        	m_ExtractROIFilter->SetStartY( 100 );*/
        	m_ExtractROIFilter->SetInput( m_InputImage );
        	m_ExtractROIFilter->Update();
                m_PrincipalImage = m_ExtractROIFilter->GetOutput();

        	// Traitement de la fenetre Scroll
                m_ShrinkImageFilter->SetShrinkFactors(m_ShrinkFactors);
                m_ShrinkImageFilter->SetInput( m_InputImage );
                m_ShrinkImageFilter->Update();
                m_ScrollImage = m_ShrinkImageFilter->GetOutput();
        }

	this->GenerateChannelsInformations();
	this->SetChannelsInformationsToImagesView();


  	this->Superclass::Update();
  	static bool firstTime = true;

        // Fenetre PRINCIPAL
        //----------------------------------------
        m_PrincipalView->SetInput( m_PrincipalImage );
        m_PrincipalView->SetDoubleWindow( iviewWindowPrincipal );
        m_PrincipalView->SetViewer( this );
//  	m_PrincipalView->BuildWithImageRegion();
//  	m_PrincipalView->FinaliseInitialisation();
//  	iviewWindowPrincipal->show();
//std::cout << "APRES BUILD"<<std::endl;
//        m_PrincipalView->PrintInfos();
  	m_PrincipalView->Show();
//std::cout << "APRES SHOW"<<std::endl;
        m_PrincipalView->PrintInfos();
//  	m_PrincipalView->show();
//std::cout << "APRES 2 ieme SHOW"<<std::endl;
//        m_PrincipalView->PrintInfos();
//	iviewWindowPrincipal->size( 	m_PrincipalView->GetViewImageRegion().GetSize()[0], 
//  					m_PrincipalView->GetViewImageRegion().GetSize()[1]);
//	iviewWindowPrincipal->size( 	m_PrincipalView->GetInput()->GetRequestedRegion().GetSize()[0], 
//  					m_PrincipalView->GetInput()->GetRequestedRegion().GetSize()[1]);
//  	m_PrincipalView->show();
//  	m_PrincipalView->update();

	m_PrincipalView->PrintInfos();

        // Fenetre ZOOM
        //----------------------------------------
//  	iviewWindowZoom->show();
	m_ZoomView->SetInput( m_PrincipalImage );
        m_ZoomView->SetDoubleWindow( iviewWindowZoom );
        m_ZoomView->SetViewer(this);
//  	m_ZoomView->FinaliseInitialisation();
	// Initialisation de la fenetre ZOOM
	//Positionne le Zoom par d�faut (facteur 4)
//       	m_ZoomView->winZoom( 4 );
//       	m_ZoomView->BuildWithWindowRegion(1);
  	m_ZoomView->Show();
//  	m_ZoomView->update();


        m_PrincipalView->SetDrawViewRectangle( m_ZoomView->GetViewImageRegion() );
//std::cout << "LA"<<std::endl;
//        m_PrincipalView->PrintInfos();
//        m_ZoomView->PrintInfos();

        // Fenetre SCROLL
        //----------------------------------------
  	if ( m_ScrollImageView == true )
  	{
		m_ScrollView->SetInput( m_ScrollImage );
/*  		m_ScrollView->BuildWithImageRegion();
	        iviewWindowScroll->size( 	m_ScrollView->GetViewImageRegion().GetSize()[0], 
  					        m_ScrollView->GetViewImageRegion().GetSize()[1]);
  		iviewWindowScroll->show();
  		m_ScrollView->show();
  		m_ScrollView->update();*/
         	m_ScrollView->SetViewer(this);
                m_ScrollView->SetDoubleWindow( iviewWindowScroll );
                m_ScrollView->SetDrawViewRectangle( ImageViewType::ShrinkRegion(m_PrincipalView->GetViewImageRegion(),(float)m_ShrinkFactors ) );
                m_ScrollView->Show();
 	}
        
  	if( firstTime == true)
  	{
    		firstTime = false;
    		Fl::check();
    		m_PrincipalView->redraw();
    		m_ZoomView->redraw();
	  	if ( m_ScrollImageView == true )
  		{
    			m_ScrollView->redraw();
  		}
    		Fl::check();
  	}
//std::cout << "m_InputImage->GetRequestedRegion(): "<<m_InputImage->GetRequestedRegion()<<std::endl;
        
}



template <class TPixel, class TOverlayPixel>
void
ImageViewer<TPixel,TOverlayPixel>
::SetImage(itk::ImageBase<2> * img)
{
  m_InputImage = dynamic_cast<ImageType *>( img );
//  m_PrincipalView->SetInputImage( image );
//  m_PrincipalView->SetInput( image );
//  m_ScrollView->SetInput( image );
  this->Modified();
  Synchronize();
}

template <class TPixel, class TOverlayPixel>
void 
ImageViewer<TPixel,TOverlayPixel>
::Show(void)
{
  this->Update();
}


template <class TPixel, class TOverlayPixel>
void 
ImageViewer<TPixel,TOverlayPixel>
::Hide(void)
{
  iviewWindowPrincipal->hide();
  iviewWindowScroll->hide();
  iviewWindowZoom->hide();
}


template <class TPixel, class TOverlayPixel>
void 
ImageViewer<TPixel,TOverlayPixel>
::Update(void)
{
  this->PrepareIHM();
  
  
}

template <class TPixel, class TOverlayPixel>
void
ImageViewer<TPixel,TOverlayPixel>
::Synchronize(void) 
{
  float iwDiff  = m_PrincipalView->iwMax() - m_PrincipalView->iwMin();
  float b       = (float)((int)log10(iwDiff)-2);
  double iwMin  = ((int)(m_PrincipalView->iwMin()*pow((float)10, (float)-b)))/pow((float)10,(float)-b);
  double iwMax  = ((int)(m_PrincipalView->iwMax()*pow((float)10, (float)-b)))/pow((float)10,(float)-b);
  double iwStep = (iwMax-iwMin)/100.0;

}


template <class TPixel, class TOverlayPixel>
void
ImageViewer<TPixel,TOverlayPixel>
::SetLabel(const char * label)
{
  iviewWindowPrincipal->label( label );
  iviewWindowScroll->label( label );
  iviewWindowZoom->label( label );

/*  std::string lLabel;
  char chaine[450];
  lLabel = std::string(label) + " - Principal Window";
std::cout << lLabel << std::endl;
strcpy(chaine, label);
std::cout << chaine << std::endl;
  iviewWindowPrincipal->label( (const char*)chaine );

  lLabel = std::string(label) + " - Scroll Window";
  iviewWindowScroll->label( lLabel.c_str() );
*/

}

template <class TPixel, class TOverlayPixel>
void
ImageViewer<TPixel,TOverlayPixel>
::CreatePrincipalImageView( Fl_Group * group, Fl_Gl_Window * original )
{

  short int x = original->x();
  short int y = original->y();
  short int w = original->w();
  short int h = original->h();

  const char * label = original->label();


//  this->m_PrincipalView = new GLImageViewType(x,y,w,h,label);
  this->m_PrincipalView = PrincipalImageViewType::New();
  this->m_PrincipalView->Init(x,y,w,h,label);
// THOMAS : comme l'image est invers??e par d??faut, on a remet a l'endroit
//  this->m_PrincipalView->flipY(true);

  this->m_PrincipalView->box( FL_EMBOSSED_BOX );

  group->remove( original );

  delete original;


}

template <class TPixel, class TOverlayPixel>
void
ImageViewer<TPixel,TOverlayPixel>
::CreateScrollImageView( Fl_Group * group, Fl_Gl_Window * original )
{
  short int x = original->x();
  short int y = original->y();
  short int w = original->w();
  short int h = original->h();

  const char * label = original->label();

  this->m_ScrollView = ScrollImageViewType::New();
  this->m_ScrollView->Init(x,y,w,h,label);
  this->m_ScrollView->box( FL_EMBOSSED_BOX );

  group->remove( original );

  delete original;

}
template <class TPixel, class TOverlayPixel>
void
ImageViewer<TPixel,TOverlayPixel>
::CreateZoomImageView( Fl_Group * group, Fl_Gl_Window * original )
{
  short int x = original->x();
  short int y = original->y();
  short int w = original->w();
  short int h = original->h();

  const char * label = original->label();

  this->m_ZoomView = ZoomImageViewType::New();
  this->m_ZoomView->Init(x,y,w,h,label);
  this->m_ZoomView->box( FL_EMBOSSED_BOX );

  group->remove( original );

  delete original;

}


template <class TPixel, class TOverlayPixel>
void 
ImageViewer<TPixel,TOverlayPixel>
::ClickSelectCallBack( void (*newClickSelectArgCallBack)(float, float,
                                                         /*float,*/ float,
                                                         void *),
                                                         void * newClickSelectArg)
{
  m_PrincipalView->clickSelectCallBack( newClickSelectArgCallBack, 
                                    newClickSelectArg           ); 
  m_ScrollView->clickSelectCallBack( newClickSelectArgCallBack, 
                                    newClickSelectArg           ); 
  m_ZoomView->clickSelectCallBack( newClickSelectArgCallBack, 
                                    newClickSelectArg           ); 
}

template <class TPixel, class TOverlayPixel>
void 
ImageViewer<TPixel,TOverlayPixel>
::GenerateChannelsInformations(void)
{

        m_ChannelsWorks.clear();
        
        //Controle que l'utilisateur a bien selectionne de canaux coherent
        if( m_GrayLevelChannel != -1 )
        {
                if( (m_RedChannel != -1 ) || (m_GreenChannel != -1 ) || (m_BlueChannel != -1 ) )
                {
                        itkExceptionMacro(<< "Vous avez selectionne un canal pour le niveau de gris, mais aussi un (ou des) canaux en RGB" );
                }
                
                m_ModeView = ImageViewBaseType::GRAY_LEVEL;
        }
        else
        {
               m_ModeView = ImageViewBaseType::RGB_LEVEL;
               if( (m_RedChannel == -1 ) && (m_GreenChannel == -1 ) && (m_BlueChannel == -1 ) )
               {
                        //itkExceptionMacro(<< "Aucun canal n'a ete selectionne" );
                        // On les choisi automatiquement,si c'est possible
                        int nbCanauxInputImage = m_PrincipalImage->GetNumberOfComponentsPerPixel();
                        
//                        std::cout << "Nb canaux de l'image : "<<nbCanauxInputImage<<std::endl;

                        switch ( nbCanauxInputImage )
                        {
                                case 1 :
                                        m_GrayLevelChannel = 1;
                                        m_ModeView = ImageViewBaseType::GRAY_LEVEL;
                                        break;
                                default :
                                case 3 : 
                                        m_BlueChannel = 3;
                                        m_ModeView = ImageViewBaseType::RGB_LEVEL;
                                case 2 : 
                                        m_GreenChannel = 2;
                                        m_RedChannel = 1;
                                        m_ModeView = ImageViewBaseType::RGB_LEVEL;
                                        break;
                        }
               }
               
        }
        // On selectionne dans "m_ChannelsWorks" que les canaux valide, a lire dans l'image d'entree
        switch( m_ModeView )
        {
                case ImageViewBaseType::GRAY_LEVEL :       m_ChannelsWorks.push_back( m_GrayLevelChannel );
                        break;
                case ImageViewBaseType::RGB_LEVEL :        
                                        if ( m_RedChannel != -1 ) m_ChannelsWorks.push_back( m_RedChannel );
                                        if ( m_GreenChannel != -1 ) m_ChannelsWorks.push_back( m_GreenChannel );
                                        if ( m_BlueChannel != -1 ) m_ChannelsWorks.push_back( m_BlueChannel );
                        break;
        
        }
        //Controle que les canaux selection existe bien dans l'image d'entree
        for ( int nbChannels = 0 ; nbChannels < m_ChannelsWorks.size() ; nbChannels++)
        {
                if ( (m_ChannelsWorks[nbChannels] <= 0) || (m_ChannelsWorks[nbChannels] > m_PrincipalImage->GetNumberOfComponentsPerPixel()) )
                {
                        itkExceptionMacro(<< "L'image d'entree possede "<<m_PrincipalImage->GetNumberOfComponentsPerPixel()<< "\n Vous devez selectionner des canaux dans [1..."<<m_PrincipalImage->GetNumberOfComponentsPerPixel()<<"]." );
                }
        }
        m_NbDim = (int)m_ModeView;
}


template <class TPixel, class TOverlayPixel>
void 
ImageViewer<TPixel,TOverlayPixel>::
SetChannelsInformationsToImagesView(void)
{
	m_PrincipalView->SetChannelsWorks(m_ChannelsWorks);
	m_PrincipalView->SetModeView(m_ModeView);
	m_PrincipalView->SetNbDim(m_NbDim);
        m_PrincipalView->SetGrayLevelChannel(m_GrayLevelChannel);
        m_PrincipalView->SetRGBChannels(m_RedChannel,m_GreenChannel,m_BlueChannel);
        
	m_ZoomView->SetChannelsWorks(m_ChannelsWorks);
	m_ZoomView->SetModeView(m_ModeView);
	m_ZoomView->SetNbDim(m_NbDim);
        m_ZoomView->SetGrayLevelChannel(m_GrayLevelChannel);
        m_ZoomView->SetRGBChannels(m_RedChannel,m_GreenChannel,m_BlueChannel);
  	if ( m_ScrollImageView == true )
  	{
		m_ScrollView->SetChannelsWorks(m_ChannelsWorks);
		m_ScrollView->SetModeView(m_ModeView);
		m_ScrollView->SetNbDim(m_NbDim);
                m_ScrollView->SetGrayLevelChannel(m_GrayLevelChannel);
                m_ScrollView->SetRGBChannels(m_RedChannel,m_GreenChannel,m_BlueChannel);
	}
}
template <class TPixel, class TOverlayPixel>
void 
ImageViewer<TPixel,TOverlayPixel>::
ClearSelectChannels(void)
{ 
        m_RedChannel            = -1;
        m_GreenChannel          = -1;
        m_BlueChannel           = -1;
        m_GrayLevelChannel      = -1;
        m_ChannelsWorks.clear();
// 	this->SetChannelsInformationsToImagesView();
}


} // end namespace otb
#endif

