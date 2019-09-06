//-----------------------------------------------------------------------------
// File: newobjetdialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// NewObjectDialog is used to query the VLNV and path of a newly created document.
//-----------------------------------------------------------------------------

#ifndef NEWOBJECTDIALOG_H
#define NEWOBJECTDIALOG_H

#include <common/widgets/LibrarySelectorWidget/LibrarySelectorWidget.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/common/TagData.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QDialog>
#include <QPushButton>

class LibraryInterface;
class VLNVEditor;
class KactusAttributeEditor;

//-----------------------------------------------------------------------------
//! NewObjectDialog is used to query the VLNV and path of a newly created document.
//-----------------------------------------------------------------------------
class NewObjectDialog: public QDialog
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] libInterface    The library interface.
     *      @param [in] type            New object type.
     *      @param [in] showAttributes  Flag for visible attributes.
     *      @param [in] parent          Owner of this dialog.
     */
    NewObjectDialog(LibraryInterface* libInterface, VLNV::IPXactType type, bool showAttributes,
        QWidget *parent = 0);
    

    //! The destructor.
    virtual ~NewObjectDialog();

	/*!
     *  Set the vlnv for the editor.
	 *
     *      @param [in] vlnv    Reference to the vlnv to set.
	 */
	void setVLNV(const VLNV& vlnv);

    /*!
     *  Sets the Kactus attributes for the dialog.
     *
     *      @param [in] prodHier    Product hierarchy.
     *      @param [in] firmness    firmness.
     *      @param [in] tags        Tags.
     */
    void setKactusAttributes(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
        QVector<TagData> tags);

    /*!
     *  Get the user inputted VLNV.
     */
    VLNV getVLNV();

    /*!
     *  Returns the product hierarchy attribute.
     */
    KactusAttribute::ProductHierarchy getProductHierarchy() const;

    /*!
     *  Returns the firmness.
     */
    KactusAttribute::Firmness getFirmness() const;

    /*!
     *  Get the document tags.
     *
     *      @return Document tags.
     */
    QVector<TagData> getTags() const;

    /*!
     *  Get the user inputted path.
     */
    QString getPath();

    /*!
     *  Shows a pre-filled dialog with the version field set empty.
     *
     *      @param [in] parent      The parent widget.
     *      @param [in] lh          Interface to the library.
     *      @param [in] oldVLNV     The old VLNV.
     *      @param [in] prodHier    Product hierarchy.
     *      @param [in] firmness    Firmness.
     *      @param [in] tags        Document tags.
     *      @param [in] vlnv        VLNV.
     *      @param [in] directory   Directory.
     *
     *      @return True, if the user pressed OK. False if the user pressed Cancel.
     */
    static bool saveAsDialog(QWidget* parent, LibraryInterface* lh, VLNV const& oldVLNV,
        KactusAttribute::ProductHierarchy& prodHier, KactusAttribute::Firmness& firmness, QVector<TagData>& tags,
        VLNV& vlnv, QString& directory);

    /*!
     *  Shows a pre-filled dialog with the version field set empty and attributes hidden.
     *
     *      @return True, if the user pressed OK. False if the user pressed Cancel.
     */
    static bool saveAsDialog(QWidget* parent, LibraryInterface* lh, VLNV const& oldVLNV, VLNV& vlnv,
                             QString& directory, QString const& windowTitle = QString("Save As"));

private slots:
    /*!
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

    /*!
     *  Updates the OK button state when contents change.
     */
    void onContentChanged();

    /*!
     *  Called when the user presses OK.
     */
    virtual void accept();

    /*!
     *  Called when the product hierarchy has changed.
     */
    void onProductHierarchyChanged();

private:

    //! Disable copying.
    NewObjectDialog(NewObjectDialog const& other);
    NewObjectDialog& operator=(NewObjectDialog const& other);

    //! Sets the widget layout.
    void setupLayout();

    //! The library interface.
    LibraryInterface* library_;

    //! Attribute editor.
    KactusAttributeEditor* attributeEditor_;

    //! The VLNV editor.
    VLNVEditor* vlnvEditor_;

	//! The editor to select the directory to save to. 
	LibrarySelectorWidget* directoryEditor_;

    //! OK button.
    QPushButton* okButton_;
};

#endif // NEWOBJECTDIALOG_H
