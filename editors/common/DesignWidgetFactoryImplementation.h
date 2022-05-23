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

#include <editors/common/DesignWidget.h>

#include <KactusAPI/include/LibraryInterface.h>

class MultipleParameterFinder;
class ListParameterFinder;

//-----------------------------------------------------------------------------
//! Class for constructing design widgets.
//-----------------------------------------------------------------------------
class DesignWidgetFactoryImplementation : public DesignWidgetFactory
{

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface                        Enables access to the library.
     *      @param [in] designAndInstanceParameterFinder    Parameter finder for design parameter reference tree.
     *      @param [in] designParameterFinder               Locates the design parameters.
     */
    DesignWidgetFactoryImplementation(LibraryInterface* libInterface,
        QSharedPointer<MultipleParameterFinder> designAndInstanceParameterFinder,
        QSharedPointer<ListParameterFinder> designParameterFinder);

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

    //! Parameter finder for design parameter reference tree.
    QSharedPointer<MultipleParameterFinder> designAndInstanceParameterFinder_;

    //! Locates the design parameters.
    QSharedPointer<ListParameterFinder> designParameterFinder_;
};

#endif // DESIGNWIDGETFACTORYIMPLEMENTATION_H
