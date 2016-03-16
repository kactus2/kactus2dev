//-----------------------------------------------------------------------------
// File: ColumnEditDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 15.4.2011
//
// Description:
// Dialog for editing diagram column properties.
//-----------------------------------------------------------------------------

#include "ColumnEditDialog.h"

#include <QMessageBox>
#include <QCoreApplication>

namespace
{
    const unsigned int ALLOWED_ITEMS[ColumnTypes::CUSTOM] = 
    {
        ColumnTypes::INTERFACE,                             //<! For COLUMN_CONTENT_IO.
        ColumnTypes::CHANNEL | ColumnTypes::BRIDGE,     //<! For COLUMN_CONTENT_BUSES.
        ColumnTypes::COMPONENT,                             //<! For COLUMN_CONTENT_COMPONENTS.
        ColumnTypes::COMPONENT                              //<! For COLUMN_CONTENT_MEMORY.
    };

    const QString ITEM_NAMES[ColumnTypes::TYPE_COUNT] = 
    {
        QObject::tr("Interfaces"),
        QObject::tr("Components"),
        QObject::tr("Channels"),
        QObject::tr("Bridges")        
    };
}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::ColumnEditDialog()
//-----------------------------------------------------------------------------
ColumnEditDialog::ColumnEditDialog(QWidget* parent, bool sw, GraphicsColumn const* column)
    : QDialog(parent), 
      layout_(new QVBoxLayout(this)), 
      nameLabel_(new QLabel(tr("Name:"), this)), 
      nameEdit_(new QLineEdit(this)),
      typeLabel_(new QLabel(tr("Type:"), this)), 
      typeCombo_(new QComboBox(this)),
      allowedItemsGroup_(new QGroupBox(tr("Allowed Items"), this)),       
      buttons_(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)),
      column_(column),
      types_()
{
    setupLayout(sw);
    initializeTypes(sw);

    // Fill the fields with the data from the column, if given.
    if (column_ != 0)
    {
        setWindowTitle(tr("Edit Column"));
        nameEdit_->setText(column_->name());
        setAllowedItems(column_->getColumnDesc()->getAllowedItems());

        QString typeName = types_.key(column_->getContentType(), tr("IO"));
        typeCombo_->setCurrentIndex(typeCombo_->findText(typeName));
    }
    else
    {
        setWindowTitle(tr("Add Column"));
        setAllowedItems(ColumnTypes::NONE);
        typeCombo_->setCurrentIndex((typeCombo_->findText(tr("IO"))));
    }

    onTypeChange(typeCombo_->currentText());

    connect(typeCombo_, SIGNAL(currentIndexChanged(QString const&)), this, SLOT(onTypeChange(QString const&)));
    connect(buttons_, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons_, SIGNAL(rejected()), this, SLOT(reject()));
}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::~ColumnEditDialog()
//-----------------------------------------------------------------------------
ColumnEditDialog::~ColumnEditDialog()
{

}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::hideContentSettings()
//-----------------------------------------------------------------------------
void ColumnEditDialog::hideContentSettings()
{
    typeLabel_->hide();
    typeCombo_->hide();
    allowedItemsGroup_->hide();

    layout_->activate();
    setFixedSize(sizeHint());
}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::onTypeChange()
//-----------------------------------------------------------------------------
void ColumnEditDialog::onTypeChange(QString const& type)
{
    // Convert the index to a column content type.
    ColumnTypes::ColumnContentType contentType = types_.value(type, ColumnTypes::CUSTOM);
    
    // Set enabled/disabled based on the content type.
    for (int i = 0; i < ColumnTypes::TYPE_COUNT; ++i)
    {
        itemCheckBoxes_[i]->setEnabled(contentType == ColumnTypes::CUSTOM);
    }

    if (contentType != ColumnTypes::CUSTOM)
    {
        setAllowedItems(ALLOWED_ITEMS[contentType]);
    }
}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::setAllowedItems()
//-----------------------------------------------------------------------------
void ColumnEditDialog::setAllowedItems(unsigned int allowedItems)
{
    for (int i = 0; i < ColumnTypes::TYPE_COUNT; ++i)
    {
        // Set the checked states based on the corresponding flag.
        itemCheckBoxes_[i]->setChecked((1 << i) & allowedItems);
    }
}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::name()
//-----------------------------------------------------------------------------
QString ColumnEditDialog::name() const
{
    return nameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::getContentType()
//-----------------------------------------------------------------------------
ColumnTypes::ColumnContentType ColumnEditDialog::getContentType() const
{
    // Convert the index to a column content type.   
    QString type = typeCombo_->currentText();
    return types_.value(type, ColumnTypes::CUSTOM);
}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::getAllowedItems()
//-----------------------------------------------------------------------------
unsigned int ColumnEditDialog::getAllowedItems() const
{
    unsigned int allowedItems = ColumnTypes::NONE;

    for (int i = 0; i < ColumnTypes::TYPE_COUNT; ++i)
    {
        allowedItems |= (itemCheckBoxes_[i]->isChecked() << i);
    }

    return allowedItems;
}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::accept()
//-----------------------------------------------------------------------------
void ColumnEditDialog::accept()
{
    if (column_ != 0 && !column_->isAllowedItemsValid(getAllowedItems()))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("The column contains items that would not be allowed in the new column configuration." 
            "Please delete the items of the unallowed type before changing the column configuration."),
            QMessageBox::Ok, (QWidget*)parent());
        msgBox.exec();
        return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::initializeTypes()
//-----------------------------------------------------------------------------
void ColumnEditDialog::initializeTypes(bool sw)
{
    types_.insert(tr("IO"), ColumnTypes::IO);
    types_.insert(tr("Components"), ColumnTypes::COMPONENTS);    

    if (!sw)
    {
        types_.insert(tr("Buses"), ColumnTypes::BUSES);
        types_.insert(tr("Memory"), ColumnTypes::MEMORY);
        types_.insert(tr("Custom"), ColumnTypes::CUSTOM);
    }

    foreach(QString typeName, types_.keys())
    {
        typeCombo_->addItem(typeName);
    }
}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::setupLayout()
//-----------------------------------------------------------------------------
void ColumnEditDialog::setupLayout(bool sw)
{
    layout_->addWidget(nameLabel_);
    layout_->addWidget(nameEdit_);
    layout_->addWidget(typeLabel_);
    layout_->addWidget(typeCombo_);

    // Create the allowed items group box and check boxes.
    QVBoxLayout* itemLayout = new QVBoxLayout(allowedItemsGroup_);

    for (int i = 0; i < ColumnTypes::TYPE_COUNT; ++i)
    {
        itemCheckBoxes_[i] = new QCheckBox(ITEM_NAMES[i], allowedItemsGroup_);
        itemLayout->addWidget(itemCheckBoxes_[i]);
    }

    allowedItemsGroup_->setVisible(!sw);

    layout_->addWidget(allowedItemsGroup_);
    layout_->addWidget(buttons_);

    setFixedSize(sizeHint());
}
