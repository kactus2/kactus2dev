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

#include <editors/BusDefinitionEditor/AbstractionDefinitionPortsSortFilter.h>

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::AbsDefGroup()
//-----------------------------------------------------------------------------
AbsDefGroup::AbsDefGroup(LibraryInterface* libraryHandler, QWidget *parent):
QGroupBox(tr("Signals (Abstraction Definition)"), parent),
portView_(this),
portProxy_(new AbstractionDefinitionPortsSortFilter(this)),
portModel_(this),
portDelegate_(this),
vlnvDisplay_(new VLNVDisplayer(this, VLNV())),
extendVLNVEditor_(new VLNVEditor(VLNV::ABSTRACTIONDEFINITION, libraryHandler, this, this)),
descriptionEditor_(new QPlainTextEdit(this)),
abstraction_()
{
    extendVLNVEditor_->setToolTip(QString("Extended abstraction definition is not currently supported in Kactus2"));

    vlnvDisplay_->setTitle(QStringLiteral("Abstraction definition VLNV"));
    extendVLNVEditor_->setTitle(tr("Extended abstraction definition"));

    extendVLNVEditor_->setDisabled(true);

    portProxy_->setSourceModel(&portModel_);

	portView_.setModel(portProxy_);
    portView_.setSortingEnabled(true);
	portView_.setItemDelegate(&portDelegate_);
    portView_.setAllowImportExport(true);
    portView_.setItemsDraggable(false);
    portView_.setAlternatingRowColors(false);

    portView_.sortByColumn(0, Qt::AscendingOrder);

	connect(&portView_, SIGNAL(addSignalOptions()), this, SLOT(onAddSignalOptions()), Qt::UniqueConnection);
	
	connect(&portModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(portRenamed(const QString&, const QString&)), 
        this, SIGNAL(portRenamed(const QString&, const QString&)), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(portRenamed(const QString&, const QString&)), 
        portProxy_, SLOT(invalidate()), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), 
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);

    connect(&portView_, SIGNAL(addItem(const QModelIndex&)), &portModel_, SLOT(addSignal()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(removeItem(const QModelIndex&)),
        &portModel_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);

	setupLayout();
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::~AbsDefGroup()
//-----------------------------------------------------------------------------
AbsDefGroup::~AbsDefGroup()
{

}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::onAddSignalOptions()
//-----------------------------------------------------------------------------
void AbsDefGroup::onAddSignalOptions()
{
    QModelIndexList selection;
    foreach (QModelIndex index, portView_.selected())
    {
        selection.append(portProxy_->mapToSource(index));
    }

	portModel_.addSignalOptions(selection);
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::save()
//-----------------------------------------------------------------------------
void AbsDefGroup::save()
{
	portModel_.save();
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::setAbsDef()
//-----------------------------------------------------------------------------
void AbsDefGroup::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
    abstraction_ = absDef;

	portModel_.setAbsDef(absDef);
    vlnvDisplay_->setVLNV(absDef->getVlnv());

    if (absDef->getExtends().isValid())
    {
        extendVLNVEditor_->setVLNV(absDef->getExtends());
    }

    if (!absDef->getDescription().isEmpty())
    {
        descriptionEditor_->setPlainText(absDef->getDescription());
    }
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::setBusDef()
//-----------------------------------------------------------------------------
void AbsDefGroup::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    portDelegate_.setBusDef(busDefinition);
    portModel_.setBusDef(busDefinition);
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::setupLayout()
//-----------------------------------------------------------------------------
void AbsDefGroup::setupLayout()
{
    QGroupBox* descriptionGroup = new QGroupBox(tr("Description"), this);

    QVBoxLayout* descriptionLayout = new QVBoxLayout(descriptionGroup);
    descriptionLayout->addWidget(descriptionEditor_);

    QGridLayout* topLayout = new QGridLayout(this);
    topLayout->addWidget(vlnvDisplay_, 0, 0, 1, 1);
    topLayout->addWidget(extendVLNVEditor_, 0, 1, 1, 1);
    topLayout->addWidget(descriptionGroup, 0, 2, 1, 1);
    topLayout->addWidget(&portView_, 1, 0, 1, 3);

    topLayout->setColumnStretch(0, 25);
    topLayout->setColumnStretch(1, 25);
    topLayout->setColumnStretch(2, 50);

    topLayout->setRowStretch(0, 1);
    topLayout->setRowStretch(1, 10);
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::onDescriptionChanged()
//-----------------------------------------------------------------------------
void AbsDefGroup::onDescriptionChanged()
{
    abstraction_->setDescription(descriptionEditor_->toPlainText());
    emit contentChanged();
}
