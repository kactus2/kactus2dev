//-----------------------------------------------------------------------------
// File: addressspaceeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.2.2012
//
// Description:
// Editor to edit and save settings of an address space within component editor.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEEDITOR_H
#define ADDRESSSPACEEDITOR_H

#include "addressspacegeneraleditor.h"
#include "segmenteditor.h"
#include "localMemoryMap/localmemorymapeditor.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/addressspace.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <editors/ComponentEditor/itemeditor.h>

#include <QSharedPointer>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor to edit and save settings of an address space within component editor.
//-----------------------------------------------------------------------------
class AddressSpaceEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               The component being edited.
	 *      @param [in] handler                 The instance managing the library.
	 *      @param [in] addrSpace               The address space being edited.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] expressionParser        The expression parser to use.
	 *      @param [in] parent                  The owner of this editor.
	 */
	AddressSpaceEditor(QSharedPointer<Component> component,
		LibraryInterface* handler,
		QSharedPointer<AddressSpace> addrSpace,
        QSharedPointer <ParameterFinder> parameterFinder,
        QSharedPointer <ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
		QWidget* parent = 0);
	
	//! The destructor
	virtual ~AddressSpaceEditor();

	/*! Check for the validity of the address space.
	* 
	*       @return True if address space is valid and can be saved.
	*/
	virtual bool isValid() const;

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();


protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying
	AddressSpaceEditor(const AddressSpaceEditor& other);
	AddressSpaceEditor& operator=(const AddressSpaceEditor& other);

    //! Sets the widget layout.
    void setupLayout();

	//! The address space being edited.
	QSharedPointer<AddressSpace> addrSpace_;

	//! The editor to set the name, display name and description.
	NameGroupEditor nameEditor_;

	//! The editor to set the general settings of address space.
	AddressSpaceGeneralEditor generalEditor_;

	//! The editor to set the segments of the address space.
	SegmentEditor segmentsEditor_;

	//! The editor to set the local memory map.
	LocalMemoryMapEditor localMemMapEditor_;
};

#endif // ADDRESSSPACEEDITOR_H
