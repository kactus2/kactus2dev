//-----------------------------------------------------------------------------
// File: dialerwidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// Contains the combo boxes to set search settings for vlnv-fields.
//-----------------------------------------------------------------------------

#include "dialerwidget.h"

#include <KactusAPI/include/LibraryItem.h>

#include <QCompleter>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStringList>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: DialerWidget::DialerWidget()
//-----------------------------------------------------------------------------
DialerWidget::DialerWidget(QWidget *parent):
QWidget(parent),
vendorBox_(this),
libraryBox_(this),
nameBox_(this),
versionBox_(this),
root_(0),
vendors_(),
libraries_(),
names_(),
versions_()
{
	vendorBox_.setEditable(true);
	libraryBox_.setEditable(true);
	nameBox_.setEditable(true);
	versionBox_.setEditable(true);

    vendorBox_.completer()->setCompletionMode(QCompleter::PopupCompletion);
    libraryBox_.completer()->setCompletionMode(QCompleter::PopupCompletion);
    nameBox_.completer()->setCompletionMode(QCompleter::PopupCompletion);
    versionBox_.completer()->setCompletionMode(QCompleter::PopupCompletion);

	// set validator to accept only valid names and special characters '?' and '*'
	QRegularExpression regExp(QString("[a-zA-Z0-9:_\\-\\.\\*\\?]*"), QRegularExpression::CaseInsensitiveOption);
	QRegularExpressionValidator* validator = new QRegularExpressionValidator(regExp, this);
	vendorBox_.setValidator(validator);
	libraryBox_.setValidator(validator);
	nameBox_.setValidator(validator);
	versionBox_.setValidator(validator);

    setupLayout();

	connectBoxes();
}

//-----------------------------------------------------------------------------
// Function: DialerWidget::~DialerWidget()
//-----------------------------------------------------------------------------
DialerWidget::~DialerWidget()
{
}

//-----------------------------------------------------------------------------
// Function: DialerWidget::setRootItem()
//-----------------------------------------------------------------------------
void DialerWidget::setRootItem(const LibraryItem* rootItem)
{
	root_ = rootItem;
	refreshVendors();
}

//-----------------------------------------------------------------------------
// Function: DialerWidget::refreshVendors()
//-----------------------------------------------------------------------------
void DialerWidget::refreshVendors()
{
	Q_ASSERT(root_);

	// get currently selected vendor search condition
	const QString vendorText = vendorBox_.currentText();

	vendorBox_.disconnect();
	vendorBox_.clear();
	vendors_.clear();

	vendors_ = root_->getVendors();

    QStringList vendorList;
    for (LibraryItem const* item : vendors_) 
    {
        vendorList.append(item->name());
    }
    vendorList.removeDuplicates();

	vendorBox_.addItem(QString());
	vendorBox_.addItems(vendorList);
	vendorBox_.setEditText(vendorText);

	connect(&vendorBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onVendorChanged(const QString&)), Qt::UniqueConnection);

	onVendorChanged(vendorText);
}

//-----------------------------------------------------------------------------
// Function: DialerWidget::onVendorChanged()
//-----------------------------------------------------------------------------
void DialerWidget::onVendorChanged(QString const& vendorText)
{
	Q_ASSERT(root_);

	// get currently selected library search condition
	const QString libraryText = libraryBox_.currentText();

	libraryBox_.disconnect();
	libraryBox_.clear();
	libraries_.clear();

	QString vendorExp = vendorText + ".*";

	QRegularExpression regExp(vendorExp, QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
	QRegularExpressionValidator validator(regExp, this);

	// get the library items from the vendors that matched 
	int pos = 0;
	for (LibraryItem const* item : vendors_)
    {
		QString vendor = item->name();
		if (validator.validate(vendor, pos) == QValidator::Acceptable)
        {
			libraries_ += item->getLibraries();
		}
	}

	// add the library names to the combo box
	QStringList libraryList;
    for (LibraryItem const* item : libraries_)
    {
        libraryList.append(item->name());
    }
    libraryList.removeDuplicates();

	libraryBox_.addItem(QString());
	libraryBox_.addItems(libraryList);
	libraryBox_.setEditText(libraryText);

	connect(&libraryBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onLibraryChanged(const QString&)), Qt::UniqueConnection);

	emit vendorChanged(vendorExp);
	onLibraryChanged(libraryText);
}

//-----------------------------------------------------------------------------
// Function: DialerWidget::onLibraryChanged()
//-----------------------------------------------------------------------------
void DialerWidget::onLibraryChanged(QString const& libraryText)
{
	Q_ASSERT(root_);

	// get currently selected name search condition
	const QString nameText = nameBox_.currentText();

	nameBox_.disconnect();
	nameBox_.clear();
	names_.clear();

	QString libraryExp = libraryText + ".*";

	QRegularExpression regExp(libraryExp, QRegularExpression::CaseInsensitiveOption);
	QRegularExpressionValidator validator(regExp, this);

    int pos = 0;
    for (LibraryItem const* item : libraries_)
    {
        QString library = item->name();
        if (validator.validate(library, pos) == QValidator::Acceptable)
        {
            names_ += item->getNames();
        }
    }
	
	// add the names to the combo box
	QStringList nameList;
    for (LibraryItem const* item : names_)
    {
        nameList.append(item->name());
    }
    nameList.removeDuplicates();

	nameBox_.addItem(QString());
	nameBox_.addItems(nameList);
	nameBox_.setEditText(nameText);

	connect(&nameBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);

	emit libraryChanged(libraryExp);
	onNameChanged(nameText);
}

//-----------------------------------------------------------------------------
// Function: DialerWidget::onNameChanged()
//-----------------------------------------------------------------------------
void DialerWidget::onNameChanged(QString const& nameText)
{
	Q_ASSERT(root_);

	// get currently selected version condition
	const QString versionText = versionBox_.currentText();

	versionBox_.disconnect();
	versionBox_.clear();
	versions_.clear();

	QString nameExp = nameText + ".*";

	QRegularExpression regExp(nameExp, QRegularExpression::CaseInsensitiveOption);
	QRegularExpressionValidator validator(regExp, this);

	int pos = 0;
	for (LibraryItem const* item : names_)
    {
        QString name = item->name();
        if (validator.validate(name, pos) == QValidator::Acceptable)
        {
            versions_ += item->getVersions();
        }
    }

	// add the names to the combo box
	QStringList versionList;
    for (LibraryItem const* item : versions_)
    {
        versionList.append(item->name());
    }
    versionList.removeDuplicates();

	versionBox_.addItem(QString());
	versionBox_.addItems(versionList);
	versionBox_.setEditText(versionText);
	
	connect(&versionBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onVersionChanged(const QString&)), Qt::UniqueConnection);

	emit nameChanged(nameExp);
	onVersionChanged(versionText);
}

//-----------------------------------------------------------------------------
// Function: DialerWidget::onVersionChanged()
//-----------------------------------------------------------------------------
void DialerWidget::onVersionChanged(QString const& versionText)
{
	Q_ASSERT(root_);

	QString version = versionText + ".*";

	emit versionChanged(version);
}

//-----------------------------------------------------------------------------
// Function: DialerWidget::setupLayout()
//-----------------------------------------------------------------------------
void DialerWidget::setupLayout()
{
    vendorBox_.setMaxVisibleItems(25);
    vendorBox_.setMinimumWidth(80);

    libraryBox_.setMaxVisibleItems(25);
    libraryBox_.setMinimumWidth(80);

    nameBox_.setMaxVisibleItems(25);
    nameBox_.setMinimumWidth(80);

    versionBox_.setMaxVisibleItems(25);
    versionBox_.setMinimumWidth(80);

    QVBoxLayout* topLayout = new QVBoxLayout(this);

    QGroupBox* filtersGroup = new QGroupBox(tr("VLNV"), this);

    QFormLayout* layout = new QFormLayout(filtersGroup);
    layout->addRow(tr("Vendor:"), &vendorBox_);
    layout->addRow(tr("Library:"), &libraryBox_);
    layout->addRow(tr("Name:"), &nameBox_);
    layout->addRow(tr("Version:"), &versionBox_);

    layout->setSpacing(2);
    layout->setContentsMargins(4, 2, 4, 2);

    topLayout->setContentsMargins(0, 0, 0, 0);

    topLayout->addWidget(filtersGroup);
}

//-----------------------------------------------------------------------------
// Function: DialerWidget::connectBoxes()
//-----------------------------------------------------------------------------
void DialerWidget::connectBoxes()
{
    connect(&vendorBox_, SIGNAL(editTextChanged(const QString&)),
        this, SLOT(onVendorChanged(const QString&)), Qt::UniqueConnection);
    connect(&libraryBox_, SIGNAL(editTextChanged(const QString&)),
        this, SLOT(onLibraryChanged(const QString&)), Qt::UniqueConnection);
    connect(&nameBox_, SIGNAL(editTextChanged(const QString&)),
        this, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);
    connect(&versionBox_, SIGNAL(editTextChanged(const QString&)),
        this, SLOT(onVersionChanged(const QString&)), Qt::UniqueConnection);
}
