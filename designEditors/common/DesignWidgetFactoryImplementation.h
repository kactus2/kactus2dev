//-----------------------------------------------------------------------------
// File: DesignWidgetFactoryImplementation.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.02.2015
//
// Description:
// Class for design widget factory.
//-----------------------------------------------------------------------------

#ifndef DESIGNWIDGETFACTORYIMPLEMENTATION_H
#define DESIGNWIDGETFACTORYIMPLEMENTATION_H

#include "DesignWidgetFactory.h"

#include <designEditors/common/DesignWidget.h>

#include <library/LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
//! Class for constructing design widgets.
//-----------------------------------------------------------------------------
class DesignWidgetFactoryImplementation : public DesignWidgetFactory
{

public:
    /*!
     *  Constructor.
     */
    DesignWidgetFactoryImplementation(LibraryInterface* libInterface);

    /*!
     *  Destructor.
     */
    ~DesignWidgetFactoryImplementation();

    /*!
     *  Make the hardware design widget.
     */
    virtual DesignWidget* makeHWDesignWidget(QWidget* parent = 0);

private:
    // Disable copying.
    DesignWidgetFactoryImplementation(DesignWidgetFactoryImplementation const& rhs);
    DesignWidgetFactoryImplementation& operator=(DesignWidgetFactoryImplementation const& rhs);

    LibraryInterface* libInterface_;
};

#endif // DESIGNWIDGETFACTORYIMPLEMENTATION_H
