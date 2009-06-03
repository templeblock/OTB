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
#include "itkExceptionObject.h"
#include "otbMacro.h"

#include "otbPostGISCreateTableTransactor.h"
#include "otbPostGISConnectionImplementation.h"

int otbPostGISCreateTableTransactorCreate(int argc, char * argv[])
{
  typedef otb::PostGISCreateTableTransactor TransactorType;

  //Instantiation
  TransactorType myTransactor;

  unsigned short dimension = 2;  
  myTransactor.SetDimension( dimension );

  std::string name = "mytable";
  myTransactor.SetTableName( name );

  int srid = -1;
  myTransactor.SetSRID( srid );



  const std::string hostName = argv[1];
  const std::string dbName = argv[2];
  const std::string userName = argv[3];
  
  typedef otb::PostGISConnectionImplementation GISConnectionType;

  //Instantiation
  GISConnectionType::Pointer connection = GISConnectionType::New();

  connection->SetHost( hostName );
  connection->SetDBName( dbName );
  connection->SetUser( userName );

  connection->ConnectToDB();

  connection->PerformTransaction( myTransactor );

  return EXIT_SUCCESS;
}