//-----------------------------------------------------------------------------
// File: absdefgroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 15.8.2011
//
// Description:
// Editor for the logical ports of an abstraction definition.
//-----------------------------------------------------------------------------

#include "absdefgroup.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>
#include <KactusAPI/include/PortAbstractionInterface.h>

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/AbstractParameterInterface.h>

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>

#include <editors/common/DocumentNameGroupEditor.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include "AbstractionDefinitionPortsSortFilter.h"
#include "AbstractionPortsModel.h"
#include "LogicalPortColumns.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::AbsDefGroup()
//-----------------------------------------------------------------------------
AbsDefGroup::AbsDefGroup(Document::Revision revision, QSharedPointer<AbstractionDefinition> absDef,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder, LibraryInterface* libraryHandler,
    PortAbstractionInterface* portInterface, PortAbstractionInterface* extendInterface, QWidget* parent) :
QWidget(parent),
documentNameGroupEditor_(new DocumentNameGroupEditor(this)),
extendEditor_(new VLNVEditor(VLNV::ABSTRACTIONDEFINITION, libraryHandler, this, this)),
busDisplay_(new VLNVDisplayer(this, VLNV())),
portTabs_(this),
portInterface_(portInterface),
extendInterface_(extendInterface),
parameterEditor_(new ParameterGroupBox(absDef->getParameters(), absDef->getChoices(), parameterFinder,
    expressionFormatter, absDef->getRevision(), this)),
parameterCompleter_(new ComponentParameterModel(parameterFinder, this)),
portModel_(new AbstractionPortsModel(parameterFinder, expressionParser, libraryHandler, portInterface, 
    extendInterface, this)),
wirePortsEditor_(new AbstractionPortsEditor(parameterCompleter_, parameterFinder, libraryHandler, 
    portInterface, revision, portModel_, LogicalPortColumns::AbstractionType::WIRE, &portTabs_)),
transactionalPortsEditor_(new AbstractionPortsEditor(parameterCompleter_, parameterFinder, libraryHandler, 
    portInterface, revision, portModel_, LogicalPortColumns::AbstractionType::TRANSACTIONAL, &portTabs_)),
abstraction_(absDef),
libraryHandler_(libraryHandler)
{
    portInterface_->setAbsDef(abstraction_);

    documentNameGroupEditor_->setTitle(QStringLiteral("Abstraction definition"));
    extendEditor_->setTitle(tr("Extended abstraction definition"));
    extendEditor_->setMandatory(false);
    extendEditor_->setRevisionFilter(true, revision);

    busDisplay_->setTitle(QStringLiteral("Bus Type"));

    portTabs_.addTab(wirePortsEditor_, QStringLiteral("Wire ports"));
    portTabs_.addTab(transactionalPortsEditor_, QStringLiteral("Transactional ports"));

    connect(parameterEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(parameterEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(parameterEditor_, SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);
    connect(parameterEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(parameterEditor_,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        this,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        Qt::UniqueConnection);

    connect(wirePortsEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(wirePortsEditor_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(wirePortsEditor_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(wirePortsEditor_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)),
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);
    connect(wirePortsEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(wirePortsEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(transactionalPortsEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(transactionalPortsEditor_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(transactionalPortsEditor_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(transactionalPortsEditor_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)),
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);
    connect(transactionalPortsEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(transactionalPortsEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(documentNameGroupEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(extendEditor_, SIGNAL(vlnvEdited()), this, SLOT(onExtendChanged()), Qt::UniqueConnection);

	setupLayout();
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::save()
//-----------------------------------------------------------------------------
void AbsDefGroup::save()
{
    portInterface_->save();
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::setAbsDef()
//-----------------------------------------------------------------------------
void AbsDefGroup::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
    abstraction_ = absDef;

    portInterface_->setAbsDef(abstraction_);
    parameterEditor_->setNewParameters(abstraction_->getParameters(), abstraction_->getChoices(), 
        abstraction_->getRevision());

    auto busDefinition = libraryHandler_->getModel<BusDefinition>(absDef->getBusType());

    wirePortsEditor_->setBusDef(busDefinition);
    transactionalPortsEditor_->setBusDef(busDefinition);

    wirePortsEditor_->resetPortModel();
    transactionalPortsEditor_->resetPortModel();

    documentNameGroupEditor_->setDocumentNameGroup(absDef, libraryHandler_->getPath(absDef->getVlnv()));

    extendEditor_->setVLNV(absDef->getExtends());
    extendEditor_->setRevisionFilter(true, absDef->getRevision());

    busDisplay_->setVLNV(absDef->getBusType());

    if (abstractionContainsTransactionalPorts())
    {
        portTabs_.setCurrentWidget(transactionalPortsEditor_);
    }

    if (absDef->getExtends().isValid())
    {
        setupExtendedAbstraction();
    }
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::abstractionContainsTransactionalPorts()
//-----------------------------------------------------------------------------
bool AbsDefGroup::abstractionContainsTransactionalPorts() const
{
    return std::any_of(abstraction_->getLogicalPorts()->cbegin(), abstraction_->getLogicalPorts()->cend(), 
        [](auto const& logicalPort) {return logicalPort->hasTransactional();});
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::onExtendChanged()
//-----------------------------------------------------------------------------
void AbsDefGroup::onExtendChanged()
{
    removeSignalsFromExtendedDefinition();

    abstraction_->setExtends(extendEditor_->getVLNV());
    setupExtendedAbstraction();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::setupExtendedAbstraction()
//-----------------------------------------------------------------------------
void AbsDefGroup::setupExtendedAbstraction()
{
    QSharedPointer<const AbstractionDefinition> extendedAbstraction = getExtendedAbstraction();
    extendInterface_->setAbsDef(extendedAbstraction);
    QString extendDescription = "";
    if (extendedAbstraction)
    {
        extendDescription = extendedAbstraction->getDescription();

        if (extendedAbstraction->getLogicalPorts() && !extendedAbstraction->getLogicalPorts()->isEmpty())
        {
            extendSignals(extendedAbstraction);
        }
    }

    documentNameGroupEditor_->setPlaceholderDescription(extendDescription);
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::getExtendedAbstraction()
//-----------------------------------------------------------------------------
QSharedPointer<const AbstractionDefinition> AbsDefGroup::getExtendedAbstraction() const
{
    VLNV extendedVLNV = abstraction_->getExtends();
    if (extendedVLNV.isValid() && extendedVLNV.getType() == VLNV::ABSTRACTIONDEFINITION)
    {
        QSharedPointer<const Document> extendedDocument = libraryHandler_->getModelReadOnly(extendedVLNV);
        if (extendedDocument)
        {
            QSharedPointer<const AbstractionDefinition> extendAbstraction =
                extendedDocument.dynamicCast<const AbstractionDefinition>();
            if (extendAbstraction)
            {
                return extendAbstraction;
            }
        }
    }

    return QSharedPointer<const AbstractionDefinition>();
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::extendSignals()
//-----------------------------------------------------------------------------
void AbsDefGroup::extendSignals(QSharedPointer<const AbstractionDefinition> /*extendAbstraction*/)
{
    portModel_->setExtendedPorts();
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::removeSignalsFromExtendedDefinition()
//-----------------------------------------------------------------------------
void AbsDefGroup::removeSignalsFromExtendedDefinition()
{
    if (abstraction_->getExtends() != extendEditor_->getVLNV() || extendEditor_->getVLNV().isEmpty())
    {
        QSharedPointer<const AbstractionDefinition> extendedAbstraction = getExtendedAbstraction();
        if (extendedAbstraction || extendEditor_->getVLNV().isEmpty())
        {
            portModel_->removeExtendedPorts();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::setupLayout()
//-----------------------------------------------------------------------------
void AbsDefGroup::setupLayout()
{
    QVBoxLayout* documentNameGroupLayout = new QVBoxLayout();
    documentNameGroupLayout->addWidget(documentNameGroupEditor_);

    QGridLayout* topLayout = new QGridLayout(this);
    topLayout->addWidget(extendEditor_, 0, 1, 1, 1);
    topLayout->addWidget(documentNameGroupEditor_, 0, 0, 2, 1);
    topLayout->addWidget(busDisplay_, 1, 1, 1, 1);

    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    splitter->addWidget(&portTabs_);
    splitter->addWidget(parameterEditor_);

    QSplitterHandle* handle = splitter->handle(1);
    QVBoxLayout* handleLayout = new QVBoxLayout(handle);
    handleLayout->setSpacing(0);
    handleLayout->setContentsMargins(0, 0, 0, 0);

    QFrame* line = new QFrame(handle);
    line->setLineWidth(2);
    line->setMidLineWidth(2);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    handleLayout->addWidget(line);
    
    splitter->setStyleSheet(QStringLiteral("QSplitter::handle { background: white }"));
    splitter->setHandleWidth(18);

    topLayout->addWidget(splitter, 2, 0, 1, 2);

    topLayout->setColumnStretch(0, 1);
    topLayout->setColumnStretch(1, 1);

    topLayout->setRowStretch(0, 1);
    topLayout->setRowStretch(1, 1);
    topLayout->setRowStretch(2, 4);
}
