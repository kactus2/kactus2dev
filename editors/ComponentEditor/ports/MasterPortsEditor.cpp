//-----------------------------------------------------------------------------
// File: MasterPortsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Master editor for component ports.
//-----------------------------------------------------------------------------

#include "MasterPortsEditor.h"

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/ports/portsdelegate.h>
#include <editors/ComponentEditor/ports/portsmodel.h>
#include <editors/ComponentEditor/ports/PortsView.h>
#include <editors/ComponentEditor/ports/PortsFilter.h>
#include <editors/ComponentEditor/ports/PortsEditorConstructor.h>
#include <editors/ComponentEditor/ports/interfaces/PortsInterface.h>

#include <common/widgets/summaryLabel/summarylabel.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: MasterPortsEditor::MasterPortsEditor()
//-----------------------------------------------------------------------------
MasterPortsEditor::MasterPortsEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<PortsInterface> portsInterface, PortsEditorConstructor* editorConstructor,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<PortValidator> portValidator,
    ParameterCompleter* parameterCompleter, QString const& defaultPath, QWidget *parent):
ItemEditor(component, handler, parent),
view_(editorConstructor->constructView(defaultPath, this)),
model_(0),
proxy_(editorConstructor->constructFilter(portsInterface, this)),
delegate_(editorConstructor->constructDelegate(
    component, parameterCompleter, parameterFinder, portValidator, this)),
portInterface_(portsInterface)
{
    model_ = editorConstructor->constructModel(parameterFinder, portsInterface, proxy_, this);

    view_->setItemDelegate(delegate_);

    proxy_->setSourceModel(model_);
    view_->setModel(proxy_);

    view_->resizeColumnsToContents();

    proxy_->setSortCaseSensitivity(Qt::CaseInsensitive);

    connectSignals();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_, 1);
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: MasterPortsEditor::connectSignals()
//-----------------------------------------------------------------------------
void MasterPortsEditor::connectSignals()
{
    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(model_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(view_, SIGNAL(addItem(const QModelIndex&)),
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(view_, SIGNAL(removeItem(const QModelIndex&)),
        model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(view_, SIGNAL(createBus(QStringList const&)),
        this, SIGNAL(createNewInteface(QStringList const&)), Qt::UniqueConnection);
    connect(view_, SIGNAL(createExistingBus(QStringList const&)),
        this, SIGNAL(createInterface(QStringList const&)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(delegate_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(view_, SIGNAL(changeExtensionsEditorItem(QModelIndex const&)),
        proxy_, SLOT(onHandleExtensionsEditorItemChange(QModelIndex const&)), Qt::UniqueConnection);
    connect(proxy_, SIGNAL(changeExtensionsEditorItem(QModelIndex const&)),
        this, SIGNAL(changeExtensionsEditorItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(model_, SIGNAL(portExtensionDataChanged(QModelIndex const&)),
        this, SIGNAL(changeExtensionsEditorItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(model_, SIGNAL(invalidateOtherFilter()), this, SIGNAL(invalidateOtherFilter()), Qt::UniqueConnection);
    connect(this, SIGNAL(ivalidateThisFilter()), proxy_, SLOT(invalidate()), Qt::UniqueConnection);

    connect(model_, SIGNAL(portCountChanged()), this, SIGNAL(portCountChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MasterPortsEditor::getAmountOfPorts()
//-----------------------------------------------------------------------------
int MasterPortsEditor::getAmountOfPorts() const
{
    return proxy_->rowCount();
}

//-----------------------------------------------------------------------------
// Function: MasterPortsEditor::isValid()
//-----------------------------------------------------------------------------
bool MasterPortsEditor::isValid() const
{
	return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: MasterPortsEditor::refresh()
//-----------------------------------------------------------------------------
void MasterPortsEditor::refresh()
{
	view_->update();
}

//-----------------------------------------------------------------------------
// Function: MasterPortsEditor::setAllowImportExport()
//-----------------------------------------------------------------------------
void MasterPortsEditor::setAllowImportExport(bool allow)
{
	view_->setAllowImportExport(allow);
}

//-----------------------------------------------------------------------------
// Function: MasterPortsEditor::setComponent()
//-----------------------------------------------------------------------------
void MasterPortsEditor::setComponent(QSharedPointer<Component> component)
{
    model_->resetModelAndLockCurrentPorts();
    delegate_->setComponent(component);
}

//-----------------------------------------------------------------------------
// Function: MasterPortsEditor::getIndexedPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> MasterPortsEditor::getIndexedPort(QModelIndex const& portIndex) const
{
    std::string portName = portInterface_->getIndexedItemName(portIndex.row());
    return portInterface_->getPort(portName);
}
