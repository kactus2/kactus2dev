/* 
 * 		filename: designwidget.h
 */

#ifndef HWDESIGNWIDGET_H
#define HWDESIGNWIDGET_H

#include <library/LibraryManager/vlnv.h>

#include "HWDesignDiagram.h"

#include <designEditors/common/DesignWidget.h>
#include <common/GenericEditProvider.h>

#include <QMainWindow>
#include <QString>
#include <QGraphicsView>
#include <QAction>
#include <QActionGroup>
#include <QDockWidget>
#include <QComboBox>
#include <QKeyEvent>
#include <QSharedPointer>
#include <QRectF>

class LibraryInterface;
class VLNV;
class Component;
class HWComponentItem;
class BusPortItem;
class DesignConfiguration;

/*! \brief HWDesignWidget is used to edit and view one design
 *
 */
class HWDesignWidget : public DesignWidget
{
    Q_OBJECT

public:

    HWDesignWidget(LibraryInterface *lh, QWidget *parent = 0);

	//! \brief The destructor
	virtual ~HWDesignWidget();

	/*! \brief Set the design to be displayed in the block diagram
	 *
	 * \param vlnv VLNV of the design to open
	 * \param viewName Name of the view to open within component.
	 *
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

    /*! \brief Create a design that represents the current drawn design.
	 *
	 * \param vlnv The vlnv to set for the design.
	 *
	 * \return QSharedPointer<Design> Pointer to the created design.
	*/
	QSharedPointer<Design> createDesign(const VLNV& vlnv);

    /*!
     *  Returns the implementation attribute.
     */
    virtual KactusAttribute::Implementation getImplementation() const;

signals:
    /*! \brief Emitted when user wants to refresh this widget
	 *
	 * \param designWidget Pointer to this HWDesignWidget instance
	 *
	*/
	void refresh(const HWDesignWidget* designWidget);

public slots:
    //! \brief Saves the design as new object
	virtual bool saveAs();

	//! \brief Called when user clicks the generate top-vhdl icon
	void onVhdlGenerate();

	//! \brief Called when user clicks the generate quartus project icon
	void onQuartusGenerate();

	//! \brief Called when user clicks the generate modelsim icon
	void onModelsimGenerate();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    HWDesignWidget(const HWDesignWidget &);
    HWDesignWidget &operator=(const HWDesignWidget &);

    /*! \brief Create an empty hierarchical component
     *
     * \return Pointer to the created component.
    */
    QSharedPointer<Component> createEmptyDesign(VLNV const& prevlnv);

    bool setDesign(QSharedPointer<Component> comp, const QString& viewName);

	/*! \brief Create an empty design and configuration for a component.
	 *
	 * \param component Pointer to the component to create the design for.
	 * \param dirPath File path to the directory to create the design and
	 * configuration to.
	 *
	*/
	void createDesignForComponent(QSharedPointer<Component> component,
		const QString& dirPath);
};

#endif // HWDESIGNWIDGET_H
