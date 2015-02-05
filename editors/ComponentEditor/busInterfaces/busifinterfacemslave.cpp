/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemslave.cpp
 */

#include "busifinterfacemslave.h"

#include <IPXACTmodels/generaldeclarations.h>
#include <common/widgets/parameterComboBox/parametercombobox.h>

#include <QLabel>
#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::BusIfInterfaceMSlave()
//-----------------------------------------------------------------------------
BusIfInterfaceMSlave::BusIfInterfaceMSlave(QSharedPointer<BusInterface> busif, 
										   QSharedPointer<Component> component,
                                           QSharedPointer<ParameterFinder> parameterFinder,
										   QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("Mirrored slave"), parent),
mirroredSlave_(QSharedPointer<MirroredSlaveInterface>(new MirroredSlaveInterface())),
remapEdit_(new ParameterComboBox(parameterFinder, this, true)),
rangeEdit_(new ParameterComboBox(parameterFinder, this, true))
{
	QLabel* remapLabel = new QLabel(tr("Remap address"), this);
	QLabel* rangeLabel = new QLabel(tr("Range"), this);

	QGridLayout* topLayout = new QGridLayout(this);
	topLayout->addWidget(remapLabel, 0, 0, Qt::AlignLeft);
	topLayout->addWidget(remapEdit_, 0, 1, Qt::AlignLeft);
	topLayout->addWidget(rangeLabel, 1, 0, Qt::AlignLeft);
	topLayout->addWidget(rangeEdit_, 1, 1, Qt::AlignLeft);
	topLayout->setColumnStretch(2, 1);
	topLayout->setRowStretch(2, 1);

	connect(remapEdit_, SIGNAL(textChanged(const QString&)),
		this, SLOT(onRemapChange(const QString&)), Qt::UniqueConnection);

    connect(rangeEdit_, SIGNAL(textChanged(const QString&)),
        this, SLOT(onRangeChange(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::~BusIfInterfaceMSlave()
//-----------------------------------------------------------------------------
BusIfInterfaceMSlave::~BusIfInterfaceMSlave()
{

}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::isValid()
//-----------------------------------------------------------------------------
bool BusIfInterfaceMSlave::isValid() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::refresh()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::refresh() {
	// if the model contains master-element
	if (busif_->getMirroredSlave()) {
		mirroredSlave_ = busif_->getMirroredSlave();
	}
	else {
		mirroredSlave_.clear();
		mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(new MirroredSlaveInterface());
	}

	remapEdit_->refresh();
    QString remapID = mirroredSlave_->getRemapAddressID();
    if (!remapID.isEmpty())
    {
        int remapIndex = remapEdit_->findData(remapID);
        remapEdit_->setRemapText(remapEdit_->itemText(remapIndex));
    }
    else
    {
        remapEdit_->setRemapText(mirroredSlave_->getRemapAddress());
    }

    rangeEdit_->refresh();
    QString rangeID = mirroredSlave_->getRangeID();
    if (!rangeID.isEmpty())
    {
        int rangeIndex = rangeEdit_->findData(rangeID);
        rangeEdit_->setRemapText(rangeEdit_->itemText(rangeIndex));
    }
    else
    {
        rangeEdit_->setRemapText(mirroredSlave_->getRange());
    }
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusIfInterfaceMSlave::getInterfaceMode() const {
	return General::MIRROREDSLAVE;
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::saveModeSpecific()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::saveModeSpecific() {
	busif_->setMirroredSlave(mirroredSlave_);
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::onRemapChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::onRemapChange( const QString& newRemapAddress )
{
    QString oldID = mirroredSlave_->getRemapAddressID();
    if (!oldID.isEmpty())
    {
        emit decreaseReferences(oldID);
    }

    mirroredSlave_->setRemapAddress(newRemapAddress);

    if (isBoxValueParameter(remapEdit_, newRemapAddress))
    {
        QString currentID = remapEdit_->itemData(remapEdit_->currentIndex()).toString();
        mirroredSlave_->setRemapAddressID(currentID);
        emit increaseReferences(currentID);
    }

    else
    {
        mirroredSlave_->removeRemapAddressID();
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::onRangeChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::onRangeChange(const QString& newRange)
{
    QString oldID = mirroredSlave_->getRangeID();
    if (!oldID.isEmpty())
    {
        emit decreaseReferences(oldID);
    }

    mirroredSlave_->setRange(newRange);

    if (isBoxValueParameter(rangeEdit_, newRange))
    {
        QString currentID = rangeEdit_->itemData(remapEdit_->currentIndex()).toString();
        mirroredSlave_->setRangeID(currentID);
        emit increaseReferences(currentID);
    }

    else
    {
        mirroredSlave_->removeRangeID();
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::isBoxValueParameter()
//-----------------------------------------------------------------------------
bool BusIfInterfaceMSlave::isBoxValueParameter(ParameterComboBox* combo, QString newText)
{
    for (int i = 0; i < combo->count(); ++i)
    {
        if (combo->itemText(i) == newText)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::giveRemapAddressID()
//-----------------------------------------------------------------------------
QString BusIfInterfaceMSlave::getRemapAddressID()
{
    return mirroredSlave_->getRemapAddressID();
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::giveRangeID()
//-----------------------------------------------------------------------------
QString BusIfInterfaceMSlave::getRangeID()
{
    return mirroredSlave_->getRangeID();
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::onBusIfParametersChanged()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::onBusIfParametersChanged()
{
    QString remapAddressID = getRemapAddressID();
    QString remapAddressName = mirroredSlave_->getRemapAddress();

    QString rangeID = getRangeID();
    QString rangeName = mirroredSlave_->getRange();

    remapEdit_->refresh();
    rangeEdit_->refresh();

    if (remapAddressID.isEmpty())
    {
        remapEdit_->setRemapText(remapAddressName);
    }
    else
    {
        int remapIndex = remapEdit_->findData(remapAddressID);
        remapEdit_->setRemapText(remapEdit_->itemText(remapIndex));
    }

    if (rangeID.isEmpty())
    {
        rangeEdit_->setRemapText(rangeName);
    }
    else
    {
        int rangeIndex = rangeEdit_->findData(rangeID);
        rangeEdit_->setRemapText(rangeEdit_->itemText(rangeIndex));
    }
}
