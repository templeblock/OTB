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
#include "otbInverseLogPolarTransform.h"
#include "otbMacro.h"

int otbInverseLogPolarTransform(int argc, char* argv[])
{
try
  {
    double radialStep = atof(argv[1]);
    double angularStep = atof(argv[2]);
    unsigned int nbPoints = atoi(argv[3]);

    typedef double PrecisionType;
    typedef otb::InverseLogPolarTransform<PrecisionType> InverseLogPolarTransformType;
    typedef itk::Point<PrecisionType,2> PointType;
    typedef std::vector<PointType> PointsVectorType;
    // input points retrieval
    PointsVectorType vect;
    for(unsigned int i=0;i<nbPoints;++i)
      {
	PointType p;
	p[0]=atof(argv[4+2*i]);
	p[1]=atof(argv[5+2*i]);
	std::cout<<"Adding point "<<p<<"."<<std::endl;
	vect.push_back(p);
      }
    // Instantiation
    InverseLogPolarTransformType::Pointer transform = InverseLogPolarTransformType::New();
    InverseLogPolarTransformType::ParametersType params(4);
    params[0]=0.;
    params[1]=0.;
    params[2]=radialStep;
    params[3]=angularStep;
    transform->SetParameters(params);

    for(PointsVectorType::iterator it=vect.begin();it!=vect.end();++it)
      {
	PointType p = transform->TransformPoint(*it);
	PointType pprime;
	if(((*it)[0]==0)&&((*it)[0]==0))
	  {
	    pprime.Fill(0);
	  }
	else
	  {
	    pprime[0]=(1/angularStep)*asin((*it)[1]/sqrt((*it)[0]*(*it)[0]+(*it)[1]*(*it)[1]));
	    pprime[1]=(1/2*radialStep)*log((*it)[0]*(*it)[0]+(*it)[1]*(*it)[1]);
	  }

	std::cout<<"Original Point: "<<(*it)<<", Reference point: "<<pprime<<", Transformed point: "<<p<<std::endl;
	otbControlConditionTestMacro(p[0]!=pprime[0],"Error while transforming point.");
	otbControlConditionTestMacro(p[1]!=pprime[1],"Error while transforming point.");
      }
  }
catch( itk::ExceptionObject & err ) 
  { 
    std::cout << "Exception itk::ExceptionObject thrown !" << std::endl; 
    std::cout << err << std::endl; 
    return EXIT_FAILURE;
  } 
catch( ... ) 
  { 
    std::cout << "Unknown exception thrown !" << std::endl; 
    return EXIT_FAILURE;
  } 
 return EXIT_SUCCESS;













}
