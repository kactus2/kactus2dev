//-----------------------------------------------------------------------------
// File: designwidget.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWDesignWidget is used to edit and view one design
//-----------------------------------------------------------------------------

#ifndef HWDESIGNWIDGET_H
#define HWDESIGNWIDGET_H

#include <IPXACTmodels/common/VLNV.h>

#include "HWDesignDiagram.h"

#include <editors/common/DesignWidget.h>

#include <QString>
#include <QKeyEvent>
#include <QSharedPointer>

class LibraryInterface;
class VLNV;
class Component;
class HWComponentItem;
class ActiveBusInterfaceItem;
class DesignConfiguration;
class ExpressionParser;
class MultipleParameterFinder;

//-----------------------------------------------------------------------------
//! HWDesignWidget is used to edit and view one design
//-----------------------------------------------------------------------------
class HWDesignWidget : public DesignWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] lh                                  The library interface.
     *      @param [in] designAndInstancesParameterFinder   Parameter finder for design parameter reference tree.
     *      @param [in] designParameterFinder               Design parameter finder.
     *      @param [in] parent                              Owner of this object.
     */
    HWDesignWidget(LibraryInterface *lh, QSharedPointer<MultipleParameterFinder> designAndIsntancesParameterFinder,
        QSharedPointer<ListParameterFinder> designParameterFinder, QWidget *parent = 0);

	/*!
     *  The destructor.
     */
	virtual ~HWDesignWidget() = default;

    //! No mutants allowed.
    HWDesignWidget(const HWDesignWidget&) = delete;
    HWDesignWidget& operator=(const HWDesignWidget&) = delete;

	/*!
     *  Set the design to be displayed in the block diagram
	 *
	 *      @param [in] vlnv        VLNV of the design to open
	 *      @param [in] viewName    Name of the view to open within component.
	 */
	bool setDesign(VLNV const& vlnv, QString const& viewName) final;
    
    /*!
     *  Called when a new column is requested to be added to the design.
     */
    void addColumn() final;

    /*!
     *  Returns the supported draw modes.
     */
    unsigned int getSupportedDrawModes() const final;

    /*!
     *  Returns the implementation attribute.
     */
    KactusAttribute::Implementation getImplementation() const final;

signals:
    
    /*!
     *  Emitted when user wants to refresh this widget
	 *
	 *      @param [in] designWidget    Pointer to this HWDesignWidget instance
	 */
	void refresh(const HWDesignWidget* designWidget);

public slots:
    
    //! Saves the design as new object.
	bool saveAs() final;

    /*!
     *  Handler for item deletion in the design diagram.
     */
    void onDeleteSelectedItems() final;

protected:
    
    /*!
     *  Handler for key press event.
     *
     *      @param [in] event   The selected key event.
     */
    void keyPressEvent(QKeyEvent *event);

private:


    /*!
     *  Delete the selected component items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedComponentItems(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected bus interface items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedBusInterfaceItems(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected bus port items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedBusPortItems(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected HW connection items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedHWConnectionItems(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected ad hoc connection items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedAdHocConnectionItems(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected HW column items.
     *
     *      @param [in] selectedItems   The selected graphics items.
     */
    void deleteSelectedHWColumns(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected ad hoc interface items. Only invalid interfaces can be deleted.
     *
     *      @param [in] selectedItems   A list of the selected items.
     */
    void deleteSelectedAdhocInterfaces(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Delete the selected ad hoc port items. Only invalid ports can be deleted.
     *
     *      @param [in] selectedItems   A list of the selected items.
     */
    void deleteSelectedAdHocPorts(QList<QGraphicsItem*> selectedItems);

    /*!
     *  Create an empty hierarchical component.
     *
     *      @return Pointer to the created component.
     */
    QSharedPointer<Component> createEmptyDesign(VLNV const& prevlnv);

    /*!
     *  Set a design to be edited.
     *
     *      @param [in] comp        The selected component.
     *      @param [in] viewName    The name of the view containing the design.
     *
     *      @return True, if the design is set correctly, false if an error occurred.
     */
    bool setDesign(QSharedPointer<Component> comp, const QString& viewName);

	/*!
     *  Create an empty design and configuration for a component.
	 *
	 *      @param [in] component   Pointer to the component to create the design for.
	 *      @param [in] dirPath     File path to the directory to create the design and configuration to.
	 */
	void createDesignForComponent(QSharedPointer<Component> component, const QString& dirPath);

    /*!
     *  Update the files.
     *
     *      @param [in] topComponent        The top component of the design.
     *      @param [in] sourcePath          The source path.
     *      @param [in] targetPath          The target directory.
     */
    void updateFiles(QSharedPointer<Component> topComponent, QString const& sourcePath,
        QString const& targetPath) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser for the top component.
    QSharedPointer<ExpressionParser> expressionParser_ = nullptr;
};

#endif // HWDESIGNWIDGET_H
