//-----------------------------------------------------------------------------
// File: ImportInstancesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.04.2019
//
// Description:
// Delegate that provides editing for imported instances.
//-----------------------------------------------------------------------------

#ifndef IMPORTINSTANCSEDELEGATE_H
#define IMPORTINSTANCSEDELEGATE_H

#include <wizards/ComponentWizard/InstanceData.h>

#include <QStyledItemDelegate>
#include <QComboBox>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Delegate that provides editing for imported instances.
//-----------------------------------------------------------------------------
class ImportInstancesDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] workingComponent    The imported component.
     *      @param [in] defaultPath         The default library path.
     *      @param [in] libraryAccess       The library interface.
     *      @param [in] parent              The parent of the object.
	 */
    ImportInstancesDelegate(QSharedPointer<const Component> workingComponent, QString const& defaultPath,
        LibraryInterface* libraryAccess, QObject* parent = 0);

	/*!
     *  The destructor.
     */
    ~ImportInstancesDelegate() = default;

    /*!
     *  Setup the options for the VLNV editor.
     *
     *      @param [in] instanceComponentMatches    The possible component modules for each component instance.
     *      @param [in] newInstances                List of imported component instances.
     */
    void setupVLNVEditors(QMultiMap< QString, QSharedPointer<const Component> > instanceComponentMatches,
        QVector<InstanceData::instanceData> const& newInstances);

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent   Owner for the editor.
	 *      @param [in] option   Contains options for the editor.
	 *      @param [in] index    Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
     */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *     @param [in] editor   Pointer to the editor that contains the data to store.
	 *     @param [in] model    Model that contains the data structure where data is to be saved to.
	 *     @param [in] index    Model index identifying the item that's data is to be saved.
     */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;


    //! No copying. No assignment.
    ImportInstancesDelegate(const ImportInstancesDelegate& other) = delete;
    ImportInstancesDelegate& operator=(const ImportInstancesDelegate& other) = delete;

signals:

    /*!
     *  Change the status of the indexed item.
     *
     *      @param [in] index       Index of the selected item.
     *      @param [in] isEditable  New status of the indexed item.
     */
    void changeItemEditableStatus(QModelIndex const& index, bool isEditable) const;

    /*!
     *  Check if the selected path is located in a new library location.
     *
     *      @param [in] newPath     The selected path.
     */
    void checkForNewLibraryLocation(QString const& newPath) const;

    /*!
     *  Change the tooltip of the indexed item.
     *
     *      @param [in] index       The index of the selected item.
     *      @param [in] newText     The new tooltip.
     */
    void changeIndexItemTooltip(QModelIndex const& index, QString const& newText) const;

private:

    /*!
     *  Create the selection widget for VLNVs.
     *
     *      @param [in] selectionIndex              Index of the item selector.
     *      @param [in] parent                      Parent item of the editor.
     *
     *      @return Combobox for VLNVs.
     */
    QComboBox* createVLNVSelector(QModelIndex const& selectionIndex, QWidget* parent) const;

    /*!
     *  Set a new XML path for the selected VLNV.
     *
     *      @param [in] vlnvIndex       Index of the VLNV.
     *      @param [in] newVLNVText     The VLNV in string.
     *      @param [in] model           Model containing the data.
     */
    void setNewXMLPath(QModelIndex const& vlnvIndex, QString const& newVLNVText, QAbstractItemModel* model) const;

    /*!
     *  Get the name of the instanced module.
     *
     *      @param [in] instanceName    Name of the selected component instance.
     */
    QString getModuleNameForInstance(QString const& instanceName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of possible components for each component instance.
    QMultiMap<QString, QSharedPointer<const Component> > instanceComponentMatches_;

    //! List of instanced components.
    QVector<InstanceData::instanceData> instanceData_;

    //! The imported component.
    QSharedPointer<const Component> workingComponent_;

    //! The default library path.
    QString defaultPath_;

    //! The library interface.
    LibraryInterface* libraryAccess_;
};

#endif // AUTOCONNECTORCONNECTIONDELEGATE_H
