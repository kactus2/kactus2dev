//-----------------------------------------------------------------------------
// File: designwidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

#include <designEditors/common/DesignWidget.h>

#include <QString>
#include <QKeyEvent>
#include <QSharedPointer>

class LibraryInterface;
class VLNV;
class Component;
class HWComponentItem;
class BusPortItem;
class DesignConfiguration;
class ExpressionParser;

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
     *      @param [in] lh      The library interface.
     *      @param [in] parent  Owner of this object.
     */
    HWDesignWidget(LibraryInterface *lh, QWidget *parent = 0);

	/*!
     *  The destructor.
     */
	virtual ~HWDesignWidget();

	/*!
     *  Set the design to be displayed in the block diagram
	 *
	 *      @param [in] vlnv        VLNV of the design to open
	 *      @param [in] viewName    Name of the view to open within component.
	 */
	virtual bool setDesign(VLNV const& vlnv, QString const& viewName);
    
    /*!
     *  Called when a new column is requested to be added to the design.
     */
    virtual void addColumn();

    /*!
     *  Returns the supported draw modes.
     */
    virtual unsigned int getSupportedDrawModes() const;

    /*!
     *  Returns the implementation attribute.
     */
    virtual KactusAttribute::Implementation getImplementation() const;

signals:
    
    /*!
     *  Emitted when user wants to refresh this widget
	 *
	 *      @param [in] designWidget    Pointer to this HWDesignWidget instance
	 */
	void refresh(const HWDesignWidget* designWidget);

public slots:
    
    //! Saves the design as new object.
	virtual bool saveAs();

	//! Called when user clicks the generate top-vhdl icon.
	void onVhdlGenerate();
    QString findEntityName() const;

protected:
    
    /*!
     *  Handler for key press event.
     *
     *      @param [in] event   The selected key event.
     */
    void keyPressEvent(QKeyEvent *event);

private:

    //! No mutants allowed.
    HWDesignWidget(const HWDesignWidget &);
    HWDesignWidget &operator=(const HWDesignWidget &);

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
     *      @param [in] targetDirectory     The target directory.
     */
    void updateFiles(QSharedPointer<Component> topComponent, QString const& sourcePath,
        QString const& targetDirectory) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser for the top component.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // HWDESIGNWIDGET_H
