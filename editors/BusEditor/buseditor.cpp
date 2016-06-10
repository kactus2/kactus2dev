//-----------------------------------------------------------------------------
// File: buseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.6.2011
//
// Description:
// Bus Editor is an editor for Bus- and Abstraction definitions.
//-----------------------------------------------------------------------------

#include "buseditor.h"

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/BusDefinition/validators/BusDefinitionValidator.h>

#include <IPXACTmodels/AbstractionDefinition/validators/AbstractionDefinitionValidator.h>

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: BusEditor::BusEditor()
//-----------------------------------------------------------------------------
BusEditor::BusEditor(QWidget *parent, LibraryInterface* libHandler, QSharedPointer<BusDefinition> busDef, 
    QSharedPointer<AbstractionDefinition> absDef, bool disableBusDef):
TabDocument(parent, DOC_PROTECTION_SUPPORT), 
    libHandler_(libHandler),
    busDef_(busDef),
    absDef_(absDef),
    busDefGroup_(this),
    absDefGroup_(this),
    expressionParser_(new SystemVerilogExpressionParser()),
    busDefinitionValidator_(new BusDefinitionValidator(expressionParser_)),
    absDefinitionValidator_(new AbstractionDefinitionValidator(libHandler, expressionParser_))
{
    if (absDef_)
    {
        absDefGroup_.setAbsDef(absDef_);
    }
	else
    {
		absDefGroup_.setDisabled(true);
	}

    if (busDef_)
    {
        busDefGroup_.setBusDef(busDef_);
        absDefGroup_.setBusDef(busDef_);
    } 
    
	busDefGroup_.setDisabled(!busDef_ || disableBusDef);

    setDocumentType(tr("Bus"));

    if (busDef_)
    {
        VLNV vlnv = busDef_->getVlnv();
        setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");

        // Open in unlocked mode by default only if the version is draft.
        setProtection(vlnv.getVersion() != "draft");
    }
    else
    {
        setProtection(true);
    }    

    setupLayout();

    connect(&busDefGroup_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&absDefGroup_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&absDefGroup_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&absDefGroup_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(&absDefGroup_, SIGNAL(portRenamed(const QString&, const QString&)), 
        this, SIGNAL(portRenamed(const QString&, const QString&)), Qt::UniqueConnection);
    connect(&absDefGroup_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), 
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusEditor::~BusEditor()
//-----------------------------------------------------------------------------
BusEditor::~BusEditor()
{

}

//-----------------------------------------------------------------------------
// Function: BusEditor::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV BusEditor::getIdentifyingVLNV() const
{
    return getDocumentVLNV();
}

//-----------------------------------------------------------------------------
// Function: BusEditor::refresh()
//-----------------------------------------------------------------------------
void BusEditor::refresh()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if (absDef_)
    {
        absDef_ = libHandler_->getModel(absDef_->getVlnv()).dynamicCast<AbstractionDefinition>();
        absDefGroup_.setAbsDef(absDef_);
    }

    if (busDef_)
    {
        busDef_ = libHandler_->getModel(busDef_->getVlnv()).dynamicCast<BusDefinition>();
        busDefGroup_.setBusDef(busDef_);
        absDefGroup_.setBusDef(busDef_);
    } 

    // The document is no longer modified.
    setModified(false);
    TabDocument::refresh();

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void BusEditor::setProtection(bool locked)
{
    TabDocument::setProtection(locked);
    setEnabled(!locked);
}

//-----------------------------------------------------------------------------
// Function: BusEditor::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV BusEditor::getDocumentVLNV() const
{
    // If abstraction definition is being edited then use it as the identifier.
    if (absDef_)
    {
        return absDef_->getVlnv();
    }
    // If only bus definition is being edited then use it as identifier.
    else if (busDef_)
    {
        return busDef_->getVlnv();	
    }
    else
    {
        return VLNV();
    }
}

//-----------------------------------------------------------------------------
// Function: BusEditor::setBusDef()
//-----------------------------------------------------------------------------
void BusEditor::setBusDef(QSharedPointer<BusDefinition> busDef)
{
    Q_ASSERT_X(busDef, "BusEditor setBusDef", "Null Bus Definition pointer given as parameter");

    busDef_ = busDef;

    busDefGroup_.setBusDef(busDef_);
    absDefGroup_.setBusDef(busDef_);
    busDefGroup_.setDisabled(false);

    VLNV vlnv = busDef_->getVlnv();
    setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
}

//-----------------------------------------------------------------------------
// Function: BusEditor::setAbsDef()
//-----------------------------------------------------------------------------
void BusEditor::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
    absDef_ = absDef;

    if (absDef_) 
    {
        absDefGroup_.setAbsDef(absDef_);
    }

    absDefGroup_.setDisabled(!absDef_);    
}

//-----------------------------------------------------------------------------
// Function: BusEditor::validate()
//-----------------------------------------------------------------------------
bool BusEditor::validate(QVector<QString>& errorList)
{    
    // if abstraction definition is being edited
    if (absDefGroup_.isEnabled())
    {
        absDefinitionValidator_->findErrorsIn(errorList, absDef_);
    }

    // if bus definition is being edited
    if (busDefGroup_.isEnabled())
    {
        busDefinitionValidator_->findErrorsIn(errorList, busDef_);
    }

    return errorList.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: BusEditor::save()
//-----------------------------------------------------------------------------
bool BusEditor::save()
{
	// If abstraction definition is being edited, save it.
	if (absDefGroup_.isEnabled())
    {
		absDefGroup_.save();
		libHandler_->writeModelToFile(absDef_);
	}

	// If bus definition is being edited, save it.
	if (busDefGroup_.isEnabled())
    {
		libHandler_->writeModelToFile(busDef_);
	}

	return TabDocument::save();
}

//-----------------------------------------------------------------------------
// Function: BusEditor::saveAs()
//-----------------------------------------------------------------------------
bool BusEditor::saveAs()
{
    // Ask the user for a new VLNV along with the directory.
    VLNV vlnv;
    
	VLNV busDefVLNV;
	QString busDirectory;

	VLNV absDefVLNV;
	QString absDirectory;

	// if bus definition is being edited
	if (busDefGroup_.isEnabled())
    {
		if (!NewObjectDialog::saveAsDialog(this, libHandler_, busDef_->getVlnv(), vlnv, busDirectory))
        {
			return false;
		}

		busDefVLNV = vlnv;
        busDefVLNV.setType(VLNV::BUSDEFINITION);

		busDef_->setVlnv(busDefVLNV);
	}

	// if abstraction definition is being edited but not the bus definition
	else if (absDef_ && !busDefGroup_.isEnabled())
    {
		if (!NewObjectDialog::saveAsDialog(this, libHandler_, absDef_->getVlnv(), vlnv, absDirectory))
        {
			return false;
		}

		absDefGroup_.save();

		absDefVLNV = vlnv;
        absDefVLNV.setType(VLNV::ABSTRACTIONDEFINITION);

		absDef_->setVlnv(absDefVLNV);

		// write only the abs def
		libHandler_->writeModelToFile(absDirectory, absDef_);

		setDocumentName(absDefVLNV.getName() + " (" + absDefVLNV.getVersion() + ")");
		return TabDocument::saveAs();
	}
    
	// if both are being edited (now the bus definition vlnv has already been defined but not abs def)
	if (busDefGroup_.isEnabled() && absDef_)
    {
		// remove the possible .busDef from the end of the name field
		QString absDefName = busDefVLNV.getName();
		absDefName = absDefName.remove(".busDef", Qt::CaseInsensitive);

		absDefGroup_.save();

		// create automatically vlnv for the abstraction definition
		absDefVLNV = VLNV(VLNV::ABSTRACTIONDEFINITION, vlnv.getVendor(), vlnv.getLibrary(),
			absDefName + ".absDef", vlnv.getVersion());

		// by default both bus def and abs def are saved in same directory
		absDirectory = busDirectory;

		// if the automatic abs def vlnv is taken
		if (libHandler_->contains(absDefVLNV))
        {
			VLNV newAbsDefVLNV;

			if (!NewObjectDialog::saveAsDialog(this, libHandler_, absDefVLNV, newAbsDefVLNV, absDirectory))
            {
				return false;
			}
			// save the created abstraction definition vlnv
			absDefVLNV = newAbsDefVLNV;
		}

		// update the vlnvs of the abs def
		absDef_->setVlnv(absDefVLNV);

		// update the abs def's reference to bus def
		absDef_->setBusType(busDefVLNV);

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

//-----------------------------------------------------------------------------
// Function: BusEditor::showEvent()
//-----------------------------------------------------------------------------
void BusEditor::showEvent(QShowEvent* event)
{
    TabDocument::showEvent(event);
    emit helpUrlRequested("definitions/busdefinition.html");
}

//-----------------------------------------------------------------------------
// Function: BusEditor::setupLayout()
//-----------------------------------------------------------------------------
void BusEditor::setupLayout()
{    
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);    

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(&busDefGroup_);
    verticalSplitter->addWidget(&absDefGroup_);
    verticalSplitter->setStretchFactor(1, 1);
    verticalSplitter->setContentsMargins(2, 2, 2, 2);
    verticalSplitter->setHandleWidth(10);

    QSplitterHandle* handle = verticalSplitter->handle(1);
    QVBoxLayout* handleLayout = new QVBoxLayout(handle);
    handleLayout->setSpacing(0);
    handleLayout->setMargin(0);

    QFrame* line = new QFrame(handle);
    line->setLineWidth(2);
    line->setMidLineWidth(2);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    handleLayout->addWidget(line);

    scrollArea->setWidget(verticalSplitter);
}
