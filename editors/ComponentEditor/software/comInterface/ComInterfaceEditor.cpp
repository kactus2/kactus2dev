//-----------------------------------------------------------------------------
// File: ComInterfaceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.4.2012
//
// Description:
// COM interface editor for the component editor.
//-----------------------------------------------------------------------------

#include "ComInterfaceEditor.h"

#include <library/LibraryManager/libraryinterface.h>

#include <mainwindow/mainwindow.h>

#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/ComDefinition.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QApplication>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::ComInterfaceEditor()
//-----------------------------------------------------------------------------
ComInterfaceEditor::ComInterfaceEditor(LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    QSharedPointer<ComInterface> comInterface,
    QWidget *parent):
ItemEditor(component, libHandler, parent),
    libInterface_(libHandler),
    comIf_(comInterface.data()),
    nameEditor_(comInterface, this, tr("Name and description")),
    comTypeEditor_(0),
    detailsGroup_(tr("Details"), this),
    transferTypeCombo_(this),
    directionCombo_(this),
    propertyValueEditor_(this),
    comImplementation_(0)
{
	// find the main window for VLNV editor
	QWidget* parentW = NULL;
	foreach (QWidget* widget, QApplication::topLevelWidgets())
 {
		MainWindow* mainWnd = dynamic_cast<MainWindow*>(widget);
		if (mainWnd) {
			parentW = mainWnd;
			break;
		}
	}

	comTypeEditor_ = new VLNVEditor(VLNV::COMDEFINITION, libHandler, parentW, this);

	// Set VLNV editor settings.
	comTypeEditor_->setTitle(tr("COM definition"));
	comTypeEditor_->setMandatory(false);

	comImplementation_ = new VLNVEditor(VLNV::COMPONENT, libHandler, parentW, this);
	comImplementation_->setTitle(tr("COM interface implementation reference"));
	comImplementation_->setMandatory(false);
	comImplementation_->addContentType(VLNV::DESIGN);
	comImplementation_->addContentType(VLNV::DESIGNCONFIGURATION);

    // Initialize the details group.
    transferTypeCombo_.addItem("");
    transferTypeCombo_.setInsertPolicy(QComboBox::InsertAlphabetically);

    directionCombo_.addItem(tr("in"));
    directionCombo_.addItem(tr("out"));
    directionCombo_.addItem(tr("inout"));
    directionCombo_.setCurrentIndex(0);
    directionCombo_.setProperty("mandatoryField", true);

    setupLayout();

	connect(&nameEditor_, SIGNAL(contentChanged()),	this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	
	connect(&transferTypeCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onTransferTypeChange()), Qt::UniqueConnection);

	connect(&directionCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onDirectionChange(int)), Qt::UniqueConnection);

	connect(&propertyValueEditor_, SIGNAL(contentChanged()),
		this, SLOT(onPropertiesChange()), Qt::UniqueConnection);

	connect(comTypeEditor_, SIGNAL(vlnvEdited()),	this, SLOT(onComDefinitionChanged()), Qt::UniqueConnection);
	
	connect(comImplementation_, SIGNAL(vlnvEdited()), 
        this, SLOT(onComImplementationChange()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::~ComInterfaceEditor()
//-----------------------------------------------------------------------------
ComInterfaceEditor::~ComInterfaceEditor()
{
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::isValid()
//-----------------------------------------------------------------------------
bool ComInterfaceEditor::isValid() const
{
    return (nameEditor_.isValid() &&
        (comTypeEditor_->isEmpty() || (comTypeEditor_->isValid() && libInterface_->contains(comTypeEditor_->getVLNV()))) &&
        (comImplementation_->isEmpty() || comImplementation_->isValid()));
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::onComDefinitionChanged()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::onComDefinitionChanged()
{
    VLNV comDefinitionVLNV = comTypeEditor_->getVLNV();

	comIf_->setComType(comDefinitionVLNV);

	disconnect(&transferTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(onTransferTypeChange()));

    if (comDefinitionVLNV.isValid())
    {
        updateEditorsForComDefinition(comDefinitionVLNV);        
    }
    else if (comDefinitionVLNV.isEmpty())
    {
        clearEditors();
    }

	connect(&transferTypeCombo_, SIGNAL(currentIndexChanged(int)), 
        this, SLOT(onTransferTypeChange()), Qt::UniqueConnection);

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::refresh()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::refresh()
{
	nameEditor_.refresh();

	VLNV comVLNV = comIf_->getComType();
	
	comTypeEditor_->setVLNV(comVLNV);
	propertyValueEditor_.setData(comIf_->getPropertyValues());

	comImplementation_->setVLNV(comIf_->getComImplementation());

	disconnect(&directionCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(onDirectionChange(int)));
	directionCombo_.setCurrentIndex(comIf_->getDirection());
	connect(&directionCombo_, SIGNAL(currentIndexChanged(int)),
			this, SLOT(onDirectionChange(int)), Qt::UniqueConnection);

	disconnect(&transferTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(onTransferTypeChange()));

	if (comVLNV.isValid())
	{
		updateEditorsForComDefinition(comVLNV);
	}
	else if (comVLNV.isEmpty())
	{
        clearEditors();
	}

	connect(&transferTypeCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onTransferTypeChange()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::onTransferTypeChange()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::onTransferTypeChange()
{
	comIf_->setTransferType(transferTypeCombo_.currentText());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::onDirectionChange()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::onDirectionChange(int index)
{
	 comIf_->setDirection(static_cast<DirectionTypes::Direction>(index));
	 emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::onPropertiesChange()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::onPropertiesChange()
{
	QMap<QString, QString> propertyValues = propertyValueEditor_.getData();
	comIf_->setPropertyValues(propertyValues);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::onComImplementationChange()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::onComImplementationChange()
{
    comIf_->setComImplementation(comImplementation_->getVLNV());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::showEvent()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/cominterface.html");
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::setComDefinition()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::updateEditorsForComDefinition(VLNV const& comDefinitionVLNV)
{
    // Retrieve the COM definition from the library.
    QSharedPointer<Document const> libComp = libInterface_->getModelReadOnly(comDefinitionVLNV);
    QSharedPointer<ComDefinition const> comDef = libComp.dynamicCast<ComDefinition const>();

    if (comDef == 0)
    {
        emit errorMessage("COM definition with the given VLNV does not exist");
        return;
    }

    propertyValueEditor_.setAllowedProperties(*comDef->getProperties());

    QString type = transferTypeCombo_.currentText();

    transferTypeCombo_.clear();
    transferTypeCombo_.addItem("");
    transferTypeCombo_.addItems(*comDef->getTransferTypes());

    if (comDef->getTransferTypes()->contains(type))
    {
        transferTypeCombo_.setCurrentIndex(transferTypeCombo_.findText(type));
    }
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::clearComDefinition()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::clearEditors()
{
    // Clear the allowed properties.
    propertyValueEditor_.setAllowedProperties(QList<QSharedPointer<ComProperty> >());

    // Clear the data type combo.
    transferTypeCombo_.clear();
    transferTypeCombo_.addItem("");
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::setupLayout()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::setupLayout()
{
    QFormLayout* detailsLayout = new QFormLayout(&detailsGroup_);
    detailsLayout->addRow(tr("Transfer type:"), &transferTypeCombo_);
    detailsLayout->addRow(tr("Direction:"), &directionCombo_);

    // Create the scroll area.
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    // Create the top widget and set it under the scroll area.
    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Create the layout for the actual editor.
    QGridLayout* layout = new QGridLayout(topWidget);
    layout->addWidget(&nameEditor_, 0, 0, 1, 1);
    layout->addWidget(&detailsGroup_, 0, 1, 1, 1);
    layout->addWidget(comTypeEditor_, 1, 0, 1, 1);
    layout->addWidget(comImplementation_, 1, 1, 1, 1);
    layout->addWidget(&propertyValueEditor_, 2, 0, 1, 2);
    layout->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(topWidget);
}
