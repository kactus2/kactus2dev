//-----------------------------------------------------------------------------
// File: ApiDefinitionEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.4.2012
//
// Description:
// Editor for API definitions.
//-----------------------------------------------------------------------------

#ifndef APIDEFINITIONEDITOR_H
#define APIDEFINITIONEDITOR_H

#include "ApiFunctionEditor.h"

#include <common/widgets/tabDocument/TabDocument.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/listManager/listmanager.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <models/ApiDefinition.h>

#include <LibraryManager/libraryinterface.h>

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
//! Editor for editing API definitions.
//-----------------------------------------------------------------------------
class ApiDefinitionEditor : public TabDocument
{
	Q_OBJECT

public:
	/*!
     *  Constructor.
	 *
	 *      @param [in] parent      The parent widget.
	 *      @param [in] libHandler  The library interface.
     *      @param [in] apiDef      The API definition.
	 */
	ApiDefinitionEditor(QWidget* parent, LibraryInterface* libHandler,
                        QSharedPointer<ApiDefinition> apiDef);
	
	/*!
     *  Destructor.
     */
	virtual ~ApiDefinitionEditor();

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
	virtual VLNV getComponentVLNV() const;

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

    //! Updates the data types suggestion lists for return value and function parameter editors.
    void updateDataTypeLists();

    //! Updates the COM definition.
    void updateComDefinition();
    
protected:
    //! Called when the editor is shown.
    virtual void showEvent(QShowEvent* event);

private:
    // Disable copying.
    ApiDefinitionEditor(ApiDefinitionEditor const& rhs);
    ApiDefinitionEditor& operator=(ApiDefinitionEditor const& rhs);

    /*!
     *  Applies the changes made in the editor to the document.
     */
    void applyChanges();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the instance that handles the library.
	LibraryInterface* libHandler_;

	//! The API definition being edited.
	QSharedPointer<ApiDefinition> apiDef_;

    //! VLNV editor for the linked COM definition.
    VLNVEditor comDefVLNVEdit_;

    //! The data type list manager.
    ListManager dataTypeList_;

    //! The function editor.
    ApiFunctionEditor functionEditor_;
};

//-----------------------------------------------------------------------------

#endif // APIDEFINITIONEDITOR_H
