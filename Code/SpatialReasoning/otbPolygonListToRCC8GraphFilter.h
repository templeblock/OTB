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
#ifndef __otbPolygonListToRCC8GraphFilter_h
#define __otbPolygonListToRCC8GraphFilter_h

#include "otbImageListToRCC8GraphFilter.h"

namespace otb
{
/**
 * \class PolygonListToRCC8GraphFilter
 * \brief 
 * \todo document this class
 */
template <class TPolygonList, class TOutputGraph>
class ITK_EXPORT PolygonListToRCC8GraphFilter
  : public RCC8GraphSource<TOutputGraph>
{
public:
  /** Standard class typedefs. */
  typedef PolygonListToRCC8GraphFilter  Self;
  typedef RCC8GraphSource<TOutputGraph> Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  /** Run-time type information (and related methods). */
  itkTypeMacro(PolygonListToRCC8GraphFilter,RCC8GraphSource);
 
  /** Input related typedef */
  typedef TPolygonList                           PolygonListType;
  typedef typename PolygonListType::ConstPointer PolygonListConstPointerType;
  typedef typename PolygonListType::ObjectType   PolygonType;
  typedef typename PolygonType::Pointer          PolygonPointerType;

  /** Output related typedefs */
  typedef TOutputGraph OutputGraphType;
  typedef typename OutputGraphType::Pointer OutputGraphPointerType;
  typedef typename OutputGraphType::VertexType VertexType;
  typedef typename VertexType::Pointer VertexPointerType;
  typedef typename VertexType::PathType PathType;
  typedef typename OutputGraphType::VertexDescriptorType VertexDescriptorType;
  /** Knowledge enum typedef */
  typedef typename OutputGraphType::RCC8ValueType RCC8ValueType;
  typedef enum {NO_INFO,LEVEL_1,LEVEL_3,FULL} KnowledgeValueType;
  typedef std::pair<KnowledgeValueType,RCC8ValueType> KnowledgeStateType;
  /** Toogle optimisation flag */
  itkBooleanMacro(Optimisation);
  itkSetMacro(Optimisation,bool);

  /**
   * Get the number of occurences of the given value
   * \return The value.
   */
  unsigned int GetRelationsCount(RCC8ValueType val);

  /**
   * Get number of relations
   * \return The number of relations.
   */
  unsigned int GetNumberOfRelations(void);

  /** Set the input of the filter */
  virtual void SetInput( const PolygonListType *input);
  /** Get the input of the filter */
  const PolygonListType * GetInput(void);

protected:
  /** Constructor */
  PolygonListToRCC8GraphFilter();
  /** Destructor */
  ~PolygonListToRCC8GraphFilter();
  /** Main computation method */
  virtual void GenerateData();
  /** PrintSelf method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const;
  /**
   * Get the composition knowledge.
   * \param r1 First RCC8 relation value,
   * \param r2 Second RCC8 relation value,
   * \return The knowledge associated with the composition.
   */
  KnowledgeStateType GetKnowledge(RCC8ValueType r1, RCC8ValueType r2);

private:
  /** Optimisation flag */
  bool m_Optimisation;
  unsigned int m_Accumulator[8];

};
} // End namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbPolygonListToRCC8GraphFilter.txx"
#endif

#endif
