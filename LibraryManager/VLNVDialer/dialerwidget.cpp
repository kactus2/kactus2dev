/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: dialerwidget.cpp
 *		Project: Kactus 2
 */

#include "dialerwidget.h"

#include <LibraryManager/libraryitem.h>

#include <QGridLayout>
#include <QLabel>
#include <QRegExp>
#include <QRegExpValidator>
#include <QStringList>

DialerWidget::DialerWidget(QWidget *parent):
QWidget(parent),
vendorBox_(this),
libraryBox_(this),
nameBox_(this),
versionBox_(this),
root_(NULL),
vendors_(),
libraries_(),
names_(),
versions_() {

	vendorBox_.setEditable(true);
	vendorBox_.setMaxVisibleItems(25);
	vendorBox_.setMinimumWidth(80);

	libraryBox_.setEditable(true);
	libraryBox_.setMaxVisibleItems(25);
	libraryBox_.setMinimumWidth(80);

	nameBox_.setEditable(true);
	nameBox_.setMaxVisibleItems(25);
	nameBox_.setMinimumWidth(80);

	versionBox_.setEditable(true);
	versionBox_.setMaxVisibleItems(25);
	versionBox_.setMinimumWidth(80);

	// set validator to accept only valid names and special characters '?' and '*'
	QRegExp regExp(QString("[a-zA-Z0-9:_-\.\*\?]*"), Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
	QRegExpValidator* validator = new QRegExpValidator(regExp, this);
	vendorBox_.setValidator(validator);
	libraryBox_.setValidator(validator);
	nameBox_.setValidator(validator);
	versionBox_.setValidator(validator);

	QLabel* vendorLabel = new QLabel(tr("Vendor"), this);
	QLabel* libraryLabel = new QLabel(tr("Library"), this);
	QLabel* nameLabel = new QLabel(tr("Name"), this);
	QLabel* versionLabel = new QLabel(tr("Version"), this);

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(vendorLabel, 0, 0, 1, 1);
	layout->addWidget(libraryLabel, 0, 1, 1, 1);
	layout->addWidget(nameLabel, 0, 2, 1, 1);
	layout->addWidget(versionLabel, 0, 3, 1, 1);
	layout->addWidget(&vendorBox_, 1, 0, 1, 1);
	layout->addWidget(&libraryBox_, 1, 1, 1, 1);
	layout->addWidget(&nameBox_, 1, 2, 1, 1);
	layout->addWidget(&versionBox_, 1, 3, 1, 1);
	layout->setSpacing(0);

	connectBoxes();
}

DialerWidget::~DialerWidget() {
}

void DialerWidget::setRootItem(const LibraryItem* rootItem ) {
	root_ = rootItem;
	refreshVendors();
}

void DialerWidget::refreshVendors() {
	Q_ASSERT(root_);

	vendorBox_.disconnect();
	vendorBox_.clear();
	vendors_.clear();

	vendors_ = root_->getVendors();
	QStringList vendorList;
	foreach (LibraryItem* item, vendors_) {
		if (!vendorList.contains(item->getName()))
			vendorList.append(item->getName());
	}
	vendorBox_.addItem(QString());
	vendorBox_.addItems(vendorList);

	connect(&vendorBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onVendorChanged(const QString&)), Qt::UniqueConnection);

	onVendorChanged(QString());
}

void DialerWidget::onVendorChanged( const QString& vendorText ) {
	Q_ASSERT(root_);

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
	foreach (LibraryItem* item, vendors_) {
		if (validator.validate(item->getName(), pos) == QValidator::Acceptable) {
			libraries_ += item->getLibraries();
		}
	}

	// add the library names to the combo box
	QStringList libraryList;
	foreach (LibraryItem* item, libraries_) {
		if (!libraryList.contains(item->getName()))
			libraryList.append(item->getName());
	}
	libraryBox_.addItem(QString());
	libraryBox_.addItems(libraryList);

	connect(&libraryBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onLibraryChanged(const QString&)), Qt::UniqueConnection);

	emit vendorChanged(vendor);
	onLibraryChanged(QString());
}

void DialerWidget::onLibraryChanged( const QString& libraryText ) {
	Q_ASSERT(root_);

	nameBox_.disconnect();
	nameBox_.clear();
	names_.clear();

	QString library = libraryText;

	// if the last character is not '?' then append '*' to the string
	if (!library.endsWith(QString("?"), Qt::CaseInsensitive))
		library += QString("*");

	QRegExp regExp(library, Qt::CaseInsensitive, QRegExp::Wildcard);
	QRegExpValidator validator(regExp, this);

	int pos = 0;
	foreach (LibraryItem* item, libraries_) {
		if (validator.validate(item->getName(), pos) == QValidator::Acceptable) {
			names_ += item->getNames();
		}
	}
	
	// add the names to the combo box
	QStringList nameList;
	foreach (LibraryItem* item, names_) {
		if (!nameList.contains(item->getName()))
			nameList.append(item->getName());
	}
	nameBox_.addItem(QString());
	nameBox_.addItems(nameList);

	connect(&nameBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);

	emit libraryChanged(library);
	onNameChanged(QString());
}

void DialerWidget::onNameChanged( const QString& nameText ) {
	Q_ASSERT(root_);

	versionBox_.disconnect();
	versionBox_.clear();
	versions_.clear();

	QString name = nameText;

	// if the last character is not '?' then append '*' to the string
	if (!name.endsWith(QString("?"), Qt::CaseInsensitive))
		name += QString("*");

	QRegExp regExp(name, Qt::CaseInsensitive, QRegExp::Wildcard);
	QRegExpValidator validator(regExp, this);

	int pos = 0;
	foreach (LibraryItem* item, names_) {
		if (validator.validate(item->getName(), pos) == QValidator::Acceptable) {
			versions_ += item->getVersions();
		}
	}

	// add the names to the combo box
	QStringList versionList;
	foreach (LibraryItem* item, versions_) {
		if (!versionList.contains(item->getName()))
			versionList.append(item->getName());
	}
	versionBox_.addItem(QString());
	versionBox_.addItems(versionList);
	
	connect(&versionBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onVersionChanged(const QString&)), Qt::UniqueConnection);

	emit nameChanged(name);
	onVersionChanged(QString());
}

void DialerWidget::onVersionChanged( const QString& versionText ) {
	Q_ASSERT(root_);

	QString version = versionText;

	// if the last character is not '?' then append '*' to the string
	if (!version.endsWith(QString("?"), Qt::CaseInsensitive))
		version += QString("*");

	emit versionChanged(version);
}

void DialerWidget::disconnectBoxes() {
	vendorBox_.disconnect();
	libraryBox_.disconnect();
	nameBox_.disconnect();
	versionBox_.disconnect();
}

void DialerWidget::connectBoxes() {
	connect(&vendorBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onVendorChanged(const QString&)), Qt::UniqueConnection);
	connect(&libraryBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onLibraryChanged(const QString&)), Qt::UniqueConnection);
	connect(&nameBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);
	connect(&versionBox_, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onVersionChanged(const QString&)), Qt::UniqueConnection);
}
