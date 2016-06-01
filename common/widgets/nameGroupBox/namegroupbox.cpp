//-----------------------------------------------------------------------------
// File: NameGroupBox.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 16.01.2011
//
// Description:
// This class provides 3 GUI items to set a name, displayName and description for an element in IP-Xact.
//-----------------------------------------------------------------------------

#include "namegroupbox.h"

#include <IPXACTmodels/validators/namevalidator.h>

#include <QSizePolicy>
#include <QLabel>
#include <QGridLayout>
#include <QValidator>
#include <QRegExpValidator>
#include <QRegExp>

//-----------------------------------------------------------------------------
// Function: namegroupbox::NameGroupBox()
//-----------------------------------------------------------------------------
NameGroupBox::NameGroupBox(QWidget* parent, const QString& title): 
QGroupBox(title, parent),
nameEdit_(0),
displayNameEdit_(0),
descriptionEdit_(0)
{
	// set the size policy for the widget
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

	// set the maximum height for the widget
    setMaximumHeight(130);

	// the layout manager for this widget
	QGridLayout* layout = new QGridLayout(this);

	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	layout->addWidget(nameLabel, 0, 0, 1, 1);

	nameEdit_ = new QLineEdit(this);
	nameEdit_->setToolTip(tr("Set the name for the element"));
	nameEdit_->setProperty("mandatoryField", true);

	NameValidator* validator = new NameValidator(nameEdit_);
	nameEdit_->setValidator(validator);
	layout->addWidget(nameEdit_, 0, 1, 1, 1);

	QLabel* displayLabel = new QLabel(tr("Display Name:"), this);
	layout->addWidget(displayLabel, 1, 0, 1, 1);

	displayNameEdit_ = new QLineEdit(this);
	displayNameEdit_->setToolTip(tr("Set the display name for the element"));
	layout->addWidget(displayNameEdit_, 1, 1, 1, 1);

	QLabel* descriptionLabel = new QLabel(tr("Description:"), this);
    layout->addWidget(descriptionLabel, 2, 0, 1, 1, Qt::AlignTop);

	descriptionEdit_ = new QPlainTextEdit(this);
	descriptionEdit_->setToolTip(tr("Set the description for the element"));
    layout->addWidget(descriptionEdit_, 2, 1, 1, 1);

	// connect the signals that inform of changes made to items
    connect(nameEdit_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(nameEdit_, SIGNAL(editingFinished()), this, SIGNAL(nameChanged()), Qt::UniqueConnection);

    connect(displayNameEdit_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(displayNameEdit_, SIGNAL(editingFinished()), this, SIGNAL(displayNameChanged()), Qt::UniqueConnection);

    connect(descriptionEdit_, SIGNAL(textChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(descriptionEdit_, SIGNAL(textChanged()), this, SIGNAL(descriptionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: namegroupbox::~NameGroupBox()
//-----------------------------------------------------------------------------
NameGroupBox::~NameGroupBox()
{

}

//-----------------------------------------------------------------------------
// Function: namegroupbox::name()
//-----------------------------------------------------------------------------
QString NameGroupBox::name() const
{
	return nameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: namegroupbox::displayName()
//-----------------------------------------------------------------------------
QString NameGroupBox::displayName() const
{
	return displayNameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: namegroupbox::description()
//-----------------------------------------------------------------------------
QString NameGroupBox::description() const
{
	return descriptionEdit_->toPlainText();
}

//-----------------------------------------------------------------------------
// Function: namegroupbox::isValid()
//-----------------------------------------------------------------------------
bool NameGroupBox::isValid() const
{
	return !nameEdit_->text().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: namegroupbox::setName()
//-----------------------------------------------------------------------------
void NameGroupBox::setName( const QString& name )
{
	nameEdit_->setText(name);
}

//-----------------------------------------------------------------------------
// Function: namegroupbox::setDisplayName()
//-----------------------------------------------------------------------------
void NameGroupBox::setDisplayName( const QString& displayName )
{
	displayNameEdit_->setText(displayName);
}

//-----------------------------------------------------------------------------
// Function: namegroupbox::setDescription()
//-----------------------------------------------------------------------------
void NameGroupBox::setDescription( const QString& description )
{
	descriptionEdit_->setPlainText(description);
}
