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

#include "otbBasicApplicationModel.h"
#include "otbFltkFilterWatcher.h"

namespace otb
{
/** Initialize the singleton */
BasicApplicationModel::Pointer BasicApplicationModel::Instance = NULL;

/** Manage the singleton */
BasicApplicationModel::Pointer
BasicApplicationModel::GetInstance()
{
  if (!Instance)
    {
    Instance = BasicApplicationModel::New();
    }
  return Instance;
}

void BasicApplicationModel::Notify(ListenerBase * listener)
{
  listener->Notify();
}

BasicApplicationModel::BasicApplicationModel() : m_VisualizationModel(),
  m_Reader()
{
  m_VisualizationModel    = VisualizationModelType::New();
  m_Reader = VectorReaderType::New();

}

BasicApplicationModel
::~BasicApplicationModel() {}

void
BasicApplicationModel
::OpenImage(const char * filename)
{
  m_Reader->SetFileName(filename);
  m_Reader->UpdateOutputInformation();

  // Generate the layer
  LayerGeneratorType::Pointer generator = LayerGeneratorType::New();
  generator->SetImage(m_Reader->GetOutput());
  generator->GenerateQuicklookOn();
  FltkFilterWatcher qlwatcher(
    generator->GetProgressSource(), 0, 0, 200, 20, "Generating QuickLook ...");
  generator->GenerateLayer();

  generator->GetLayer()->SetName("Image");

  // Clear previous layers
  m_VisualizationModel->ClearLayers();

  // Add the layer to the models
  m_VisualizationModel->AddLayer(generator->GetLayer());

  m_VisualizationModel->Update();
  this->NotifyAll();
}

}
