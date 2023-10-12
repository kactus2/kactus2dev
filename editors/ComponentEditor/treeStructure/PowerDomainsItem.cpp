//-----------------------------------------------------------------------------
// File: PowerDomainsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.10.2023
//
// Description:
// The Power domains-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "PowerDomainsItem.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PowerDomain.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <editors/ComponentEditor/powerDomains/PowerDomainsEditor.h>

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::PowerDomainsItem()
//-----------------------------------------------------------------------------
PowerDomainsItem::PowerDomainsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> referenceCounter, 
    ExpressionSet expressions, ComponentEditorItem* parent) :
ComponentEditorItem(model, libHandler, component, parent),
    validator_(new PowerDomainValidator(component->getPowerDomains(), expressions.parser,
        QSharedPointer<ParameterValidator>(new ParameterValidator(expressions.parser, component->getChoices(),
            component->getRevision())))),
    expressions_(expressions)
{

    setReferenceCounter(referenceCounter);
}


//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::getFont()
//-----------------------------------------------------------------------------
QFont PowerDomainsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!childItems_.isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::text()
//-----------------------------------------------------------------------------
QString PowerDomainsItem::text() const
{
	return tr("Power domains");
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* PowerDomainsItem::editor()
{
	if (!editor_)
    {
        editor_ = new PowerDomainsEditor(component_, libHandler_, validator_, expressions_);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::getTooltip()
//-----------------------------------------------------------------------------
QString PowerDomainsItem::getTooltip() const
{
	return tr("Contains the power domains of the component");
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::isValid()
//-----------------------------------------------------------------------------
bool PowerDomainsItem::isValid() const
{
    QVector<QString> domainNames;

    return std::all_of(component_->getPowerDomains()->cbegin(), component_->getPowerDomains()->cend(),
        [this, &domainNames](auto const& domain)
        { 
            if (domainNames.contains(domain->name()) || !validator_->validate(domain))
            {
                return false;
            }

            domainNames.append(domain->name());
            return true;
        });
}
