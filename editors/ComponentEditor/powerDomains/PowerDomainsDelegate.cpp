//-----------------------------------------------------------------------------
// File: PowerDomainsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 5.10.2023
//
// Description:
// The delegate that provides editors to edit PowerDomains of a component.
//-----------------------------------------------------------------------------

#include "PowerDomainsDelegate.h"

#include "PowerDomainColumns.h"

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <IPXACTmodels/Component/PowerDomain.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: PowerDomainsDelegate::PowerDomainsDelegate()
//-----------------------------------------------------------------------------
PowerDomainsDelegate::PowerDomainsDelegate(QSharedPointer<QList<QSharedPointer<PowerDomain> > > powerDomains,
    QAbstractItemModel* completionModel,
    QSharedPointer<ParameterFinder> parameterFinder, 
    //QSharedPointer<PortTypeValidator> typeValidator,
    QObject* parent):
ExpressionDelegate(completionModel, parameterFinder, parent),
powerDomains_(powerDomains)
{

}

//-----------------------------------------------------------------------------
// Function: PowerDomainsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PowerDomainsDelegate::createEditor(QWidget * parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == PowerDomainColumns::SUBDOMAIN)
    {
        auto editor = new ReferenceSelector(parent);

        QStringList domainNames;
        for (auto domain : *powerDomains_)
        {
            domainNames.append(domain->name());
        }
        
        domainNames.removeAll(index.siblingAtColumn(PowerDomainColumns::NAME).data().toString());

        editor->addItem(QString());
        editor->addItems(domainNames);


        return editor;
    }

    return ExpressionDelegate::createEditor(parent, option, index);

}

//-----------------------------------------------------------------------------
// Function: PowerDomainsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PowerDomainsDelegate::setEditorData(QWidget * editor, QModelIndex const& index) const
{
    if (index.column() == PowerDomainColumns::SUBDOMAIN)
    {
        auto domainEditor = qobject_cast<ReferenceSelector*>(editor);
        domainEditor->setCurrentText(index.data().toString());
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }

}

//-----------------------------------------------------------------------------
// Function: PowerDomainsDelegate::setModelData()
//-----------------------------------------------------------------------------
void PowerDomainsDelegate::setModelData(QWidget* editor, QAbstractItemModel * model, QModelIndex const& index) const
{
    if (index.column() == PowerDomainColumns::SUBDOMAIN)
    {
        auto domainEditor = qobject_cast<ReferenceSelector*>(editor);
        model->setData(index, domainEditor->currentText(), Qt::EditRole);
    }
    else
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool PowerDomainsDelegate::columnAcceptsExpression(int column) const
{
    return column == PowerDomainColumns::ALWAYS_ON;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int PowerDomainsDelegate::descriptionColumn() const
{
    return PowerDomainColumns::DESCRIPTION;
}
