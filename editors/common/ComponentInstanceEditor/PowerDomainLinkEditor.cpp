

#include "PowerDomainLinkEditor.h"

#include <IPXACTmodels/common/Parameter.h>

#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkEditor::PowerDomainLinkEditor()
//-----------------------------------------------------------------------------
PowerDomainLinkEditor::PowerDomainLinkEditor(QWidget* parent):
    QGroupBox(tr("Power domains"), parent)
{
    view_->setItemDelegate(delegate_);
    view_->setModel(model_);
    
    view_->setSelectionBehavior(QAbstractItemView::SelectItems);
    view_->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(view_, SIGNAL(addItem(QModelIndex const&)), model_, SLOT(addItem(QModelIndex const&)));
    connect(view_, SIGNAL(removeItem(QModelIndex const&)), model_, SLOT(removeItem(QModelIndex const&)));

    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkEditor::setContent()
//-----------------------------------------------------------------------------
void PowerDomainLinkEditor::setContent(ComponentInstance::PowerDomainLink::List powerDomains,
    QSharedPointer<Component> externalComponent, QSharedPointer<Component> internalComponent) const noexcept
{
    model_->setPowerDomains(powerDomains);
    delegate_->setComponents(externalComponent, internalComponent);
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkEditor::setupLayout()
//-----------------------------------------------------------------------------
void PowerDomainLinkEditor::setupLayout()
{
    auto layout = new QHBoxLayout(this);

    layout->addWidget(view_);
}
