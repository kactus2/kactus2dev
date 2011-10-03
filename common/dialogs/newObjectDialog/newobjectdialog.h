#ifndef NEWOBJECTDIALOG_H
#define NEWOBJECTDIALOG_H

#include <QDialog>
#include <QPushButton>

#include <common/KactusAttribute.h>
#include <LibraryManager/vlnv.h>

class QLineEdit;
class LibraryInterface;
class VLNVEditor;
class KactusAttributeEditor;

/*! \brief NewObjectDialog is used to query the VLNV and path
 * of a newly created design
 *
 */
class NewObjectDialog: public QDialog
{
    Q_OBJECT

public:
    NewObjectDialog(LibraryInterface* libInterface, VLNV::IPXactType type,
                    bool showAttributes, QWidget *parent = 0);

	/*! \brief Set the vlnv for the editor
	 *
	 * \param vlnv Reference to the vlnv to set
	 *
	*/
	void setVLNV(const VLNV& vlnv);

    /*!
     *  Sets the Kactus attributes for the dialog.
     *
     *      @param [in] prodHier    Product hierarchy.
     *      @param [in] firmness	firmness.
     */
    void setKactusAttributes(KactusAttribute::ProductHierarchy prodHier,
                             KactusAttribute::Firmness firmness);

    /*! \brief Get the user inputted VLNV
     *
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

    /*! \brief Get the user inputted path
     *
     */
    QString getPath();

    /*!
     *  Shows a pre-filled dialog with the version field set empty.
     *
     *      @return True, if the user pressed OK. False if the user pressed Cancel.
     */
    static bool saveAsDialog(QWidget* parent, LibraryInterface* lh, VLNV const& oldVLNV,
                             KactusAttribute::ProductHierarchy& prodHier,
                             KactusAttribute::Firmness& firmness,
                             VLNV& vlnv, QString& directory);

    /*!
     *  Shows a pre-filled dialog with the version field set empty and attributes hidden.
     *
     *      @return True, if the user pressed OK. False if the user pressed Cancel.
     */
    static bool saveAsDialog(QWidget* parent, LibraryInterface* lh, VLNV const& oldVLNV,
                             VLNV& vlnv, QString& directory,
							 const QString& windowTitle = QString("Save As"));

private slots:
    /*!
     *  Asks the user to select a directory.
     */
    void selectDirectory();

    /*!
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

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
    //! The library interface.
    LibraryInterface* lh_;

    //! Attribute editor.
    KactusAttributeEditor* attributeEditor_;

    //! The VLNV editor.
    VLNVEditor* vlnvEditor_;

    //! The directory edit.
    QLineEdit *directoryEdit_;

    //! OK button.
    QPushButton* okButton_;
};

#endif // NEWOBJECTDIALOG_H
