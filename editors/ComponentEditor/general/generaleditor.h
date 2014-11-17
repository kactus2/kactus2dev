//-----------------------------------------------------------------------------
// File: generaleditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.2.2011
//
// Description:
// General editor for editing the component's general data.
//-----------------------------------------------------------------------------

#ifndef GENERALEDITOR_H
#define GENERALEDITOR_H

#include "../itemeditor.h"
#include "desceditor.h"

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <QSharedPointer>
#include <QLineEdit>

class Component;
class LibraryInterface;
class LibraryComponent;
class KactusAttributeEditor;
class ComponentPreviewBox;

//-----------------------------------------------------------------------------
//! GeneralEditor class.
//-----------------------------------------------------------------------------
class GeneralEditor : public ItemEditor
{
    Q_OBJECT
       
public:
    /*!
     *  Constructor.
     *
     *      @param [in] libHandler The library handler.
     *      @param [in] component  The component model to edit.
     *      @param [in] parent     The parent widget.
	 */
	GeneralEditor(LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QWidget *parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~GeneralEditor();

    /*! \brief Check for the validity of the edited item.
     *
     * Base class implementation always returns true.
     * 
     * \return True is item is valid.
     */
    virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

signals:

	/*!
	 *  Signaled when the product hierarchy has changed.
	 */
	void hierarchyChanged(QSettings& settings);

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! \brief Handler for changes in attribute editor.
	void onAttributesChange();

	//! \brief Handler for changes in description.
	void onDescriptionChange();

	//! \brief Handler for changes in XML header.
	void onHeaderChange();

    //! \brief Handler for changes in author field.
    void onAuthorChange();

private:
    // Disable copying.
    GeneralEditor(const GeneralEditor& other);
    GeneralEditor& operator=(const GeneralEditor& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The VLNV displayer.
    VLNVDisplayer* vlnvDisplayer_;

    //! Kactus attribute editor.
    KactusAttributeEditor* attributeEditor_;

    QLineEdit* authorEditor_;

    //! The description editor.
    DescEditor* descEditor_;

	//! \brief The editor to view/edit the header of the XML-file.
	DescEditor* headerEditor_;

    //! The preview box.
    ComponentPreviewBox* previewBox_;
};

//-----------------------------------------------------------------------------

#endif // GENERALEDITOR_H
