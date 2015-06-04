//-----------------------------------------------------------------------------
// File: SystemDesignWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.4.2012
//
// Description:
// System design widget.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDESIGNWIDGET_H
#define SYSTEMDESIGNWIDGET_H

#include <designEditors/common/DesignWidget.h>

class SystemDesignDiagram;

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

    //! \brief Saves the design as new object
    virtual bool saveAs();

protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual bool setDesign(QSharedPointer<Component> comp, const QString& viewName);

private:
    // Disable copying.
    SystemDesignWidget(SystemDesignWidget const& rhs);
    SystemDesignWidget& operator=(SystemDesignWidget const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! If true, the widget is used for editing SW designs.
    bool onlySW_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDESIGNWIDGET_H
