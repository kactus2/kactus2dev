//-----------------------------------------------------------------------------
// File: ObjectSelectionDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.07.2017
//
// Description:
// Dialog for selecting the items.
//-----------------------------------------------------------------------------

#ifndef OBJECTSELECTIONDIALOG_H
#define OBJECTSELECTIONDIALOG_H

#include <IPXACTmodels/common/VLNV.h>

#include <KactusAPI/KactusAPIGlobal.h>

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QSize>
#include <QCheckBox>
#include <QListWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

class ObjectSelectionListItem;

//-----------------------------------------------------------------------------
//! Dialog for selecting the items.
//-----------------------------------------------------------------------------
class ObjectSelectionDialog : public QDialog
{
    Q_OBJECT

public:

    /*!
	 *  The Constructor.
	 *
     *      @param [in] okButtonText    QString containing the text for the ok button.
     *      @param [in] iconPath        Path to the icon used by the selection dialog.
     *      @param [in] okIconPath      Path to the icon used by the ok button.
     *      @param [in] windowTitle     Window title of the selection dialog.
	 *      @param [in] parent          The parent widget.
	 *      @param [in] f               Widget flags for the dialog.
	 */
    ObjectSelectionDialog(QString const& okButtonText, QString const& iconPath, QString const& okIconPath,
        QString const& windowTitle, QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    /*!
	 *  The destructor.
	 */
    virtual ~ObjectSelectionDialog();

    /*!
     *  Create a list widget item for the dialog.
     *
     *      @param [in] path    Location path of the item file.
     *      @param [in] vlnv    VLNV of the item.
     *      @param [in] locked  Value for separating exportable items.
     */
    void createItem(const QString& path, const VLNV& vlnv = VLNV(), bool locked = false);

	/*!
     *  Get the size hint of this dialog.
	 *
     *      @return QSize contains the desired size for this dialog.
     */
	virtual QSize sizeHint() const;

    /*!
     *  Get the items to be exported.
     *
     *      @return A list of items to be exported.
     */
    QVector<ObjectSelectionListItem*> getSelectedItems() const;

private slots:

    /*!
     *  Handles the selection of all the items in the list.
     *
     *      @param [in] checkAllState   The new state of the items.
     */
    void onCheckAllChanged(int checkAllState);

    /*!
     *  [Description].
     *
     *      @param [in] currentItem   [Description].
     */
    void onItemClicked(QListWidgetItem* currentItem);

protected:

    /*!
     *  Connect the signals.
     */
    virtual void connectSignals();

    /*!
     *  Enable the ok button.
     */
    void enableOkButton();

    /*!
     *  Disable the ok button.
     */
    void disableOkButton();

    /*!
     *  Construct the intro labels for the dialog.
     *
     *      @param [in] introName   Name of the introduction.
     *      @param [in] introText   Text explaining the use of this selection dialog.
     *
     *      @return QWidget containing the information of the selection dialog.
     */
    QWidget* setupIntroWidget(QString const& introName, QString const& introText) const;

    /*!
     *  Setup the layout for the buttons used in the selection dialog.
     *
     *      @return The layout containing the dialog buttons.
     */
    QHBoxLayout* setupButtonLayout();

    /*!
     *  Get the list widget containing the selectable items.
     *
     *      @return The list widget containing the selectable items.
     */
    QListWidget* getItemList();
    
    /*!
     *  Setup the dialog layout.
     */
    virtual void setupLayout() = 0;

private:

	//! No copying. No assignment.
    ObjectSelectionDialog(const ObjectSelectionDialog& other);
    ObjectSelectionDialog& operator=(const ObjectSelectionDialog& other);

    /*!
     *  Get the icon used by the selected item VLNV.
     *
     *      @param [in] itemVLNV    VLNV of the selected item.
     *
     *      @return Icon corresponding to the selected VLNV.
     */
    QIcon getUsedIcon(const VLNV& itemVLNV) const;

    /*!
     *  Check if an item exists in the list.
     *
     *      @param [in] itemPath    Path of the selected item.
     *      @param [in] itemVLNV    VLNV of the selected item.
     *
     *      @return True, if the item exists in the list, false otherwise.
     */
    bool itemExistsInList(QString const& itemPath, VLNV const& itemVLNV) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Path to the icon used by the selection dialog.
    QString dialogIconPath_;

	//! Ok button to accept the dialog.
	QPushButton okButton_;

	//! Cancel button to reject the dialog.
	QPushButton cancelButton_;

	//! Check box to select/deselect all items.
    QCheckBox checkAllBox_;

    //! List containing the items of the selection dialog.
    QListWidget itemList_;

    //! Icon used by bus definitions.
    QIcon busDefinitionIcon_;

    //! Icon used by abstraction definitions.
    QIcon abstractionDefinition_;

    //! Icon used by components.
    QIcon componentIcon_;

    //! Icon used by designs.
    QIcon designIcon_;

    //! Icon used by design configurations.
    QIcon designConfigurationIcon_;

    //! Icon used by generator chains.
    QIcon generatorChainIcon_;

    //! Icon used by abstractors.
    QIcon abstractorIcon_;

    //! Icon used by catalogues.
    QIcon catalogIcon_;

    //! Icon used by COM definitions.
    QIcon comDefinitionIcon_;

    //! Icon used by API definitions.
    QIcon apiDefinitionIcon_;

    //! Icon used by files.
    QIcon fileIcon_;
};

#endif // OBJECTEXPORTDIALOG_H
