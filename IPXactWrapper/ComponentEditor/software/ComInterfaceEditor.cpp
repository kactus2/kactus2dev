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

#include <LibraryManager/libraryinterface.h>
#include <models/ComInterface.h>
#include <models/ComDefinition.h>

#include <QScrollArea>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>

ComInterfaceEditor::ComInterfaceEditor(LibraryInterface* libHandler,
									   QSharedPointer<Component> component,
									   QSharedPointer<ComInterface> comInterface,
									   QWidget *parent):
ItemEditor(component, parent),
libInterface_(libHandler),
comIf_(comInterface.data()),
nameEditor_(comInterface->getNameGroup(), this, tr("Name and description")),
comType_(VLNV::COMDEFINITION, libHandler, this, this),
detailsGroup_(tr("Details"), this),
transferTypeCombo_(this),
directionCombo_(this),
propertyValueEditor_(this) {

	// Set VLNV editor settings.
	comType_.setTitle(tr("COM definition"));
	comType_.setMandatory(false);

	// Initialize the details group.
	QLabel* transferTypeLabel = new QLabel(tr("Transfer type:"), &detailsGroup_);
	transferTypeCombo_.addItem("");
	transferTypeCombo_.setInsertPolicy(QComboBox::InsertAlphabetically);

	QLabel* directionLabel = new QLabel(tr("Direction:"), &detailsGroup_);
	directionCombo_.addItem(tr("in"));
	directionCombo_.addItem(tr("out"));
	directionCombo_.addItem(tr("inout"));
	directionCombo_.setCurrentIndex(0);
	directionCombo_.setProperty("mandatoryField", true);

	QGridLayout* detailsLayout = new QGridLayout(&detailsGroup_);
	detailsLayout->addWidget(transferTypeLabel, 0, 0, 1, 1);
	detailsLayout->addWidget(&transferTypeCombo_, 0, 1, 1, 1);
	detailsLayout->addWidget(directionLabel, 1, 0, 1, 1);
	detailsLayout->addWidget(&directionCombo_, 1, 1, 1, 1);

	detailsLayout->setColumnMinimumWidth(1, 150);
	detailsLayout->setColumnStretch(2, 1);

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
	QVBoxLayout* layout = new QVBoxLayout(topWidget);
	layout->addWidget(&nameEditor_);
	layout->addWidget(&comType_);
	layout->addWidget(&detailsGroup_);
	layout->addWidget(&propertyValueEditor_);
	layout->addStretch();
	layout->setContentsMargins(0, 0, 0, 0);

	scrollArea->setWidget(topWidget);

	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	
	connect(&transferTypeCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onTransferTypeChange()), Qt::UniqueConnection);
	connect(&directionCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onDirectionChange(int)), Qt::UniqueConnection);
	connect(&propertyValueEditor_, SIGNAL(contentChanged()),
		this, SLOT(onPropertiesChange()), Qt::UniqueConnection);

	connect(&comType_, SIGNAL(vlnvEdited()),
		this, SLOT(onComDefinitionChanged()), Qt::UniqueConnection);

	refresh();
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::~ComInterfaceEditor()
//-----------------------------------------------------------------------------
ComInterfaceEditor::~ComInterfaceEditor() {
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::isValid()
//-----------------------------------------------------------------------------
bool ComInterfaceEditor::isValid() const
{
    return (nameEditor_.isValid() &&
            (comType_.isEmpty() || (comType_.isValid() && libInterface_->contains(comType_.getVLNV()))));
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::onComDefinitionChanged()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::onComDefinitionChanged()
{
	comIf_->setComType(comType_.getVLNV());

	disconnect(&transferTypeCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onTransferTypeChange()));
    if (comType_.getVLNV().isValid())
    {
        // Retrieve the COM definition from the library.
        QSharedPointer<LibraryComponent const> libComp = libInterface_->getModelReadOnly(comType_.getVLNV());
        QSharedPointer<ComDefinition const> comDef = libComp.dynamicCast<ComDefinition const>();

        if (comDef == 0)
        {
            emit errorMessage("COM definition with the given VLNV does not exist");
            return;
        }

        propertyValueEditor_.setAllowedProperties(&comDef->getProperties());

        QString type = transferTypeCombo_.currentText();

        transferTypeCombo_.clear();
        transferTypeCombo_.addItem("");
        transferTypeCombo_.addItems(comDef->getTransferTypes());

        if (comDef->getTransferTypes().contains(type))
        {
            transferTypeCombo_.setCurrentIndex(transferTypeCombo_.findText(type));
        }
    }
    else if (comType_.getVLNV().isEmpty())
    {
        // Clear the allowed properties.
        propertyValueEditor_.setAllowedProperties(0);

        // Clear the data type combo.
        transferTypeCombo_.clear();
        transferTypeCombo_.addItem("");
    }
	connect(&transferTypeCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onTransferTypeChange()), Qt::UniqueConnection);
	emit contentChanged();
}

void ComInterfaceEditor::refresh() {
	nameEditor_.refresh();

	VLNV comVLNV = comIf_->getComType();
	
	comType_.setVLNV(comVLNV);
	propertyValueEditor_.setData(comIf_->getPropertyValues());

	disconnect(&directionCombo_, SIGNAL(currentIndexChanged(int)),
				this, SLOT(onDirectionChange(int)));
	directionCombo_.setCurrentIndex(comIf_->getDirection());
	connect(&directionCombo_, SIGNAL(currentIndexChanged(int)),
			this, SLOT(onDirectionChange(int)), Qt::UniqueConnection);

	disconnect(&transferTypeCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onTransferTypeChange()));
	if (comVLNV.isValid())
	{
		// Retrieve the COM definition from the library.
		QSharedPointer<LibraryComponent const> libComp = libInterface_->getModelReadOnly(comVLNV);
		QSharedPointer<ComDefinition const> comDef = libComp.dynamicCast<ComDefinition const>();

		if (comDef == 0)
		{
			emit errorMessage("COM definition with the given VLNV does not exist");
			return;
		}

		propertyValueEditor_.setAllowedProperties(&comDef->getProperties());

		QString type = comIf_->getTransferType();

		transferTypeCombo_.clear();
		transferTypeCombo_.addItem("");
		transferTypeCombo_.addItems(comDef->getTransferTypes());

		if (comDef->getTransferTypes().contains(type))
		{
			transferTypeCombo_.setCurrentIndex(transferTypeCombo_.findText(type));
		}
	}
	else if (comVLNV.isEmpty())
	{
		// Clear the allowed properties.
		propertyValueEditor_.setAllowedProperties(0);

		// Clear the data type combo.
		transferTypeCombo_.clear();
		transferTypeCombo_.addItem("");
	}
	connect(&transferTypeCombo_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onTransferTypeChange()), Qt::UniqueConnection);
}

void ComInterfaceEditor::onTransferTypeChange() {
	comIf_->setTransferType(transferTypeCombo_.currentText());
	emit contentChanged();
}

void ComInterfaceEditor::onDirectionChange( int index ) {
	 comIf_->setDirection(static_cast<General::Direction>(index));
	 emit contentChanged();
}

void ComInterfaceEditor::onPropertiesChange() {
	QMap<QString, QString> propertyValues = propertyValueEditor_.getData();
	comIf_->setPropertyValues(propertyValues);
	emit contentChanged();
}

void ComInterfaceEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/cominterface.html");
}
