//-----------------------------------------------------------------------------
// File: localmemorymapeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 13.10.2012
//
// Description:
// LocalMemoryMapEditor is used to edit a local memory map of an address space.
//-----------------------------------------------------------------------------

#ifndef LOCALMEMORYMAPEDITOR_H
#define LOCALMEMORYMAPEDITOR_H

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <QGroupBox>
#include <QSharedPointer>
#include <QCheckBox>

class AddressSpace;
class Component;
class EditableTableView;
class LibraryInterface;
class MemoryMapBase;
class MemoryMapModel;
class NameGroupEditor;

//-----------------------------------------------------------------------------
//! LocalMemoryMapEditor is used to edit a local memory map of an address space.
//-----------------------------------------------------------------------------
class LocalMemoryMapEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] memoryMap               Pointer to the local memory map being edited.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] handler                 Pointer to the instance managing the library.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the owner of the editor.
	 */
	LocalMemoryMapEditor(QSharedPointer<AddressSpace> memoryMap,
		QSharedPointer<Component> component,
		LibraryInterface* handler,
        QSharedPointer <ParameterFinder> parameterFinder,
        QSharedPointer <ExpressionFormatter> expressionFormatter,
		QWidget *parent);
	
	//! The destructor.
	virtual ~LocalMemoryMapEditor();

	/*!
     *  Check for the validity of the editor.
	 * 
	 *      @return True if local memory map is valid.
	 */
	bool isValid() const;

	//! Reload the information from the model to the editor.
	void refresh();

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

    //! Emitted when the changes should be reflected in visualization.
    void graphicsChanged();

	//! Emitted when a new memory map item is added to the given index.
	void itemAdded(int index);

	//! Emitted when a memory map item is removed from the given index.
	void itemRemoved(int index);

    //! Prints an error message to the user.
    void errorMessage(const QString& msg) const;

    /*!
     *  Increase the amount of references to a parameter.
     *
     *      @param [in] id  The id of the parameter.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to a parameter.
     *
     *      @param [in] id  The id of the parameter.
     */
    void decreaseReferences(QString id);

public slots:

    /*!
     *  The check state has been changed.
     */
    void onCheckStateChanged();

private:
	
	//! No copying. No assignment.
	LocalMemoryMapEditor(const LocalMemoryMapEditor& other);
	LocalMemoryMapEditor& operator=(const LocalMemoryMapEditor& other);

    //! The address space being edited.
    QSharedPointer<AddressSpace> addressSpace_;

	//! Pointer to the local memory map being edited.
	QSharedPointer<MemoryMapBase> localMemoryMap_;

	//! Used to edit the name, display name and description.
	NameGroupEditor* nameEditor_;

	//! The view to display the table of local memory address blocks
	EditableTableView* view_;

	//! The model that manages the items.
	MemoryMapModel* model_;

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! Pointer to the instance managing the library.
	LibraryInterface* handler_;
};

#endif // LOCALMEMORYMAPEDITOR_H
