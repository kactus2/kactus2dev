//-----------------------------------------------------------------------------
// File: VendorExtensionsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.12.2018
//
// Description:
// Editor to display/edit vendor extensions.
//-----------------------------------------------------------------------------

#ifndef VENDOREXTENSIONSEDITOR_H
#define VENDOREXTENSIONSEDITOR_H

#include <QList>
#include <QSharedPointer>
#include <QWidget>

class EditableTableView;
class VendorExtensionsModel;
class VendorExtension;
class TabDocument;

//-----------------------------------------------------------------------------
//! Editor to display/edit vendor extensions.
//-----------------------------------------------------------------------------
class VendorExtensionsEditor : public QWidget
{
    Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent      The owner of this widget.     
	 */
	VendorExtensionsEditor(QWidget *parent);
	
	//! The destructor.
	virtual ~VendorExtensionsEditor() = default;

    //! No copying.
    VendorExtensionsEditor(const VendorExtensionsEditor& other) = delete;

    //! No assignment.
    VendorExtensionsEditor& operator=(const VendorExtensionsEditor& other) = delete;

	/*!
     *  Set the vendor extensions for the editor.
	 *
	 *      @param [in] extensions  The vendor extensions to edit.
	 */
    void setVendorExtensions(QSharedPointer<QList<QSharedPointer<VendorExtension> >  > extensions);

    /*!
     * Locks/Unlocks the editor.
     *
     *     @param [in] locked  The lock state to set.
     */
    void setLocked(bool locked);
    
    /*!
     * Set the context for the editor.
     *
     *     @param [in] doc  The document widget whose vendor extensions are being edited.     
     */
     void setContext(TabDocument* doc);

signals:

    //! Informs of a content change in the editor.
    void contentChanged();

public slots:

	//! Clear the editor so no extensions are shown
    void clear();

	
private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The view for the vendor extensions table.
    EditableTableView* extensionsView_;

    //! The model for the vendor extensions.
    VendorExtensionsModel* extensionsModel_;

};

#endif // VENDOREXTENSIONSEDITOR_H
