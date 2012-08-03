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

#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog()
//-----------------------------------------------------------------------------
ColumnEditDialog::ColumnEditDialog(QWidget* parent, bool sw, GraphicsColumn const* column)
    : QDialog(parent), layout_(0), nameLabel_(0), typeLabel_(0), typeCombo_(0),
      allowedItemsGroup_(0), column_(column)
{
    layout_ = new QGridLayout(this);
    
    // Create the name label and line edit.
    nameLabel_ = new QLabel(tr("Name:"), this);
    layout_->addWidget(nameLabel_, 0, 0, 1, 2);

    nameEdit_ = new QLineEdit(this);
    layout_->addWidget(nameEdit_, 1, 0, 1, 2);

    // Create the type label and combo box.
    typeLabel_ = new QLabel(tr("Type:"), this);
    layout_->addWidget(typeLabel_, 2, 0, 1, 2);

    typeCombo_ = new QComboBox(this);
    typeCombo_->addItem(tr("IO"));
    typeCombo_->addItem(tr("Components"));

    if (!sw)
    {
        typeCombo_->addItem(tr("Buses"));
        typeCombo_->addItem(tr("Custom"));
    }

    typeCombo_->setCurrentIndex(-1);

    layout_->addWidget(typeCombo_, 3, 0, 1, 2);

    // Create the allowed items group box and check boxes.
    allowedItemsGroup_ = new QGroupBox(tr("Allowed Items"), this);

    QGridLayout* itemLayout = new QGridLayout(allowedItemsGroup_);

    itemCheckBoxes_[0] = new QCheckBox(tr("Interfaces"), allowedItemsGroup_);
    itemCheckBoxes_[1] = new QCheckBox(tr("Components"), allowedItemsGroup_);
    itemCheckBoxes_[2] = new QCheckBox(tr("Channels"), allowedItemsGroup_);
    itemCheckBoxes_[3] = new QCheckBox(tr("Bridges"), allowedItemsGroup_);

    for (int i = 0; i < CIT_COUNT; ++i)
    {
        itemLayout->addWidget(itemCheckBoxes_[i], i, 0, 1, 1);
    }

    layout_->addWidget(allowedItemsGroup_, 4, 0, 1, 2);
    allowedItemsGroup_->setVisible(!sw);

    // Create the OK and Cancel buttons.
    QPushButton* btnOK = new QPushButton(this);
    btnOK->setText(tr("&OK"));
    layout_->addWidget(btnOK, 5, 0, 1, 1);

    QPushButton* btnCancel = new QPushButton(this);
    btnCancel->setText(tr("&Cancel"));
    layout_->addWidget(btnCancel, 5, 1, 1, 1);

    // Connect the button signals to accept() and reject().
    QObject::connect(btnOK, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(typeCombo_, SIGNAL(currentIndexChanged(QString const&)), this, SLOT(onTypeChange(QString const&)));

    // Fill the fields with the data from the column, if given.
    if (column_ != 0)
    {
        setWindowTitle(tr("Edit Column"));
        nameEdit_->setText(column_->getName());
        setAllowedItems(column_->getColumnDesc().getAllowedItems());

        switch (column_->getContentType())
        {
        case COLUMN_CONTENT_IO:
            {
                typeCombo_->setCurrentIndex(typeCombo_->findText("IO"));
                break;
            }

        case COLUMN_CONTENT_BUSES:
            {
                typeCombo_->setCurrentIndex(typeCombo_->findText("Buses"));
                break;
            }

        case COLUMN_CONTENT_COMPONENTS:
            {
                typeCombo_->setCurrentIndex(typeCombo_->findText("Components"));
                break;
            }

        case COLUMN_CONTENT_CUSTOM:
            {
                typeCombo_->setCurrentIndex(typeCombo_->findText("Custom"));
                break;
            }

        default:
            {
                Q_ASSERT(false);
                break;
            }
        }
    }
    else
    {
        setWindowTitle(tr("Add Column"));
        setAllowedItems(CIT_NONE);
        typeCombo_->setCurrentIndex(0);
    }

    setFixedSize(sizeHint());
}

//-----------------------------------------------------------------------------
// Function: ~ColumnEditDialog()
//-----------------------------------------------------------------------------
ColumnEditDialog::~ColumnEditDialog()
{
}

//-----------------------------------------------------------------------------
// Function: ColumnEditDialog::hideContentSettings()
//-----------------------------------------------------------------------------
void ColumnEditDialog::hideContentSettings()
{
    typeLabel_->setVisible(false);
    typeCombo_->setVisible(false);
    allowedItemsGroup_->setVisible(false);

    layout_->activate();
    setFixedSize(sizeHint());
}

//-----------------------------------------------------------------------------
// Function: onTypeChange()
//-----------------------------------------------------------------------------
void ColumnEditDialog::onTypeChange(QString const& type)
{
    // Convert the index to a column content type.
    ColumnContentType contentType = COLUMN_CONTENT_CUSTOM;

    if (type == "Components")
    {
        contentType = COLUMN_CONTENT_COMPONENTS;
    }
    else if (type == "Buses")
    {
        contentType = COLUMN_CONTENT_BUSES;
    }
    else if (type == "IO")
    {
        contentType = COLUMN_CONTENT_IO;
    }
    
    // Set enabled/disabled based on the content type.
    for (int i = 0; i < CIT_COUNT; ++i)
    {
        itemCheckBoxes_[i]->setEnabled(contentType == COLUMN_CONTENT_CUSTOM);
    }

    switch (contentType)
    {
    case COLUMN_CONTENT_IO:
        {
            setAllowedItems(CIT_INTERFACE);
            break;
        }

    case COLUMN_CONTENT_BUSES:
        {
            setAllowedItems(CIT_CHANNEL | CIT_BRIDGE);
            break;
        }

    case COLUMN_CONTENT_COMPONENTS:
        {
            setAllowedItems(CIT_COMPONENT);
            break;
        }

    default:
        {
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: onTypeChange()
//-----------------------------------------------------------------------------
void ColumnEditDialog::setAllowedItems(unsigned int allowedItems)
{
    for (int i = 0; i < CIT_COUNT; ++i)
    {
        // Set the checked states based on the corresponding flag.
        itemCheckBoxes_[i]->setChecked((1 << i) & allowedItems);
    }
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString ColumnEditDialog::getName() const
{
    return nameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: getContentType()
//-----------------------------------------------------------------------------
ColumnContentType ColumnEditDialog::getContentType() const
{
    // Convert the index to a column content type.
    ColumnContentType contentType = COLUMN_CONTENT_CUSTOM;
    QString type = typeCombo_->currentText();

    if (type == "Components")
    {
        contentType = COLUMN_CONTENT_COMPONENTS;
    }
    else if (type == "Buses")
    {
        contentType = COLUMN_CONTENT_BUSES;
    }
    else if (type == "IO")
    {
        contentType = COLUMN_CONTENT_IO;
    }

    return contentType;
}

//-----------------------------------------------------------------------------
// Function: getAllowedItems()
//-----------------------------------------------------------------------------
unsigned int ColumnEditDialog::getAllowedItems() const
{
    unsigned int allowedItems = CIT_NONE;

    for (int i = 0; i < CIT_COUNT; ++i)
    {
        allowedItems |= (itemCheckBoxes_[i]->isChecked() << i);
    }

    return allowedItems;
}

//-----------------------------------------------------------------------------
// Function: accept()
//-----------------------------------------------------------------------------
void ColumnEditDialog::accept()
{
    if (column_ != 0)
    {
        if (!column_->isAllowedItemsValid(getAllowedItems()))
        {
            QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
                               tr("The column contains items that would not be "
                                  "allowed in the new column configuration. Please "
                                  "delete the items of the unallowed type before "
                                  "changing the column configuration."),
                               QMessageBox::Ok, (QWidget*)parent());
            msgBox.exec();
            return;
        }
    }

    QDialog::accept();
}
