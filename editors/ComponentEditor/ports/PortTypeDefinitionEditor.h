//-----------------------------------------------------------------------------
// File: PortTypeDefinitionEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.01.2018
//
// Description:
// Editor for a list of port type definitions.
//-----------------------------------------------------------------------------

#ifndef PORTTYPEDEFINITIONEDITOR_H
#define PORTTYPEDEFINITIONEDITOR_H

#include <common/widgets/listManager/listeditor.h>

#include <QModelIndex>

//-----------------------------------------------------------------------------
//! Editor for a list of port type definitions.
//-----------------------------------------------------------------------------
class PortTypeDefinitionEditor : public ListEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] indexedRow  Index of the type for which this editor belongs to.
	 *      @param [in] parent      Pointer to the owner of this editor.
	 */
    PortTypeDefinitionEditor(QModelIndex const& indexedRow, QWidget* parent);
	
	/*!
     *  The destructor.
     */
    virtual ~PortTypeDefinitionEditor();

signals:

    /*!
     *  Informs of the closing of this editor.
     *
     *      @param [in] indexedRow  Index of the containing type.
     */
    void closingEditor(QModelIndex const& indexedRow);

private:
	
	//! No copying. No assignment.
    PortTypeDefinitionEditor(const PortTypeDefinitionEditor& other);
    PortTypeDefinitionEditor& operator=(const PortTypeDefinitionEditor& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The index of the containing type.
    QModelIndex indexedRow_;
};

#endif // PORTTYPEDEFINITIONEDITOR_H
