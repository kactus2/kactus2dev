//-----------------------------------------------------------------------------
// File: DesignWidgetFactoryMock.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.02.2015
//
// Description:
// Mock class for design widget factory.
//-----------------------------------------------------------------------------

#ifndef DESIGNWIDGETFACTORYMOCK_H
#define DESIGNWIDGETFACTORYMOCK_H

#include <editors/common/DesignWidgetFactory.h>
#include <editors/common/DesignWidget.h>

#include <KactusAPI/include/LibraryInterface.h>

//-----------------------------------------------------------------------------
//! Mock class for constructing design widgets.
//-----------------------------------------------------------------------------
class DesignWidgetFactoryMock : public DesignWidgetFactory
{

public:
    /*!
     *  Constructor.
     */
    DesignWidgetFactoryMock(LibraryInterface* libInterface);

    /*!
     *  Destructor.
     */
    ~DesignWidgetFactoryMock();

    /*!
     *  Make the hardware design widget.
     */
    virtual DesignWidget* makeHWDesignWidget(QWidget* parent /* = 0 */);

private:
    // Disable copying.
    DesignWidgetFactoryMock(DesignWidgetFactoryMock const& rhs);
    DesignWidgetFactoryMock& operator=(DesignWidgetFactoryMock const& rhs);

    LibraryInterface* libInterface_;
};

#endif // DESIGNWIDGETFACTORYIMPLEMENTATION_H
