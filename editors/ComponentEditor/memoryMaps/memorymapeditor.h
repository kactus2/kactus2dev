/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapeditor.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPEDITOR_H
#define MEMORYMAPEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/AbstractMemoryMap.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>
#include <QGroupBox>

class EditableTableView;
class MemoryMapModel;
class MemoryMapProxy;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! The editor to edit the address blocks of a single memory map.
//-----------------------------------------------------------------------------
class MemoryMapEditor : public QGroupBox
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] handler                 Pointer to the instance managing the library.
	 *      @param [in] memoryRemap             Pointer to the memory remap being edited.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the parent of this editor.
	 */
	MemoryMapEditor(QSharedPointer<Component> component,
		LibraryInterface* handler, 
        QSharedPointer<AbstractMemoryMap> memoryRemap,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~MemoryMapEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

signals:
    
    /*!
     *  Emitted when the contents of the widget change.
     */
    void contentChanged();

    /*!
     *  Emits an error message for the user.
     *
     *      @param [in] msg     The error message.
     */
    void errorMessage(const QString& msg) const;

    /*!
     *  Emits a notice message for the user.
     *
     *      @param [in] msg   [Description].
     */
    void noticeMessage(const QString& msg) const;

    /*!
     *  Increases the number of references to the given parameter.
     *
     *      @param [in] id  The target parameter.
     */
    void increaseReferences(QString id);

    /*!
     *  Decreases the number of references to the given parameter.
     *
     *      @param [in] id  The target parameter.
     */
    void decreaseReferences(QString id);

    /*!
     *  Informs of an added child.
     *
     *      @param [in] index   The index of the new child.
     */
    void childAdded(int index);

    /*!
     *  Informs of a removed child.
     *
     *      @param [in] index   The index of the removed child.
     */
    void childRemoved(int index);

private:
	
	//! \brief No copying
	MemoryMapEditor(const MemoryMapEditor& other);

	//! \brief No assignment
	MemoryMapEditor& operator=(const MemoryMapEditor& other);

	//! \brief The view to show the details of a memory map.
	EditableTableView* view_;

	//! \brief The proxy that does the sorting of items.
	MemoryMapProxy* proxy_;

	//! \brief The model that manages the items.
	MemoryMapModel* model_;
};

#endif // MEMORYMAPEDITOR_H
