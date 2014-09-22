//-----------------------------------------------------------------------------
// File: ModelParameterEditorAdapter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.09.2014
//
// Description:
// Adapter for ModelParameterEditor to implement ModelParameterVisualizer. 
//-----------------------------------------------------------------------------

#include "ModelParameterEditorAdapter.h"

#include <IPXACTmodels/modelparameter.h>

#include <editors/ComponentEditor/modelParameters/modelparametereditor.h>

//-----------------------------------------------------------------------------
// Function: PortEditorAdapter::PortEditorAdapter()
//-----------------------------------------------------------------------------
ModelParameterEditorAdapter::ModelParameterEditorAdapter(ModelParameterEditor* editor) : editor_(editor)
{

}

//-----------------------------------------------------------------------------
// Function: PortEditorAdapter::~PortEditorAdapter()
//-----------------------------------------------------------------------------
ModelParameterEditorAdapter::~ModelParameterEditorAdapter()
{

}

//-----------------------------------------------------------------------------
// Function: PortEditorAdapter::addModelParameter()
//-----------------------------------------------------------------------------
void ModelParameterEditorAdapter::addModelParameter(QSharedPointer<ModelParameter> modelParameterToAdd)
{
    editor_->addModelParameter(modelParameterToAdd);
}

//-----------------------------------------------------------------------------
// Function: PortEditorAdapter::removeModelParameter()
//-----------------------------------------------------------------------------
void ModelParameterEditorAdapter::removeModelParameter(QSharedPointer<ModelParameter> modelParameterToRemove)
{
    editor_->removeModelParameter(modelParameterToRemove);
}

//-----------------------------------------------------------------------------
// Function: ModelParameterEditorAdapter::registerChangeListener()
//-----------------------------------------------------------------------------
void ModelParameterEditorAdapter::registerChangeListener(QObject const* listener)
{
    QObject::connect(editor_, SIGNAL(parameterChanged(QSharedPointer<ModelParameter>)),
        listener, SLOT(onModelParameterChanged(QSharedPointer<ModelParameter>)), 
        Qt::UniqueConnection);
}
