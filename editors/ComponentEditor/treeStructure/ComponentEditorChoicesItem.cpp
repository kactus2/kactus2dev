
#include "ComponentEditorChoicesItem.h"

#include <editors/ComponentEditor/choices/ChoicesEditor.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::ComponentEditorChoicesItem()
//-----------------------------------------------------------------------------
ComponentEditorChoicesItem::ComponentEditorChoicesItem(ComponentEditorTreeModel* model, 
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent), component_(component)
{
	
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::~()
//-----------------------------------------------------------------------------
ComponentEditorChoicesItem::~ComponentEditorChoicesItem()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorChoicesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!component_->getChoices()->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorChoicesItem::getTooltip() const
{
    return tr("Contains the choices available in the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorChoicesItem::text() const
{
    return tr("Choices");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorChoicesItem::isValid() const
{
    if (editor_)
    {
        return editor_->isValid();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorChoicesItem::editor()
{
    if (!editor_)
    {
        editor_ = new ChoicesEditor(component_);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(contentChanged()),
            this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
    }

    return editor_;
}
