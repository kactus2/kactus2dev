//-----------------------------------------------------------------------------
// File: DesignWidgetFactoryImplementation.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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

#include <library/LibraryInterface.h>

class MultipleParameterFinder;

//-----------------------------------------------------------------------------
//! Class for constructing design widgets.
//-----------------------------------------------------------------------------
class DesignWidgetFactoryImplementation : public DesignWidgetFactory
{

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface                    Enables access to the library.
     *      @param [in] designParameterReferenceFinder  Parameter finder for design parameter references.
     */
    DesignWidgetFactoryImplementation(LibraryInterface* libInterface,
        QSharedPointer<MultipleParameterFinder> designParameterReferenceFinder);

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

    //! Parameter finder for design parameter references.
    QSharedPointer<MultipleParameterFinder> designParameterReferenceFinder_;
};

#endif // DESIGNWIDGETFACTORYIMPLEMENTATION_H
