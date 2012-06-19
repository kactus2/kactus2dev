/* 
 *  Created on: 20.6.2011
 *      Author: Antti Kamppi
 * 		filename: buseditor.cpp
 */

#include "buseditor.h"

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <LibraryManager/vlnv.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QModelIndexList>
#include <QCoreApplication>
#include <QFile>

BusEditor::BusEditor( QWidget *parent, 
					 LibraryInterface* libHandler,
					 QSharedPointer<BusDefinition> busDef, 
					 QSharedPointer<AbstractionDefinition> absDef,
					 bool disableBusDef):
TabDocument(parent, DOC_PROTECTION_SUPPORT), 
libHandler_(libHandler),
busDef_(busDef),
absDef_(absDef),
editableBusDef_(),
editableAbsDef_(),
busDefGroup_(this),
absDefGroup_(this) {

	Q_ASSERT_X(busDef, "BusEditor constructor",
		"Null Bus Definition pointer given as parameter");

	// make copies of the bus definition and abstraction definition
	editableBusDef_ = QSharedPointer<BusDefinition>(new BusDefinition(*busDef.data()));

	// if abstraction definition is being edited
	if (absDef) {
		editableAbsDef_ = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition(*absDef.data()));
		absDefGroup_.setAbsDef(editableAbsDef_);
	}
	// if abstraction definition is not being edited.
	else {
		absDefGroup_.setDisabled(true);
	}

	busDefGroup_.setBusDef(editableBusDef_);
	busDefGroup_.setDisabled(disableBusDef);

	setupLayout();

	connect(&busDefGroup_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&absDefGroup_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&absDefGroup_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&absDefGroup_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    VLNV const* vlnv = busDef_->getVlnv();
    setDocumentName(vlnv->getName() + " (" + vlnv->getVersion() + ")");
    setDocumentType(tr("Bus"));

    // Open in unlocked mode by default only if the version is draft.
    setProtection(vlnv->getVersion() != "draft");
}

BusEditor::~BusEditor() {
}

//-----------------------------------------------------------------------------
// Function: BusEditor::validate()
//-----------------------------------------------------------------------------
bool BusEditor::validate(QStringList& errorList)
{
    bool valid = true;

    // if abstraction definition is being edited
    if (absDefGroup_.isEnabled())
    {
        // save the changes from the model to the abstraction definition
        absDefGroup_.save();

        valid = editableAbsDef_->isValid(errorList);
    }

    // if bus definition is being edited
    if (busDefGroup_.isEnabled())
    {
        valid = editableBusDef_->isValid(errorList) && valid;
    }

    return valid;
}

bool BusEditor::save() {

	// if abstraction definition is being edited
	if (absDefGroup_.isEnabled()) {
		// save the changes from the model to the abstraction definition
		absDefGroup_.save();

		*absDef_ = *editableAbsDef_;
		libHandler_->writeModelToFile(absDef_);
	}

	// if bus definition is being edited
	if (busDefGroup_.isEnabled()) {
		// copy the settings from the temporary items to the original
		*busDef_ = *editableBusDef_;

		libHandler_->writeModelToFile(busDef_);
	}

	return TabDocument::save();
}

bool BusEditor::saveAs() {

    // Ask the user for a new VLNV along with the directory.
    VLNV vlnv;
    
	VLNV busDefVLNV;
	QString busDirectory;

	VLNV absDefVLNV;
	QString absDirectory;

	// if bus definition is being edited
	if (busDefGroup_.isEnabled()) {
		if (!NewObjectDialog::saveAsDialog(this, libHandler_, *busDef_->getVlnv(), vlnv, busDirectory))	{
			return false;
		}

		// copy the settings from the temporary items to the original
		busDef_ = QSharedPointer<BusDefinition>(new BusDefinition(*editableBusDef_.data()));
		editableBusDef_ = QSharedPointer<BusDefinition>(new BusDefinition(*busDef_.data()));

		busDefVLNV = VLNV(VLNV::BUSDEFINITION, vlnv.getVendor(), vlnv.getLibrary(),
			vlnv.getName(), vlnv.getVersion());

		busDef_->setVlnv(busDefVLNV);
		editableBusDef_->setVlnv(busDefVLNV);
	}

	// if abstraction definition is being edited but not the bus definition
	else if (absDef_ && !busDefGroup_.isEnabled()) {
		if (!NewObjectDialog::saveAsDialog(this, libHandler_, *absDef_->getVlnv(), vlnv, absDirectory))	{
			return false;
		}

		absDefGroup_.save();

		absDef_ = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition(*editableAbsDef_.data()));
		editableAbsDef_ = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition(*absDef_.data()));

		absDefVLNV = VLNV(VLNV::ABSTRACTIONDEFINITION, vlnv.getVendor(), vlnv.getLibrary(),
			vlnv.getName(), vlnv.getVersion());

		absDef_->setVlnv(absDefVLNV);
		editableAbsDef_->setVlnv(absDefVLNV);

		// write only the abs def
		libHandler_->writeModelToFile(absDirectory, absDef_);

		setDocumentName(absDefVLNV.getName() + " (" + absDefVLNV.getVersion() + ")");
		return TabDocument::saveAs();
	}
    
	// if both are being edited (now the bus definition vlnv has already been defined but not abs def)
	if (busDefGroup_.isEnabled() && absDef_) {

		// remove the possible .busDef from the end of the name field
		QString absDefName = busDefVLNV.getName();
		absDefName = absDefName.remove(".busDef", Qt::CaseInsensitive);

		absDefGroup_.save();

		absDef_ = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition(*editableAbsDef_.data()));
		editableAbsDef_ = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition(*absDef_.data()));

		// create automatically vlnv for the abstraction definition
		absDefVLNV = VLNV(VLNV::ABSTRACTIONDEFINITION, vlnv.getVendor(), vlnv.getLibrary(),
			absDefName + ".absDef", vlnv.getVersion());

		// by default both bus def and abs def are saved in same directory
		absDirectory = busDirectory;

		// if the automatic abs def vlnv is taken
		if (libHandler_->contains(absDefVLNV)) {
			VLNV newAbsDefVLNV;

			if (!NewObjectDialog::saveAsDialog(this, libHandler_, absDefVLNV, newAbsDefVLNV, absDirectory)) {
				return false;
			}
			// save the created abstraction definition vlnv
			absDefVLNV = newAbsDefVLNV;
		}

		// update the vlnvs of the abs def
		absDef_->setVlnv(absDefVLNV);
		editableAbsDef_->setVlnv(absDefVLNV);

		// update the abs def's reference to bus def
		absDef_->setBusType(busDefVLNV);
		editableAbsDef_->setBusType(busDefVLNV);

		// write the abs def and bus def
		libHandler_->writeModelToFile(busDirectory, busDef_);
		libHandler_->writeModelToFile(absDirectory, absDef_);

		setDocumentName(absDefVLNV.getName() + " (" + absDefVLNV.getVersion() + ")");
		return TabDocument::saveAs();
	}
	
	// if only bus def was being edited, write bus def and set the name
    libHandler_->writeModelToFile(busDirectory, busDef_);
	setDocumentName(busDefVLNV.getName() + " (" + busDefVLNV.getVersion() + ")");
	return TabDocument::saveAs();
}

void BusEditor::setupLayout() {

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&busDefGroup_);
	topLayout->addWidget(&absDefGroup_, 1);
}

VLNV BusEditor::getComponentVLNV() const {

	// if abstraction definition is being edited then use it as the identifier.
	if (absDef_)
		return *absDef_->getVlnv();

	// if only bus definition is being edited then use it as identifier.
	else
		return *busDef_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void BusEditor::setProtection(bool locked)
{
    TabDocument::setProtection(locked);
    setEnabled(!locked);
}
