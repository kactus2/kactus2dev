//-----------------------------------------------------------------------------
// File: ParameterizableItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.10.2017
//
// Description:
// Component editor item for handling parameter signals.
//-----------------------------------------------------------------------------

#ifndef PARAMETERIZABLEITEM_H
#define PARAMETERIZABLEITEM_H

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <editors/ComponentEditor/referenceCounter/ReferenceCounter.h>
#include <editors/ComponentEditor/treeStructure/componenteditoritem.h>

#include <QObject>

class LibraryInterface;
class ComponentEditorTreeModel;
class Component;

//-----------------------------------------------------------------------------
//! Component editor item for handling parameter signals.
//-----------------------------------------------------------------------------
class ParameterizableItem : public ComponentEditorItem
{
    Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] model       Pointer to the model that owns the items.
	 *    @param [in] libHandler  Pointer to the instance that manages the library.
	 *    @param [in] component   Pointer to the component being edited.
	 *    @param [in] parent      Pointer to the parent item.
	 */
	ParameterizableItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        QSharedPointer<Component> component, ComponentEditorItem* parent);
	
	/*!
     *  The constructor for root item.
	 *
	 *    @param [in] libHandler  Pointer to the instance that manages the library.
	 *    @param [in] component   Pointer to the component being edited.
	 *    @param [in] parent      Pointer to the parent object.
	 */
	ParameterizableItem(LibraryInterface* libHandler, QSharedPointer<Component> component,
        ComponentEditorTreeModel* parent);

	/*!
     *  The destructor.
     */
    virtual ~ParameterizableItem();

protected:

    /*!
     *  Connects this component editor items item editor to its reference counter.
     */
    virtual void connectItemEditorToReferenceCounter();

    /*!
     *  Connect the signals related to a reference tree.
     */
    void connectReferenceTree();

private:

	//! No copying or assignment.
	ParameterizableItem(const ParameterizableItem& other);
	ParameterizableItem& operator=(const ParameterizableItem& other);
};

#endif // PARAMETERIZABLEITEM_H
