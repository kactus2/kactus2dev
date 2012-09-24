//-----------------------------------------------------------------------------
// File: MemoryDesignWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.9.2012
//
// Description:
// Declares the memory design widget class.
//-----------------------------------------------------------------------------

#ifndef MEMORYDESIGNWIDGET_H
#define MEMORYDESIGNWIDGET_H

#include <common/DesignWidget.h>

class MemoryDesignDiagram;
class MainWindow;

//-----------------------------------------------------------------------------
//! MemoryDesignWidget class.
//-----------------------------------------------------------------------------
class MemoryDesignWidget : public DesignWidget
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    MemoryDesignWidget(LibraryInterface* lh, MainWindow* mainWnd, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~MemoryDesignWidget();

    /*!
     *  Sets the design identified with the given VLNV as the currently edited one.
     *
     *      @return False if there were errors opening the design; otherwise true.
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

public slots:
    //! Adds a new column to the diagram.
    virtual void addColumn();

    //! \brief Saves the design as new object
    virtual bool saveAs();

protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual bool setDesign(QSharedPointer<Component> comp, const QString& viewName);

private:
    // Disable copying.
    MemoryDesignWidget(MemoryDesignWidget const& rhs);
    MemoryDesignWidget& operator=(MemoryDesignWidget const& rhs);
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNWIDGET_H
