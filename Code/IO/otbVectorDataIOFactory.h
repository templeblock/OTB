/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: otbVectorDataIOFactory.h,v $
  Language:  C++
  Date:      $Date: 2007/03/22 14:28:51 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbVectorDataIOFactory_h
#define __otbVectorDataIOFactory_h

#include "itkObject.h"
#include "otbVectorDataIOBase.h"

namespace otb
{
/** \class VectorDataIOFactory
 * \brief Create instances of VectorDataIO objects using an object factory.
 */
class ITK_EXPORT VectorDataIOFactory : public itk::Object
{
public:  
  /** Standard class typedefs. */
  typedef VectorDataIOFactory           Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  
  /** Class Methods used to interface with the registered factories */
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(VectorDataIOFactory, Object);

  /** Convenient typedefs. */
  typedef ::otb::VectorDataIOBase::Pointer VectorDataIOBasePointer;

  /** Mode in which the files is intended to be used */
  typedef enum { ReadMode, WriteMode } FileModeType;
  
  /** Create the appropriate VectorDataIO depending on the particulars of the file. */
  static VectorDataIOBasePointer CreateVectorDataIO(const char* path, FileModeType mode);

  /** Register Built-in factories */
  static void RegisterBuiltInFactories();

protected:
  VectorDataIOFactory();
  ~VectorDataIOFactory();

private:
  VectorDataIOFactory(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};
  
  
} // end namespace otb

#endif
