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

#include <library/LibraryManager/libraryitem.h>

#include <QGridLayout>
#include <QLabel>
#include <QRegExp>
#include <QRegExpValidator>
#include <QStringList>
#include <QFormLayout>
#include <QGroupBox>
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

	// set validator to accept only valid names and special characters '?' and '*'
	QRegExp regExp(QString("[a-zA-Z0-9:_\\-\\.\\*\\?]*"), Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
	QRegExpValidator* validator = new QRegExpValidator(regExp, this);
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
    foreach (LibraryItem* item, vendors_) 
    {
        vendorList.append(item->getName());
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

	QString vendor = vendorText;

	// if the last character is not '?' then append '*' to the string
	if (!vendor.endsWith(QString("?"), Qt::CaseInsensitive))
		vendor += QString("*");

	QRegExp regExp(vendor, Qt::CaseInsensitive, QRegExp::Wildcard);
	QRegExpValidator validator(regExp, this);

	// get the library items from the vendors that matched 
	int pos = 0;
	foreach (LibraryItem* item, vendors_)
    {
		QString name = item->getName();
		if (validator.validate(name, pos) == QValidator::Acceptable)
        {
			libraries_ += item->getLibraries();
		}
	}

	// add the library names to the combo box
	QStringList libraryList;
    foreach (LibraryItem* item, libraries_)
    {
        libraryList.append(item->getName());
    }
    libraryList.removeDuplicates();

	libraryBox_.addItem(QString());
	libraryBox_.addItems(libraryList);
	libraryBox_.setEditText(libraryText);

	connect(&libraryBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onLibraryChanged(const QString&)), Qt::UniqueConnection);

	emit vendorChanged(vendor);
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

	QString library = libraryText;

	// if the last character is not '?' then append '*' to the string
	if (!library.endsWith(QString("?"), Qt::CaseInsensitive))
    {
		library += QString("*");
    }

	QRegExp regExp(library, Qt::CaseInsensitive, QRegExp::Wildcard);
	QRegExpValidator validator(regExp, this);

    int pos = 0;
    foreach (LibraryItem* item, libraries_)
    {
        QString name = item->getName();
        if (validator.validate(name, pos) == QValidator::Acceptable)
        {
            names_ += item->getNames();
        }
    }
	
	// add the names to the combo box
	QStringList nameList;
    foreach (LibraryItem* item, names_)
    {
        nameList.append(item->getName());
    }
    nameList.removeDuplicates();

	nameBox_.addItem(QString());
	nameBox_.addItems(nameList);
	nameBox_.setEditText(nameText);

	connect(&nameBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);

	emit libraryChanged(library);
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

	QString name = nameText;

	// if the last character is not '?' then append '*' to the string
	if (!name.endsWith(QString("?"), Qt::CaseInsensitive))
    {
		name += QString("*");
    }

	QRegExp regExp(name, Qt::CaseInsensitive, QRegExp::Wildcard);
	QRegExpValidator validator(regExp, this);

	int pos = 0;
	foreach (LibraryItem* item, names_)
    {
        QString name = item->getName();
        if (validator.validate(name, pos) == QValidator::Acceptable)
        {
            versions_ += item->getVersions();
        }
    }

	// add the names to the combo box
	QStringList versionList;
    foreach (LibraryItem* item, versions_)
    {
        versionList.append(item->getName());
    }
    versionList.removeDuplicates();

	versionBox_.addItem(QString());
	versionBox_.addItems(versionList);
	versionBox_.setEditText(versionText);
	
	connect(&versionBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onVersionChanged(const QString&)), Qt::UniqueConnection);

	emit nameChanged(name);
	onVersionChanged(versionText);
}

//-----------------------------------------------------------------------------
// Function: DialerWidget::onVersionChanged()
//-----------------------------------------------------------------------------
void DialerWidget::onVersionChanged(QString const& versionText)
{
	Q_ASSERT(root_);

	QString version = versionText;

	// if the last character is not '?' then append '*' to the string
	if (!version.endsWith(QString("?"), Qt::CaseInsensitive))
    {
		version += QString("*");
    }

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

    QGroupBox* filtersGroup = new QGroupBox(tr("Library Filters"), this);

    QFormLayout* layout = new QFormLayout(filtersGroup);
    layout->addRow(tr("Vendor"), &vendorBox_);
    layout->addRow(tr("Library"), &libraryBox_);
    layout->addRow(tr("Name"), &nameBox_);
    layout->addRow(tr("Version"), &versionBox_);

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
