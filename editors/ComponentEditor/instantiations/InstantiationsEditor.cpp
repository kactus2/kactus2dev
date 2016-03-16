//-----------------------------------------------------------------------------
// File: InstantiationsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 29.01.2016
//
// Description:
// Editor for component, design and design configuration instantiations.
//-----------------------------------------------------------------------------

#include "InstantiationsEditor.h"

#include "ComponentInstantiationsEditor.h"
#include "DesignConfigurationInstantiationsEditor.h"
#include "DesignInstantiationsEditor.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: InstantiationsEditor::InstantiationsEditor()
//-----------------------------------------------------------------------------
InstantiationsEditor::InstantiationsEditor(QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder, LibraryInterface* handler,
    QSharedPointer<InstantiationsValidator> validator, QWidget* parent /* = 0 */):
ItemEditor(component, handler, parent),
componentInstantiationsEditor_(new ComponentInstantiationsEditor(component, handler, parameterFinder, validator,
                                                                 this)),
designConfigurationInstantiationsEditor_(new DesignConfigurationInstantiationsEditor(component, handler, validator,
                                                                                     this)),
designInstantiationsEditor_(new DesignInstantiationsEditor(component, handler, validator, this))
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(componentInstantiationsEditor_);
    layout->addWidget(designConfigurationInstantiationsEditor_);
    layout->addWidget(designInstantiationsEditor_);
	layout->setContentsMargins(0, 0, 0, 0);

    connect(componentInstantiationsEditor_, SIGNAL(contentChanged()), 
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(componentInstantiationsEditor_, SIGNAL(childAdded(int)), 
        this, SIGNAL(componentInstanceAdded(int)), Qt::UniqueConnection);
    connect(componentInstantiationsEditor_, SIGNAL(childRemoved(int)), 
        this, SIGNAL(componentInstanceRemoved(int)), Qt::UniqueConnection);

    connect(designConfigurationInstantiationsEditor_, SIGNAL(contentChanged()), 
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(designConfigurationInstantiationsEditor_, SIGNAL(childAdded(int)), 
        this, SIGNAL(designConfigurationInstanceAdded(int)), Qt::UniqueConnection);
    connect(designConfigurationInstantiationsEditor_, SIGNAL(childRemoved(int)), 
        this, SIGNAL(designConfigurationInstanceRemoved(int)), Qt::UniqueConnection);

    connect(designInstantiationsEditor_, SIGNAL(contentChanged()), 
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(designInstantiationsEditor_, SIGNAL(childAdded(int)), 
        this, SIGNAL(designInstanceAdded(int)), Qt::UniqueConnection);
    connect(designInstantiationsEditor_, SIGNAL(childRemoved(int)), 
        this, SIGNAL(designInstanceRemoved(int)), Qt::UniqueConnection);

    connect(componentInstantiationsEditor_, SIGNAL(decreaseReferences(QString)), this,
        SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InstantiationsEditor::~InstantiationsEditor()
//-----------------------------------------------------------------------------
InstantiationsEditor::~InstantiationsEditor()
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsEditor::refresh()
//-----------------------------------------------------------------------------
void InstantiationsEditor::refresh()
{    
    componentInstantiationsEditor_->refresh();
    designConfigurationInstantiationsEditor_->refresh();
    designInstantiationsEditor_->refresh();
}

//-----------------------------------------------------------------------------
// Function: InstantiationsEditor::showEvent()
//-----------------------------------------------------------------------------
void InstantiationsEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/instantiations.html");
}
