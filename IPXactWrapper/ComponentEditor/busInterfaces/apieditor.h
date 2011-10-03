//-----------------------------------------------------------------------------
// File: apieditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 8.9.2011
//
// Description:
// API editor for SW Platform components.
//-----------------------------------------------------------------------------

#ifndef APIEDITOR_H
#define APIEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include "busifgeneraltab.h"

#include <QSharedPointer>
#include <QRadioButton>
#include <QGroupBox>

class Component;
class BusInterface;
class LibraryInterface;

/*! \brief Container for editors to edit a bus interface.
 *
 */
class APIEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the library handler instance.
	 * \param component Pointer to the component being edited.
	 * \param dataPointer Pointer  to the bus interface being edited.
	 * \param parent Pointer to the owner of this container.
	 *
	*/
	APIEditor(LibraryInterface* libHandler,
		QSharedPointer<Component> component, 
		void* dataPointer, QWidget *parent);
	
	//! \brief The destructor
	virtual ~APIEditor();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Remove the edited bus interface from the model.
	 *
	 * This function should be used when user wants to remove an element from 
	 * the component.
	 *
	*/
	virtual void removeModel();

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	 * The base class implementation does nothing.
	*/
	virtual void makeChanges();

    /*!
     *  Allows/disallows the editor change.
     *
     *      @return True, if this editor can be hidden without consequences. Otherwise false.
     */
    virtual bool confirmEditorChange();

private:

    /*!
     *  Restores the changes.
     */
    void restoreChanges();

    //! \brief No copying
    APIEditor(const APIEditor& other);

    //! No assignment
    APIEditor& operator=(const APIEditor& other);

    //-----------------------------------------------------------------------------
    //! APIType enumeration.
    //-----------------------------------------------------------------------------
    enum APIType
    {
        API_TYPE_UPPER = 0,
        API_TYPE_LOWER,
        API_TYPE_COUNT
    };
	
	//! \brief Pointer to the API being edited.
	BusInterface* api_;

    //! \brief Contains the name, display name and description of bus interface
    NameGroupBox nameGroup_;

    //! \brief Contains the attributes of the bus interface
    AttributeBox attributes_;

    //! \brief Contains the vlnv of the bus type for this bus interface
    VLNVEditor busType_;

    //! API type group.
    QGroupBox apiTypeGroup_;

    //! API type radio buttons.
    QRadioButton* apiTypeButtons_[API_TYPE_COUNT];

    //! \brief Pointer to the instance that manages the library
    LibraryInterface* libHandler_;
};

#endif // APIEDITOR_H
