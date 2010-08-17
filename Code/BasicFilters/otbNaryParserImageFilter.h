/*=========================================================================

  Program:   ORFEO Toolbox
    Language:  C++
    Date:      $Date$
    Version:   $Revision$


    Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
    See OTBCopyright.txt for details.

    Some parts of this code are derived from ITK. See ITKCopyright.txt
    for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
        PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __otbNaryParserImageFilter_h
#define __otbNaryParserImageFilter_h

#include "itkInPlaceImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkArray.h"

#include "otbParser.h"

namespace otb
{
/** \class NaryParserImageFilter
 * \brief Performs a mathematical operation on the input images
 * according to the formula specified by the user
 *
 * \sa Parser
 *
 */

template< class TImage >
class ITK_EXPORT NaryParserImageFilter 
  : public itk::InPlaceImageFilter< TImage >
{
public:
  /** Standard class typedefs. */
  typedef NaryParserImageFilter< TImage >                                               Self;
  typedef itk::InPlaceImageFilter< TImage >                                             Superclass;
  typedef itk::SmartPointer< Self >                                                     Pointer;
  typedef itk::SmartPointer< const Self >                                               ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(NaryParserImageFilter, InPlaceImageFilter);

  /** Some convenient typedefs. */
  typedef TImage                                  ImageType;
  typedef typename ImageType::ConstPointer        ImagePointer;
  typedef typename ImageType::RegionType          ImageRegionType; 
  typedef typename ImageType::PixelType           PixelType;
  typedef Parser                                  ParserType;
  
  /** Set the nth filter input with or without a specified associated variable name */
  void SetNthInput( unsigned int idx, const ImageType * image);
  void SetNthInput( unsigned int idx, const ImageType * image, const std::string& varName);

  /** Change the nth filter input associated variable name */
  void SetNthInputName(unsigned int idx, const std::string& expression);

  /** Set the expression to be parsed */
  void SetExpression(const std::string& expression);

  /** Return the expression to be parsed */
  std::string GetExpression() const;
  
  /** Return the nth filter input associated variable name */
  std::string GetNthInputName(unsigned int idx) const;

  /** Return a pointer on the nth filter input */
  ImageType * GetNthInput(unsigned int idx);

protected :
  NaryParserImageFilter();
  virtual ~NaryParserImageFilter();
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;
 
  void BeforeThreadedGenerateData();
  void ThreadedGenerateData(const ImageRegionType& outputRegionForThread, int threadId );
  void AfterThreadedGenerateData();

private :
  NaryParserImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::string                           m_Expression; 
  std::vector<ParserType::Pointer>      m_VParser;
  std::vector< std::vector<double> >    m_AImage;
  std::vector< std::string >            m_VVarName;

  long                                  m_UnderflowCount;
  long                                  m_OverflowCount;
  itk::Array<long>                      m_ThreadUnderflow;
  itk::Array<long>                      m_ThreadOverflow;
};

}//end namespace otb

#ifndef ITK_MANUAL_INSTANTIATION
#include "otbNaryParserImageFilter.txx"
#endif

#endif