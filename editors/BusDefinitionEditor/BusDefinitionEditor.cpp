//-----------------------------------------------------------------------------
// File: BusDefinitionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.6.2011
//
// Description:
// Bus defition editor is an editor for Bus- and Abstraction definitions.
//-----------------------------------------------------------------------------

#include "BusDefinitionEditor.h"

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/BusDefinition/validators/BusDefinitionValidator.h>
#include <IPXACTmodels/AbstractionDefinition/validators/AbstractionDefinitionValidator.h>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>
#include <KactusAPI/include/PortAbstractionInterface.h>

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::BusDefinitionEditor()
//-----------------------------------------------------------------------------
BusDefinitionEditor::BusDefinitionEditor(QWidget *parent, LibraryInterface* libHandler,
    QSharedPointer<BusDefinition> busDef):
TabDocument(parent, DOC_PROTECTION_SUPPORT),
libHandler_(libHandler),
busDef_(busDef),
busDefGroup_(libHandler, this),
expressionParser_(new SystemVerilogExpressionParser()),
busDefinitionValidator_(new BusDefinitionValidator(libHandler, expressionParser_))
{
    setDocumentType(tr("Bus Definition"));

    if (busDef_)
    {
        busDefGroup_.setBusDef(busDef_);

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
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV BusDefinitionEditor::getIdentifyingVLNV() const
{
    return getDocumentVLNV();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::refresh()
//-----------------------------------------------------------------------------
void BusDefinitionEditor::refresh()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if (busDef_)
    {
        busDef_ = libHandler_->getModel(busDef_->getVlnv()).dynamicCast<BusDefinition>();
        busDefGroup_.setBusDef(busDef_);
    } 

    // The document is no longer modified.
    setModified(false);
    TabDocument::refresh();

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void BusDefinitionEditor::setProtection(bool locked)
{
    TabDocument::setProtection(locked);
    setEnabled(!locked);
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV BusDefinitionEditor::getDocumentVLNV() const
{
    // If bus definition is being edited then use it as identifier.
    if (busDef_)
    {
        return busDef_->getVlnv();	
    }
    else
    {
        return VLNV();
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::setBusDef()
//-----------------------------------------------------------------------------
void BusDefinitionEditor::setBusDef(QSharedPointer<BusDefinition> busDef)
{
    Q_ASSERT_X(busDef, "BusDefinitionEditor setBusDef", "Null Bus Definition pointer given as parameter");

    busDef_ = busDef;

    busDefGroup_.setBusDef(busDef_);
    busDefGroup_.setDisabled(false);

    VLNV vlnv = busDef_->getVlnv();
    setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::validate()
//-----------------------------------------------------------------------------
bool BusDefinitionEditor::validate(QVector<QString>& errorList)
{
    QVector<QString> busDefinitionErrors;
    busDefinitionValidator_->findErrorsIn(busDefinitionErrors, busDef_);

    if (busDefinitionErrors.isEmpty() == false)
    {
        errorList.append(busDefinitionErrors);
    }

    return busDefinitionErrors.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::save()
//-----------------------------------------------------------------------------
bool BusDefinitionEditor::save()
{
    libHandler_->writeModelToFile(busDef_);

    return TabDocument::save();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::saveAs()
//-----------------------------------------------------------------------------
bool BusDefinitionEditor::saveAs()
{
    // Ask the user for a new VLNV along with the directory.
    VLNV vlnv;

    VLNV busDefVLNV;
    QString busDirectory;

    VLNV absDefVLNV;
    QString absDirectory;

    if (!NewObjectDialog::saveAsDialog(this, libHandler_, busDef_->getVlnv(), vlnv, busDirectory))
    {
        return false;
    }

    busDefVLNV = vlnv;
    busDefVLNV.setType(VLNV::BUSDEFINITION);
    busDef_->setVlnv(busDefVLNV);
    
    libHandler_->writeModelToFile(busDirectory, busDef_);
    setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
    return TabDocument::saveAs();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::showEvent()
//-----------------------------------------------------------------------------
void BusDefinitionEditor::showEvent(QShowEvent* event)
{
    TabDocument::showEvent(event);
    emit helpUrlRequested("definitions/busdefinition.html");
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionEditor::setupLayout()
//-----------------------------------------------------------------------------
void BusDefinitionEditor::setupLayout()
{    
    QWidget* busDefGroup = new QWidget(this);
    QVBoxLayout* busLayout = new QVBoxLayout(busDefGroup);
    busLayout->addWidget(new SummaryLabel(tr("Bus definition"), this), 0, Qt::AlignCenter);
    busLayout->addWidget(&busDefGroup_);
    busLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addWidget(busDefGroup);
}
