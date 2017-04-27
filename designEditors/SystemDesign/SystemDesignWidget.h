//-----------------------------------------------------------------------------
// File: SystemDesignWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.4.2012
//
// Description:
// System design widget.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDESIGNWIDGET_H
#define SYSTEMDESIGNWIDGET_H

#include <designEditors/common/DesignWidget.h>

class SWConnectionEndpoint;

//-----------------------------------------------------------------------------
//! SystemDesignWidget class.
//-----------------------------------------------------------------------------
class SystemDesignWidget : public DesignWidget
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    SystemDesignWidget(bool onlySW, LibraryInterface* lh, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~SystemDesignWidget();

    /*!
     *  Sets the system identified with the given VLNV as the currently edited one.
     *
     *      @return False if there were errors opening the system; otherwise true.
     */
    bool setDesign(VLNV const& vlnv, QString const& viewName);
    
    /*!
     *  Returns the supported draw modes.
     */
    virtual unsigned int getSupportedDrawModes() const;

    /*!
     *  Returns the implementation attribute.
     */
    virtual KactusAttribute::Implementation getImplementation() const;

    /*!
     *  Get the vlnv that can be used to identify the document.
     *
     *      @return The VLNV that identifies the document.
     */
    virtual VLNV getIdentifyingVLNV() const;

public slots:
    //! Adds a new column to the diagram.
    virtual void addColumn();

    //! Saves the design as new object.
    virtual bool saveAs();

    /*!
     *  Handler for item deletion in the design diagram.
     */
    virtual void onDeleteSelectedItems();

protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual bool setDesign(QSharedPointer<Component> comp, const QString& viewName);

private:
    // Disable copying.
    SystemDesignWidget(SystemDesignWidget const& rhs);
    SystemDesignWidget& operator=(SystemDesignWidget const& rhs);

    /*!
     *  Delete the selected system columns items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedSystemColumns(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected SW component items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedSWComponentItems(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected SW port items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedSWPortItems(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected SW interface items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedSWInterfaceItems(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected COM connection items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedComConnectionItems(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected API connection items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedApiConnectionItems(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected graphics connection items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedGraphicsConnectionItems(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the connected end point.
     *
     *      @param [in] endPoint        The selected end point.
     *      @param [in] parentCommand   The parent command.
     */
    void deleteConnectedEndPoint(SWConnectionEndpoint* endPoint, QSharedPointer<QUndoCommand> parentCommand);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! If true, the widget is used for editing SW designs.
    bool onlySW_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDESIGNWIDGET_H
