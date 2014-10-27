//-----------------------------------------------------------------------------
// File: ComponentDiffWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Tree widget for displaying differences of two components.
//-----------------------------------------------------------------------------

#ifndef COMPONENTDIFFWIDGET_H
#define COMPONENTDIFFWIDGET_H

#include <QTreeWidget>

#include <wizards/common/IPXactDiff.h>

class Component;

//-----------------------------------------------------------------------------
//! Tree widget for displaying differences of two components.
//-----------------------------------------------------------------------------
class ComponentDiffWidget : public QTreeWidget 
{
    Q_OBJECT
public:

    //! Enumeration of the tree columns.
    enum COLUMNS
    {
        ITEM_NAME = 0,
        CHANGE_ELEMENT,
        PREVIOUS_VALUE,
        UPDATED_VALUE,
        COLUMN_COUNT
    };

	//! The constructor.
	ComponentDiffWidget(QWidget *parent);

	//! The destructor.
	~ComponentDiffWidget();

    /*!
     *  Sets the components to diff in the view.
     *
     *      @param [in] reference   The reference component to compare to.
     *      @param [in] subject     The component to compare against the reference.
     */
    void setComponents(QSharedPointer<const Component> reference, QSharedPointer<const Component> subject);


private:

	// Disable copying.
	ComponentDiffWidget(ComponentDiffWidget const& rhs);
	ComponentDiffWidget& operator=(ComponentDiffWidget const& rhs);
    
    /*!
     *  Checks if a list of diffs shows no change.
     *
     *      @param [in] diffs   The list to check.
     *
     *      @return True, if nothing has changed, otherwise false.
     */
    bool nothingChanged(QList<QSharedPointer<IPXactDiff> > const& diffs);

    /*!
     *  Creates the top level items in the tree.
     *
     *      @param [in] diffs   The diffs to display in the tree.
     *
     *      @return Top level items indexed by their element type.
     */
    QMap<QString, QTreeWidgetItem*> createTopLevelItems(QList<QSharedPointer<IPXactDiff> > const& diffs);

    /*!
     *  Creates a top level item for a given element.
     *
     *      @param [in] elementName   The name of the element.
     *
     *      @return The created item.
     */
    QTreeWidgetItem* createTopLevelItemForElement(QString const& elementName);

    /*!
     *  Adds an item to the tree indicating an added element.
     *
     *      @param [in] name        The name of the added element.
     *      @param [in] parent      The parent item.
     */
    void addAddItem(QString const& name, QTreeWidgetItem* parent);
    
    /*!
     *  Adds an item to the tree indicating a removed element.
     *
     *      @param [in] name        The name of the removed element.
     *      @param [in] parent      The parent item.
     */
    void addRemoveItem(QString const& name, QTreeWidgetItem* parent);

    /*!
     *  Adds an item to the tree indicating a modified element.
     *
     *      @param [in] name        The name of the modified element.
     *      @param [in] parent      The parent item.
     */
    void addModificationItem(QSharedPointer<IPXactDiff> diff, QTreeWidgetItem* parent);

    /*!
     *  Adds an item to the tree indicating an modified element with a single modified sub-element.
     *
     *      @param [in] name            The name of the modified element.
     *      @param [in] modification    The modification to the element.
     *      @param [in] parent          The parent item.
     */
    void addSingleLevelModificationItem(QString const& name, IPXactDiff::Modification const& modification, 
        QTreeWidgetItem* parent);

    /*!
     *  Creates an item to indicate a modified sub-element.
     *
     *      @param [in] modification    The modification to the element.
     *      @param [in] parent          The parent item.

     *
     *      @return Item for a modified sub-element.
     */
    QTreeWidgetItem* createModificationItem(IPXactDiff::Modification const& modification,
        QTreeWidgetItem* parent = 0) const;

    /*!
     *  Adds an item to the tree indicating an modified element with multiple modified sub-elements.
     *
     *      @param [in] name            The name of the modified element.
     *      @param [in] changelist      The modifications to the element.
     *      @param [in] parent          The parent item.
     */
    void addMultiLevelModificationItem(QString const& name, QList<IPXactDiff::Modification> changelist, 
        QTreeWidgetItem* parent);

};

#endif // COMPONENTDIFFWIDGET_H
