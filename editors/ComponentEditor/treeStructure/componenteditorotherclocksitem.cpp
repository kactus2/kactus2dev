//-----------------------------------------------------------------------------
// File: componenteditorotherclocksitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Other clock drivers-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorotherclocksitem.h"

#include <editors/ComponentEditor/otherClockDrivers/otherclockdriverseditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/OtherClockDriver.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorOtherClocksItem::ComponentEditorOtherClocksItem()
//-----------------------------------------------------------------------------
ComponentEditorOtherClocksItem::ComponentEditorOtherClocksItem(ComponentEditorTreeModel* model, 
    LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    otherClocks_(component->getOtherClockDrivers())
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorOtherClocksItem::~ComponentEditorOtherClocksItem()
//-----------------------------------------------------------------------------
ComponentEditorOtherClocksItem::~ComponentEditorOtherClocksItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorOtherClocksItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorOtherClocksItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!otherClocks_->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorOtherClocksItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorOtherClocksItem::text() const
{
	return tr("Other clock drivers");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorOtherClocksItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorOtherClocksItem::isValid() const
{
	/*foreach (QSharedPointer<OtherClockDriver> otherClock, otherClocks_)
    {
		if (!otherClock->isValid())
        {
			return false;
		}
	}*/
	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorOtherClocksItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorOtherClocksItem::editor()
{
	if (!editor_)
    {
		editor_ = new OtherClockDriversEditor(component_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorOtherClocksItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorOtherClocksItem::getTooltip() const
{
	return tr("Contains the details that define clock waveforms");
}
