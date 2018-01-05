//-----------------------------------------------------------------------------
// File: PortWireTypeView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.11.2017
//
// Description:
// View for port wire type definitions.
//-----------------------------------------------------------------------------

#ifndef PORTWIRETYPEVIEW_H
#define PORTWIRETYPEVIEW_H

#include <common/views/EditableTreeView/EditableTreeView.h>

//-----------------------------------------------------------------------------
//! View for port wire type definitions.
//-----------------------------------------------------------------------------
class PortWireTypeView : public EditableTreeView
{
    Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this view.
     */
	PortWireTypeView(QWidget* parent);
	
	/*!
     *  The destructor.
     */
    virtual ~PortWireTypeView();

signals:

    /*!
     *  Close and commit any open type definition editors.
     */
    void closeOpenTypeDefinitionEditor();

protected:

    /*!
     *  Handles the context menu events.
     *
     *      @param [in] event   The context menu event.
     */
    virtual void contextMenuEvent(QContextMenuEvent* event);

private:
	
	//! No copying. No assignment.
	PortWireTypeView(const PortWireTypeView& other);
	PortWireTypeView& operator=(const PortWireTypeView& other);

};

#endif // PORTWIRETYPEMODEL_H
