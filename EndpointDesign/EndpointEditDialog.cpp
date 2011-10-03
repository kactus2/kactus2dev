//-----------------------------------------------------------------------------
// File: EndpointEditDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.3.2011
//
// Description:
// Endpoint editor widget.
//-----------------------------------------------------------------------------

#include "EndpointEditDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QCoreApplication>

#include "../EndpointDesign/EndpointItem.h"

//-----------------------------------------------------------------------------
// Function: EndpointEditDialog()
//-----------------------------------------------------------------------------
EndpointEditDialog::EndpointEditDialog(QWidget* parent,
                                       EndpointItem const* endpoint) : QDialog(parent), nameEdit_(0),
                                                                       typeCombo_(0), btnOK_(0)
{
    QGridLayout* layout = new QGridLayout(this);

    // Create the name label and field.
    QLabel* nameLabel = new QLabel(tr("Name:"), this);
    layout->addWidget(nameLabel, 0, 0, 1, 1);

    nameEdit_ = new QLineEdit(this);
    layout->addWidget(nameEdit_, 0, 1, 1, 1);

    // Create the type label and field.
    QLabel* typeLabel = new QLabel(tr("Type:"), this);
    layout->addWidget(typeLabel, 1, 0, 1, 1);

    typeCombo_ = new QComboBox(this);
    typeCombo_->addItem("In");
    typeCombo_->addItem("Out");
    layout->addWidget(typeCombo_, 1, 1, 1, 1);

    // Create the connection label and field.
    QLabel* connLabel = new QLabel(tr("Connection:"), this);
    layout->addWidget(connLabel, 2, 0, 1, 1);

    connCombo_ = new QComboBox(this);
    connCombo_->addItem("Message");
    connCombo_->addItem("Packet");
    connCombo_->addItem("Scalar");
    layout->addWidget(connCombo_, 2, 1, 1, 1);

    // Create the dialog buttons.
    QGridLayout* btnLayout = new QGridLayout();

    btnOK_ = new QPushButton(this);
    btnOK_->setText("&OK");
    btnLayout->addWidget(btnOK_, 0, 0, 1, 1);

    QPushButton* btnCancel = new QPushButton(this);
    btnCancel->setText("&Cancel");
    btnLayout->addWidget(btnCancel, 0, 1, 1, 1);

    layout->addLayout(btnLayout, 3, 0, 1, 2);

    // Connect the button signals to accept() and reject().
    QObject::connect(btnOK_, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

    // Set the data to the field if an existing endpoint was given.
    if (endpoint != 0)
    {
        nameEdit_->setText(endpoint->getName());
        typeCombo_->setCurrentIndex(endpoint->getMCAPIDirection());
        connCombo_->setCurrentIndex(endpoint->getConnectionType());
        
        // Disallow the endpoint type and connection type to be changed if there is already a connection.
        if (endpoint->isConnected())
        {
            typeCombo_->setEnabled(false);
            connCombo_->setEnabled(false);
        }

        // Also set the dialog title.
        setWindowTitle(tr("Edit Endpoint"));
    }
    else
    {
        btnOK_->setEnabled(false);
        setWindowTitle(tr("Add Endpoint"));
    }

    connect(nameEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(onContentChanged()));

    setFixedSize(sizeHint());
}

//-----------------------------------------------------------------------------
// Function: ~EndpointEditDialog()
//-----------------------------------------------------------------------------
EndpointEditDialog::~EndpointEditDialog()
{
}

//-----------------------------------------------------------------------------
// Function: getName()
//----------------------------------------------------------------------------- 
QString EndpointEditDialog::getName() const
{
    return nameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: getType()
//-----------------------------------------------------------------------------
MCAPIEndpointDirection EndpointEditDialog::getType() const
{
    return static_cast<MCAPIEndpointDirection>(typeCombo_->currentIndex());
}

//-----------------------------------------------------------------------------
// Function: getConnectionType()
//-----------------------------------------------------------------------------
MCAPIDataType EndpointEditDialog::getConnectionType() const
{
    return static_cast<MCAPIDataType>(connCombo_->currentIndex());
}

//-----------------------------------------------------------------------------
// Function: accept()
//-----------------------------------------------------------------------------
void EndpointEditDialog::accept()
{
    // Validate the name.
    if (!nameEdit_->text().contains(QRegExp("^[a-z|A-Z|_][a-z|A-Z|0-9|_]*$")))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("The name '%1' is invalid because it is not a "
                              "valid C variable name.").arg(nameEdit_->text()),
                           QMessageBox::Ok, this);
        msgBox.setInformativeText("Please enter a name containing only characters A-Z, a-z, 0-9 or _ "
                                  "and starting with A-Z or a-z.");
        msgBox.exec();
        return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: onContentChanged()
//-----------------------------------------------------------------------------
void EndpointEditDialog::onContentChanged()
{
    btnOK_->setEnabled(!nameEdit_->text().isEmpty());
}
