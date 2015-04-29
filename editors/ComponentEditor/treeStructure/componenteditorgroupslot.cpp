/* 
 *  	Created on: 21.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorgroupslot.cpp
 *		Project: Kactus 2
 */

#include "componenteditorgroupslot.h"

//-----------------------------------------------------------------------------
// Function: ComponentEditorGroupSlot::ComponentEditorGroupSlot()
//-----------------------------------------------------------------------------
ComponentEditorGroupSlot::ComponentEditorGroupSlot(QWidget* parent):
QWidget(parent),
layout_(this),
currentWidget_(NULL)
{
	layout_.setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorGroupSlot::~ComponentEditorGroupSlot()
//-----------------------------------------------------------------------------
ComponentEditorGroupSlot::~ComponentEditorGroupSlot()
{
	if (currentWidget_)
    {
		currentWidget_->hide();
		layout_.removeWidget(currentWidget_);
		currentWidget_->setParent(NULL);
		currentWidget_ = NULL;
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorGroupSlot::setWidget()
//-----------------------------------------------------------------------------
void ComponentEditorGroupSlot::setWidget(QWidget* widget)
{
    if (widget != currentWidget_)
    {
        // if there is a previous widget being displayed
        if (currentWidget_) 
        {
            // set the previous widget to hidden
            currentWidget_->hide();

            // remove this widget as the parent of the widget
            currentWidget_->setParent(NULL);

            // remove the previous because only one widget can be displayed at a time.
            layout_.removeWidget(currentWidget_);
        }

        // update the current widget and display it
        currentWidget_ = widget;

        // TODO remove this and replace with Q_ASSERT(widget)
        if (currentWidget_)
        {
            layout_.addWidget(currentWidget_);
            currentWidget_->show();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorGroupSlot::getWidget()
//-----------------------------------------------------------------------------
QWidget* ComponentEditorGroupSlot::getWidget() const
{
	return currentWidget_;
}
