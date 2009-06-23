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
#ifndef __otbChannelSelectorFunctor_h
#define __otbChannelSelectorFunctor_h
#include <cassert>

#include "itkNumericTraits.h"
#include "itkVariableLengthVector.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"

namespace otb
{
  namespace Function
  {
    /** \class ChannelSelectorFunctor
    * \brief Base class for pixel representation functions.
    *
    *  \ingroup Visualization
    */
    template <class TInputPixel>
    class ChannelSelectorFunctor
    {
      public:

        typedef TInputPixel PixelType;
        typedef typename itk::NumericTraits<PixelType>::ValueType ScalarType;
        typedef itk::VariableLengthVector<ScalarType>       VectorPixelType;
        typedef itk::RGBPixel<ScalarType> RGBPixelType;
        typedef itk::RGBAPixel<ScalarType> RGBAPixelType;

        typedef VectorPixelType OutputPixelType;


        /** Constructor */
        ChannelSelectorFunctor() :
           usingDefaultParameters(true)
        {
          PixelType pix;
          ChannelListInitialization(pix);
        }

        /** Destructor */
        virtual ~ChannelSelectorFunctor() {}

        const char *GetNameOfClass() const
        {return "ChannelSelectorFunctor";}


        OutputPixelType operator()(const VectorPixelType & inPixel) const
        {
//           otbMsgDevMacro(<<"Channel list "<< m_ChannelList[0]);
          OutputPixelType outPixel;
          outPixel.SetSize(m_ChannelList.size());
          for (unsigned int i=0; i<m_ChannelList.size(); ++i)
          {
// REVIEW: Not sure that assert is the best choice here. Exception
// would be better.
// REPLY: ideally, the verification should be done outside and only
// once for the image, not for every pixel, when we reach this point
// the m_ChannelList MUST be valid
            assert(m_ChannelList[i] < inPixel.Size());
            outPixel[i] = inPixel[m_ChannelList[i]];
          }
          return outPixel;
        }

        OutputPixelType operator()(ScalarType inPixel) const
        {
          OutputPixelType outPixel;
          outPixel.SetSize(1);
          for (unsigned int i=0; i<m_ChannelList.size(); ++i)
          {
            assert(m_ChannelList[i] < 1);
            outPixel[0] = inPixel;
          }
          return outPixel;
        }

        OutputPixelType operator()(const RGBPixelType & inPixel) const
        {
          OutputPixelType outPixel;
          outPixel.SetSize(m_ChannelList.size());
          for (unsigned int i=0; i<m_ChannelList.size(); ++i)
          {
            assert(m_ChannelList[i] < 3);
            outPixel[i] = inPixel[m_ChannelList[i]];
          }
          return outPixel;
        }

        OutputPixelType operator()(const RGBAPixelType & inPixel) const
        {
          OutputPixelType outPixel;
          outPixel.SetSize(m_ChannelList.size());
          for (unsigned int i=0; i<m_ChannelList.size(); ++i)
          {
            assert(m_ChannelList[i] < 4);
            outPixel[i] = inPixel[m_ChannelList[i]];
          }
          return outPixel;
        }

        unsigned int GetOutputSize() const
        {
          return m_ChannelList.size();
        }

        std::vector<unsigned int> GetChannelList() const
        {
          return m_ChannelList;
        }
        void SetChannelList(std::vector<unsigned int> channels)
        {
          m_ChannelList = channels;
        }

       void SetChannelIndex(unsigned int channelPosition, unsigned int channel)
       {
          if (m_ChannelList.size() < channelPosition+1)
          {
            m_ChannelList.resize(channelPosition+1,0);
          }
         m_ChannelList[channelPosition] = channel;
         usingDefaultParameters = false;
       }

       unsigned int GetChannelIndex(unsigned int channelPosition) const
       {
         if (channelPosition >= m_ChannelList.size())
         {
           itkExceptionMacro(<<"Can't get channel " << channelPosition << ", there is only " << m_ChannelList.size() << " element in the list");
         }
         return m_ChannelList[channelPosition];

       }

        /** Only for backward compatibility but should not be used*/
        void SetAllChannels(unsigned int channel)
        {
          if (m_ChannelList.size() <3)
          {
            m_ChannelList.resize(3,0);
          }
          m_ChannelList[0]=channel;
          m_ChannelList[1]=channel;
          m_ChannelList[2]=channel;
          usingDefaultParameters = false;
        }
        void SetRedChannelIndex(unsigned int channel)
        {
          if (m_ChannelList.size() <1)
          {
            m_ChannelList.resize(3,0);
          }
          m_ChannelList[0] = channel;
          usingDefaultParameters = false;
        }

        void SetGreenChannelIndex(unsigned int channel)
        {
          if (m_ChannelList.size() <2)
          {
            m_ChannelList.resize(3,0);
          }
          m_ChannelList[1] = channel;
          usingDefaultParameters = false;
        }

        void SetBlueChannelIndex(unsigned int channel)
        {
          if (m_ChannelList.size() <3)
          {
            m_ChannelList.resize(3,0);
          }
          m_ChannelList[2] = channel;
          usingDefaultParameters = false;
        }

        unsigned int GetRedChannelIndex() const
        {
          return m_ChannelList[0];
        }
        unsigned int GetGreenChannelIndex() const
        {
          return m_ChannelList[1];
        }
        unsigned int GetBlueChannelIndex() const
        {
          return m_ChannelList[2];
        }

        bool IsUsingDefaultParameters()
        {
          return usingDefaultParameters;
        }
      private:

        void ChannelListInitialization(ScalarType inPixel)
        {
          m_ChannelList.push_back(0);
        }
        void ChannelListInitialization(VectorPixelType inPixel)
        {
          //at this point, we are not sure to have three bands
          m_ChannelList.push_back(0);
        }
        void ChannelListInitialization(RGBPixelType inPixel)
        {
          m_ChannelList.push_back(0);
          m_ChannelList.push_back(1);
          m_ChannelList.push_back(2);
        }
        void ChannelListInitialization(RGBAPixelType inPixel)
        {
          m_ChannelList.push_back(0);
          m_ChannelList.push_back(1);
          m_ChannelList.push_back(2);
          m_ChannelList.push_back(3);
        }
        std::vector<unsigned int> m_ChannelList;
        bool usingDefaultParameters;

    };


  }
}

#endif