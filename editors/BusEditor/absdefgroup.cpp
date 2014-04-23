/* 
 *  	Created on: 15.8.2011
 *      Author: Antti Kamppi
 * 		filename: absdefgroup.cpp
 *		Project: Kactus 2
 */

#include "absdefgroup.h"
#include <library/LibraryManager/libraryinterface.h>

#include "busportsdelegate.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QModelIndexList>
#include <QCoreApplication>
#include <QFile>

AbsDefGroup::AbsDefGroup(LibraryInterface* handler, QWidget *parent): 
QGroupBox(tr("Signals (Abstraction Definition)"), parent),
portView_(this),
portModel_(this),
handler_(handler),
absDef_() {

	portView_.setModel(&portModel_);
	portView_.setItemDelegate(new BusPortsDelegate(this));
    portView_.setAllowImportExport(true);

	connect(&portView_, SIGNAL(addSignalOptions()),
		this, SLOT(onAddSignalOptions()), Qt::UniqueConnection);
	
	connect(&portModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(portRenamed(const QString&, const QString&)), 
        this, SIGNAL(portRenamed(const QString&, const QString&)), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), 
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);

    connect(&portView_, SIGNAL(addItem(const QModelIndex&)),
        &portModel_, SLOT(addSignal()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(removeItem(const QModelIndex&)),
        &portModel_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	/*connect(&portView_, SIGNAL(removeItems(const QModelIndexList&)),
		&portModel_, SLOT(removeIndexes(const QModelIndexList&)), Qt::UniqueConnection);
	connect(&portView_, SIGNAL(copyItems(const QModelIndexList&)),
		&portModel_, SLOT(copyIndexes(const QModelIndexList&)), Qt::UniqueConnection);		*/

	setupLayout();
}

AbsDefGroup::~AbsDefGroup() {
}

void AbsDefGroup::onImport() {

	QString homePath;

	// if abs def is set then use its location in library
	if (absDef_) {
		const QString busPath = handler_->getDirectoryPath(*absDef_->getVlnv());
		homePath = QString("%1/logicalSignals.csv").arg(busPath);
	}
	// if there is no abs def then use default library location
	else {
		QSettings settings;
		homePath = settings.value(QString("Library/DefaultLocation"), 
			QCoreApplication::applicationDirPath()).toString();
	}

	QString path = QFileDialog::getOpenFileName(this,  tr("csv-files (*.csv)"), homePath);

	// if user clicked cancel
	if (path.isEmpty()) {
		return;
	}

	portModel_.importCSV(path);
}

void AbsDefGroup::onExport() {
	QString homePath;

	// if abs def is set then use its location in library
	if (absDef_) {
		const QString busPath = handler_->getDirectoryPath(*absDef_->getVlnv());
		homePath = QString("%1/logicalSignals.csv").arg(busPath);
	}
	// if there is no abs def then use default library location
	else {
		QSettings settings;
		homePath = settings.value(QString("Library/DefaultLocation"), 
			QCoreApplication::applicationDirPath()).toString();
	}

	QString path = QFileDialog::getSaveFileName(this, tr("Save a csv-file"), 
		homePath, tr("csv-files (*.csv)"));

	// if user clicked cancel
	if (path.isEmpty())
		return;

	portModel_.exportCSV(path);
}

void AbsDefGroup::onAddSignalOptions() {
	portModel_.addSignalOptions(portView_.selected());
}

void AbsDefGroup::save() {
	// tell port model to transfer the ports to the abstraction definition
	portModel_.save();
}

void AbsDefGroup::setupLayout() {

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&portView_, 1);
}

void AbsDefGroup::setAbsDef(QSharedPointer<AbstractionDefinition> absDef) {
	absDef_ = absDef;
	portModel_.setAbsDef(absDef);
}
