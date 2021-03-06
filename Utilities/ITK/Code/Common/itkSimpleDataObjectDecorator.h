/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSimpleDataObjectDecorator.h,v $
  Language:  C++
  Date:      $Date: 2009-04-21 14:20:10 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSimpleDataObjectDecorator_h
#define __itkSimpleDataObjectDecorator_h

#include "itkDataObject.h"
#include "itkObjectFactory.h"

namespace itk
{


/** \class SimpleDataObjectDecorator
 * \brief Decorates any "simple" data type (data types without smart pointers) with a DataObject API
 *
 * SimpleDataObjectDecorator decorates an object with a DataObject
 * API. This allows simple objects to be encapsulated into objects
 * that can be passed as down the pipeline. This decorator is intended
 * to be used on native types (float, int, etc.) or any objects not
 * derived from itkObject.  To decorate a subclass of itkObject, see
 * DataObjectDecorator. 
 *
 * The decorator provides two methods Set() and Get() to access the
 * decorated object (referred internally as the component).
 *
 * Note that when an instance of SimpleDataObjectDecorator is created,
 * the component is initialized with its default constructor.
 *
 * SimpleDataObjectDecorator can decorate any simple data type. Two
 * other decorators are provided for decorating pointers.
 * DataObjectDecorator will decorate pointers to subclasses of
 * itkObject (internally storing the pointer in a
 * SmartPointer). AutoPointerDataObjectDecorator will decorate any
 * other pointer and manage the memory deallocationg of the component.
 *
 * \sa DataObjectDecorator
 * \sa AutoPointerDataObjectDecorator
 * \ingroup ITKSystemObjects
 *
 */
template<class T>
class ITK_EXPORT SimpleDataObjectDecorator : public DataObject
{
public:
  /** Standard typedefs. */
  typedef SimpleDataObjectDecorator           Self;
  typedef DataObject                          Superclass;
  typedef SmartPointer<Self>                  Pointer;
  typedef SmartPointer<const Self>            ConstPointer;

  /** Typedef for the component type (object being decorated) */
  typedef T ComponentType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SimpleDataObjectDecorator, DataObject);

  /** Set the contained object */
  virtual void Set(const T& val);
  
  /** Get the contained object */
  virtual T& Get() { return m_Component; }
  virtual const T& Get() const { return m_Component; }

  
protected:
  SimpleDataObjectDecorator();
  ~SimpleDataObjectDecorator();
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

protected:

private:
  SimpleDataObjectDecorator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ComponentType m_Component;
  bool          m_Initialized;
};

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_SimpleDataObjectDecorator(_, EXPORT, x, y) namespace itk { \
  _(1(class EXPORT SimpleDataObjectDecorator< ITK_TEMPLATE_1 x >)) \
  namespace Templates { typedef SimpleDataObjectDecorator< ITK_TEMPLATE_1 x > \
                                         SimpleDataObjectDecorator##y; } \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkSimpleDataObjectDecorator+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkSimpleDataObjectDecorator.txx"
#endif

#endif
