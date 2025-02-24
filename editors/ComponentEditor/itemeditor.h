//-----------------------------------------------------------------------------
// File: itemeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 02.02.2011
//
// Description:
// ItemEditor is a base class for editors in Component Editor module..
//-----------------------------------------------------------------------------

#ifndef ITEMEDITOR_H
#define ITEMEDITOR_H

#include <QWidget>

class LibraryInterface;
class Component;
class Extendable;

//-----------------------------------------------------------------------------
//! ItemEditor is a base class for editors in Component Editor module..
// 
// This class should be used as a base class when implementing an element-specific editor.
// This base class provides means to communicate with ComponentTreeNavigator.
//-----------------------------------------------------------------------------
class ItemEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] component   Pointer to the component instance that is being edited.
     *    @param [in] handler     Pointer to the instance that manages the library.
     *    @param [in] parent      Pointer to the owner of this widget.
     */
    ItemEditor(QSharedPointer<Component> component, LibraryInterface* handler, QWidget *parent = 0);

    //! The destructor.
    virtual ~ItemEditor();

    /*!
     *  Allows/disallows the editor change.
     *
     *    @return True, if this editor can be hidden without consequences. Otherwise false.
     */
    virtual bool confirmEditorChange();

    /*!
     *  Reload the information from the model to the editor.
     */
    virtual void refresh() = 0;

    /*!
     *  Sets the protection state of the editor.
     *
     *  The base class implementation enables/disables the editor widget. Override to e.g. enable/disable
     *  individual editor tabs.
     *
     *    @param [in] locked  True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

    //! No copying. No Assignment.
    ItemEditor(const ItemEditor& other) = delete;
    ItemEditor& operator=(const ItemEditor& other) = delete;


signals:

    /*!
     *  Emitted when the contents of the widget change.
      *
     *    @param [in] change  This is set just for convenience so this signal can be connected directly to
     *                          a widget to set it enabled. This should always be set to true.
     */
    void contentChanged();

    /*!
     *  Informs of the necessity to redraw the visualizer.
     */
    void graphicsChanged();

    //! Prints an error message to the user.
    void errorMessage(const QString& msg) const;

    //! Prints a notification to the user.
    void noticeMessage(const QString& msg) const;

    //! Emitted when a help page should be changed in the context help window.
    void helpUrlRequested(QString const& url);

    /*! 
     *  Emitted when the editor adds a new item that should be shown in navigation tree.
     *
     *    @param [in] index   The index of the added item.
     */
    void childAdded(int index);

    /*!
     *  Emitted when the editor removes a child that should also be removed from navigation tree.
     *
     *    @param [in] index   The index of the removed item.
     */
    void childRemoved(int index);

    /*!
     *  Emitted when the editor moves child from one position to another.
     *
     *    @param [in] source  The index of the moved item.
     *    @param [in] target  The index to move the child to.
     */
    void childMoved(int source, int target);

    /*!
     *  Increase the amount of references to a parameter with a matching id.
     *
     *    @param [in] id      Id of the parameter, whose references are being increased.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to a parameter with a matching id.
     *
     *    @param [in] id      Id of the parameter, whose references are being increased.
     */
    void decreaseReferences(QString id);

    /*!
     *  Change the vendor extensions of the vendor extensions editor.
     *
     *    @param [in] containingID    ID for the vendor extensions editor.
     *    @param [in] extensionItem   The item containing the selected vendor extensions.
     */
    void changeVendorExtensions(QString const& containingID, QSharedPointer<Extendable> extensionItem);

protected:

    /*!
     *  Get the the component pointer from the base-class.
     *
     *    @return Pointer to the component model that is being edited
     */
    QSharedPointer<Component> component() const;


    /*!
     *  Get pointer to the instance managing the library.
     *
     *    @return Pointer to the instance managing the library.
     */
    LibraryInterface* handler() const;

private:

    //! A pointer to the component that's element is being edited.
    QSharedPointer<Component> component_;

    //! Pointer to the instance that manages the library.
    LibraryInterface* handler_;
    
};

#endif // ITEMEDITOR_H
