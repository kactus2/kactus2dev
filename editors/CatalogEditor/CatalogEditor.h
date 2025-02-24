//-----------------------------------------------------------------------------
// File: CatalogEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.02.2017
//
// Description:
// Editor for editing Catalogs.
//-----------------------------------------------------------------------------

#ifndef CATALOGEDITOR_H
#define CATALOGEDITOR_H

#include <common/widgets/tabDocument/TabDocument.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <editors/common/DocumentNameGroupEditor.h>

#include <QTextEdit>
#include <QTreeView>

class Catalog;
class CatalogFileModel;
class LibraryInterface;
class VLNVDisplayer;

//-----------------------------------------------------------------------------
//! Editor for editing Catalogs.
//-----------------------------------------------------------------------------
class CatalogEditor : public TabDocument
{
	Q_OBJECT

public:
	/*!
     *  Constructor.
	 *
	 *    @param [in] library     The library of available IP-XACT documents.
     *    @param [in] catalog     The catalog to edit.
     *    @param [in] parent      The parent widget.
	 */
	CatalogEditor(LibraryInterface* libHandler, QSharedPointer<Catalog> catalog, QWidget* parent);
	
	/*!
     *  Destructor.
     */
	virtual ~CatalogEditor();

	/*! Get the VLNV that can be used to identify the catalog.
	 *
	 *    @return The VLNV of the catalog.
	*/
	virtual VLNV getIdentifyingVLNV() const;

	/*!
     *  Returns the VLNV of the SW design component.
     */
	virtual VLNV getDocumentVLNV() const;
    
    /*!
     *  Refreshes the editor.
     */
    virtual void refresh();

    /*!
     *  Sets the protection state of the document.
     *
     *    @param [in] locked True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

signals:
    
    //! Emitted when a catalog should be opened.
    void openCatalog(VLNV const&);

    //! Emitted when a component should be opened.
    void openComponent(VLNV const&);

public slots:
    /*!
     *  Validates the document against the IP-XACT standard.
     *
     *    @param [out] errorList Error message list for reporting standard violations.
     *
     *    @return True if the document is valid. False if there were any violations.
     */
    virtual bool validate(QVector<QString>& errorList);

	//! Saves the document and resets the modified state.
	virtual bool save();

	//! Saves the document as new object and resets modifies state
	virtual bool saveAs();

    /*!
     *  Displays warning message when trying to drag and drop an IP-XACT file using different standard revision
     *  than the catalog.
     */
    void showStdRevisionMismatchWarning();
    
protected:
    //! Called when the editor is shown.
    virtual void showEvent(QShowEvent* event);

private:
    // Disable copying.
    CatalogEditor(CatalogEditor const& rhs);
    CatalogEditor& operator=(CatalogEditor const& rhs);

    //! Sets the editor layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The catalog being edited.
    QSharedPointer<Catalog> catalog_;

    //! The document name group editor.
    DocumentNameGroupEditor* documentNameGroupEditor_;

    //! Model for catalog files.
    CatalogFileModel* fileModel_;

    //! View for catalog files.
    QTreeView* fileView_;

};

//-----------------------------------------------------------------------------

#endif // CATALOGEDITOR_H
