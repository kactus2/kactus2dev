//-----------------------------------------------------------------------------
// File: AbstractionDefinitionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.6.2011
//
// Description:
// Bus defition editor is an editor for Bus- and Abstraction definitions.
//-----------------------------------------------------------------------------

#include "AbstractionDefinitionEditor.h"

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/BusDefinition/validators/BusDefinitionValidator.h>
#include <IPXACTmodels/AbstractionDefinition/validators/AbstractionDefinitionValidator.h>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>
#include <KactusAPI/include/PortAbstractionInterface.h>
#include <KactusAPI/include/ExpressionFormatter.h>

#include "AbsDefParameterReferenceCounter.h"
#include "AbsDefParameterReferenceTree.h"

#include <editors/ComponentEditor/parameterReferenceTree/ParameterReferenceTreeWindow.h>

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionEditor::AbstractionDefinitionEditor()
//-----------------------------------------------------------------------------
AbstractionDefinitionEditor::AbstractionDefinitionEditor(QWidget *parent, LibraryInterface* libHandler, QSharedPointer<AbstractionDefinition> absDef, Document::Revision revision):
TabDocument(parent, libHandler, DOC_PROTECTION_SUPPORT),
absDef_(absDef),
expressionFormatter_(new ExpressionFormatter(absDefParameterFinder_)),
absDefGroup_(revision, absDef, expressionFormatter_, absDefParameterFinder_, libHandler, 
    createPortAbstractionInterface(), createPortAbstractionInterface(), this),
expressionParser_(new SystemVerilogExpressionParser()),
absDefinitionValidator_(new AbstractionDefinitionValidator(libHandler, expressionParser_)),
referenceCounter_(new AbsDefParameterReferenceCounter(absDefParameterFinder_, absDef)),
parameterReferenceTree_(new AbsDefParameterReferenceTree(absDef, referenceCounter_, expressionFormatter_)),
referenceTreeWindow_(new ParameterReferenceTreeWindow(parameterReferenceTree_))
{
    setDocumentType(DocumentType(DocumentTypes::ABSTRACTION_DEFINITION));

    if (absDef_)
    {
        absDefGroup_.setAbsDef(absDef_);

        VLNV vlnv = absDef_->getVlnv();
        setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");

        // Open in unlocked mode by default only if the version is draft.
        setProtection(vlnv.getVersion() != "draft");
    }
    else
    {
        setProtection(true);
    }

    setupLayout();

    connect(&absDefGroup_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&absDefGroup_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&absDefGroup_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(&absDefGroup_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), 
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);

    connect(&absDefGroup_, SIGNAL(increaseReferences(QString)),
        referenceCounter_.data(), SLOT(increaseReferenceCount(QString)), Qt::UniqueConnection);

    connect(&absDefGroup_, SIGNAL(decreaseReferences(QString)),
        referenceCounter_.data(), SLOT(decreaseReferenceCount(QString)), Qt::UniqueConnection);

    connect(&absDefGroup_, SIGNAL(openReferenceTree(QString const&, QString const&)),
        referenceTreeWindow_, SLOT(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    connect(&absDefGroup_, SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        referenceCounter_.data(), SLOT(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionEditor::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV AbstractionDefinitionEditor::getIdentifyingVLNV() const
{
    return getDocumentVLNV();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionEditor::refresh()
//-----------------------------------------------------------------------------
void AbstractionDefinitionEditor::refresh()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if (absDef_)
    {
        absDef_ = getLibHandler()->getModel(absDef_->getVlnv()).dynamicCast<AbstractionDefinition>();
        absDefGroup_.setAbsDef(absDef_);
        referenceCounter_->setAbstractionDefinition(absDef_);
        parameterReferenceTree_->setAbsDef(absDef_);
    }

    // The document is no longer modified.
    setModified(false);
    TabDocument::refresh();

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void AbstractionDefinitionEditor::setProtection(bool locked)
{
    TabDocument::setProtection(locked);
    setEnabled(!locked);
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionEditor::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV AbstractionDefinitionEditor::getDocumentVLNV() const
{
    // If abstraction definition is being edited then use it as the identifier.
    if (absDef_)
    {
        return absDef_->getVlnv();
    }
    else
    {
        return VLNV();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionEditor::setAbsDef()
//-----------------------------------------------------------------------------
void AbstractionDefinitionEditor::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
    absDef_ = absDef;

    if (absDef_) 
    {
        absDefGroup_.setAbsDef(absDef_);
        referenceCounter_->setAbstractionDefinition(absDef_);
        parameterReferenceTree_->setAbsDef(absDef_);
    }

    absDefGroup_.setDisabled(!absDef_);
    
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionEditor::validate()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionEditor::validate(QVector<QString>& errorList)
{
    absDefGroup_.save();
    absDefinitionValidator_->findErrorsIn(errorList, absDef_);

    return errorList.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionEditor::save()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionEditor::save()
{  
    absDefGroup_.save();
    getLibHandler()->writeModelToFile(absDef_);

	return TabDocument::save();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionEditor::saveAs()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionEditor::saveAs()
{
    // Ask the user for a new VLNV along with the directory.
    VLNV vlnv;

    VLNV absDefVLNV;
    QString absDirectory;

    if (!NewObjectDialog::saveAsDialog(this, getLibHandler(), absDef_->getVlnv(), vlnv, absDirectory))
    {
        return false;
    }

    absDefVLNV = vlnv;
    absDefVLNV.setType(VLNV::ABSTRACTIONDEFINITION);
    absDef_->setVlnv(absDefVLNV);


    absDefGroup_.save();
  
    getLibHandler()->writeModelToFile(absDirectory, absDef_);

    setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
    return TabDocument::saveAs();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionEditor::showEvent()
//-----------------------------------------------------------------------------
void AbstractionDefinitionEditor::showEvent(QShowEvent* event)
{
    TabDocument::showEvent(event);

    if (absDef_->getRevision() == Document::Revision::Std22)
    {
        emit helpUrlRequested("definitions/abstractiondefinition2022.html");
    }
    else
    {
        emit helpUrlRequested("definitions/abstractiondefinition.html");
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionEditor::setupLayout()
//-----------------------------------------------------------------------------
void AbstractionDefinitionEditor::setupLayout()
{    
    QWidget* absDefGroup = new QWidget(this);
    QVBoxLayout* absLayout = new QVBoxLayout(absDefGroup);
    absLayout->addWidget(new SummaryLabel(tr("Abstraction definition"), this), 0, Qt::AlignCenter);
    absLayout->addWidget(&absDefGroup_);
    absLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addWidget(absDefGroup);
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionEditor::()
//-----------------------------------------------------------------------------
PortAbstractionInterface* AbstractionDefinitionEditor::createPortAbstractionInterface()
{
    PortAbstractionInterface* portInterface(new PortAbstractionInterface());
    return portInterface;
}
