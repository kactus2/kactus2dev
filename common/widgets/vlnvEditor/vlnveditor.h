//-----------------------------------------------------------------------------
// File: vlnveditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.2.2011
//
// Description:
// VLNV editor group box.
//-----------------------------------------------------------------------------

#ifndef VLNVEDITOR_H
#define VLNVEDITOR_H

#include <LibraryManager/vlnv.h>

#include <QGroupBox>
#include <QLineEdit>
#include <QDropEvent>
#include <QDragEnterEvent>

class LibraryInterface;
class LibraryItem;
class AssistedLineEdit;
class VLNVContentMatcher;

//-----------------------------------------------------------------------------
//! VLNVEditor class.
//-----------------------------------------------------------------------------
class VLNVEditor : public QGroupBox
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] vlnvType    The VLNV type which to edit.
     *      @param [in] parentWnd   The parent window (having a title bar).
     *      @param [in] libHandler  The library handler.
     *      @param [in] parent      The parent widget.
     *      @param [in] compact     If true, the VLNV layout is made compact (label and edit on the same row).
     *                              If false, the labels and edits are on different rows.
     */
    VLNVEditor(VLNV::IPXactType type, LibraryInterface* libHandler, QWidget* parentWnd = 0,
               QWidget* parent = 0, bool compact = true);

    /*!
     *  Destructor.
     */
    virtual ~VLNVEditor();

    /*!
     *  Sets the contents of the editor to match the given VLNV.
     *
     *      @param [in] vlnv The VLNV.
     */
    void setVLNV(VLNV const* vlnv);

	/*! 
	 * Set the contents of the editor to match the given VLNV.
	 *
	 *		@param [in] vlnv The VLNV. 
	 */
	void setVLNV(const VLNV& vlnv);

    /*!
     *  Generates a VLNV that matches the contents of the editor.
     *
     */
    VLNV getVLNV() const;

    /*!
     *  Returns true if the contents of the editor are valid for a VLNV.
     */
    bool isValid() const;

	/*!
	 * Checks if the editor contains text in any field.
	 */
	bool isEmpty() const;

signals:
    //! Emitted when the contents of the editor have changed.
    void contentChanged();

public slots:
    //! Called when the vendor field has changed.
    void updateLibraryMatcherItem();

    //! Called when the library field has changeed.
    void updateNameMatcherItem();

    //! Called when the name field has changed.
    void updateVersionMatcherItem();

	//! \brief Sets the vendor-field to contain the given string.
	void setVendor(const QString& vendor);

	//! \brief Sets the library-field to contain the given string.
	void setLibrary(const QString& library);

	//! \brief Sets the name-field to contain the given string.
	void setName(const QString& name);

	//! \brief Sets the version-field to contain the given string.
	void setVersion(const QString& version);

protected:

	/*! \brief Handler for drop events on drag & drop
	 *
	 * \param event Pointer to the drop event.
	 *
	*/
	virtual void dropEvent(QDropEvent* event);

	/*! \brief Handler for drag enter events in drag & drop.
	 *
	 * \param event Pointer to the event.
	 *
	*/
	virtual void dragEnterEvent(QDragEnterEvent* event);

private:
    // Disable copying.
    VLNVEditor(VLNVEditor const& rhs);
    VLNVEditor& operator=(VLNVEditor const& rhs);

    /*!
     *  Initializes the Qt connections between signals and slots.
     */
    void initConnections();

    /*!
     *  Initializes the widgets of the vlnv editor.
     *
     *      @param [in] parentWnd The parent window.
     *      @param [in] compact   If true, the layout is made compact.
     */
    void initWidgets(QWidget* parentWnd, bool compact);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The VLNV type which to edit.
    VLNV::IPXactType m_type;

    //! The IP-XACT sub-tree root of the library handler.
    LibraryItem const* m_rootItem;

    //! Line edit for the vendor element.
    AssistedLineEdit* m_vendorEdit;

    //! Content matcher for the vendor field.
    QSharedPointer<VLNVContentMatcher> m_vendorMatcher;

    //! Line edit for the library element.
    AssistedLineEdit* m_libraryEdit;

    //! Content matcher for the library field.
    QSharedPointer<VLNVContentMatcher> m_libraryMatcher;

    //! Line edit for the name element.
    AssistedLineEdit* m_nameEdit;

    //! Content matcher for the name field.
    QSharedPointer<VLNVContentMatcher> m_nameMatcher;

    //! Line edit for the version element.
    AssistedLineEdit* m_versionEdit;

    //! Content matcher for the version field.
    QSharedPointer<VLNVContentMatcher> m_versionMatcher;

	//! \brief Pointer to the instance that manages the library
	LibraryInterface* handler_;
};

//-----------------------------------------------------------------------------

#endif // VLNVEDITOR_H