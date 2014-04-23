//-----------------------------------------------------------------------------
// File: ComDefinitionEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.4.2012
//
// Description:
// Editor for editing COM definitions.
//-----------------------------------------------------------------------------

#ifndef COMDEFINITIONEDITOR_H
#define COMDEFINITIONEDITOR_H

#include "ComPropertyEditor.h"

#include <common/widgets/tabDocument/TabDocument.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/listManager/listmanager.h>
#include <models/ComDefinition.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QSharedPointer>
#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QString>
#include <QMap>
#include <QList>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
//! Editor for editing COM definitions.
//-----------------------------------------------------------------------------
class ComDefinitionEditor : public TabDocument
{
	Q_OBJECT

public:
	/*!
     *  Constructor.
	 *
	 *      @param [in] parent      The parent widget.
     *      @param [in] libHandler  The library interface.
     *      @param [in] comDef      The COM definition.
	 */
	ComDefinitionEditor(QWidget* parent, LibraryInterface* libHandler,
                        QSharedPointer<ComDefinition> comDef);
	
	/*!
     *  Destructor.
     */
	virtual ~ComDefinitionEditor();

	/*! \brief Get the VLNV that can be used to identify the COM definition.
	 *
	 * \return The VLNV of the COM definition
	*/
	virtual VLNV getIdentifyingVLNV() const;

    /*!
     *  Sets the protection state of the document.
     *
     *      @param [in] locked True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

    /*!
     *  Refreshes the editor.
     */
    virtual void refresh();

	/*!
     *  Returns the VLNV of the SW design component.
     */
	virtual VLNV getDocumentVLNV() const;

public slots:
    /*!
     *  Validates the document against the IP-XACT standard.
     *
     *      @param [out] errorList Error message list for reporting standard violations.
     *
     *      @return True if the document is valid. False if there were any violations.
     */
    virtual bool validate(QStringList& errorList);

	//! Saves the document and resets the modified state.
	virtual bool save();

	//! \brief Saves the document as new object and resets modifies state
	virtual bool saveAs();

protected:
    //! Called when the editor is shown.
    virtual void showEvent(QShowEvent* event);

private:
    // Disable copying.
    ComDefinitionEditor(ComDefinitionEditor const& rhs);
    ComDefinitionEditor& operator=(ComDefinitionEditor const& rhs);

    /*!
     *  Applies the changes made in the editor to the document.
     */
    void applyChanges();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the instance that handles the library.
	LibraryInterface* libHandler_;

	//! The COM definition being edited.
	QSharedPointer<ComDefinition> comDef_;

    //! The data type list manager.
    ListManager dataTypeList_;

    //! The properties editor.
    ComPropertyEditor propertyEditor_;
};

//-----------------------------------------------------------------------------

#endif // COMDEFINITIONEDITOR_H
