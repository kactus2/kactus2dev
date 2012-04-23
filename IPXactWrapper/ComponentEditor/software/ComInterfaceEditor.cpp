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

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::ComInterfaceEditor()
//-----------------------------------------------------------------------------
ComInterfaceEditor::ComInterfaceEditor(LibraryInterface* libHandler,
                                       QSharedPointer<Component> component,
                                       void* dataPointer, QWidget *parent)
    : ItemEditor(component, parent),
      libInterface_(libHandler),
      comIf_(static_cast<ComInterface*>(dataPointer)),
      nameGroup_(this),
      comType_(VLNV::COMDEFINITION, libHandler, this, this),
      detailsGroup_(tr("Details"), this),
      dataTypeCombo_(this),
      directionCombo_(this),
      propertyValueEditor_(this)
{
    Q_ASSERT(dataPointer != 0);
    Q_ASSERT(libHandler != 0);

    connect(&nameGroup_, SIGNAL(contentChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&nameGroup_, SIGNAL(nameChanged(const QString&)),
            this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
    connect(&comType_, SIGNAL(contentChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&dataTypeCombo_, SIGNAL(currentIndexChanged(int)),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&dataTypeCombo_, SIGNAL(editTextChanged(QString const&)),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&directionCombo_, SIGNAL(currentIndexChanged(int)),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&propertyValueEditor_, SIGNAL(contentChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&comType_, SIGNAL(contentChanged()),
            this, SLOT(onComDefinitionChanged()), Qt::UniqueConnection);
    
    // Set name group and VLNV editor settings.
    nameGroup_.setTitle(tr("Name and description"));
    comType_.setTitle(tr("COM definition"));
    comType_.setMandatory(false);

    // Initialize the details group.
    QLabel* dataTypeLabel = new QLabel(tr("Data type:"), &detailsGroup_);
    dataTypeCombo_.setEditable(true);
    dataTypeCombo_.setInsertPolicy(QComboBox::InsertAlphabetically);

    QLabel* directionLabel = new QLabel(tr("Direction:"), &detailsGroup_);
    directionCombo_.addItem(tr("in"));
    directionCombo_.addItem(tr("out"));
    directionCombo_.addItem(tr("inout"));
    directionCombo_.setCurrentIndex(0);

    QGridLayout* detailsLayout = new QGridLayout(&detailsGroup_);
    detailsLayout->addWidget(dataTypeLabel, 0, 0, 1, 1);
    detailsLayout->addWidget(&dataTypeCombo_, 0, 1, 1, 1);
    detailsLayout->addWidget(directionLabel, 1, 0, 1, 1);
    detailsLayout->addWidget(&directionCombo_, 1, 1, 1, 1);

    detailsLayout->setColumnMinimumWidth(1, 150);
    detailsLayout->setColumnStretch(2, 1);

    // Create the scroll area.
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);

    // Create the top widget and set it under the scroll area.
    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Create the layout for the actual editor.
    QVBoxLayout* layout = new QVBoxLayout(topWidget);
    layout->addWidget(&nameGroup_);
    layout->addWidget(&comType_);
    layout->addWidget(&detailsGroup_);
    layout->addWidget(&propertyValueEditor_);
    layout->addStretch();

    scrollArea->setWidget(topWidget);

    restoreChanges();
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
    return (nameGroup_.isValid() &&
            (comType_.isEmpty() || (comType_.isValid() && libInterface_->contains(comType_.getVLNV()))));
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::removeModel()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::removeModel()
{
    component()->removeComInterface(comIf_);
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::makeChanges()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::makeChanges()
{
    if (!isValid())
    {
        return;
    }

    comIf_->setName(nameGroup_.getName());
    comIf_->setDisplayName(nameGroup_.getDisplayName());
    comIf_->setDescription(nameGroup_.getDescription());
    comIf_->setComType(comType_.getVLNV());
    comIf_->setDataType(dataTypeCombo_.currentText());
    comIf_->setDirection(static_cast<General::Direction>(directionCombo_.currentIndex()));

    QMap<QString, QString> propertyValues = propertyValueEditor_.getData();
    comIf_->setPropertyValues(propertyValues);

    component()->updateComInteface(comIf_);
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::restoreChanges()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::restoreChanges()
{
    nameGroup_.setName(comIf_->getName());
    nameGroup_.setDisplayName(comIf_->getDisplayName());
    nameGroup_.setDescription(comIf_->getDescription());

    propertyValueEditor_.setData(comIf_->getPropertyValues());

    comType_.setVLNV(comIf_->getComType());
    directionCombo_.setCurrentIndex(comIf_->getDirection());

    if (!comIf_->getDataType().isEmpty())
    {
        int index = dataTypeCombo_.findText(comIf_->getDataType());

        if (index == -1)
        {
            dataTypeCombo_.addItem(comIf_->getDataType());
            index = dataTypeCombo_.findText(comIf_->getDataType());
        }

        dataTypeCombo_.setCurrentIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: ComInterfaceEditor::onComDefinitionChanged()
//-----------------------------------------------------------------------------
void ComInterfaceEditor::onComDefinitionChanged()
{
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
    }
}
