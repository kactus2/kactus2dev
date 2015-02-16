//-----------------------------------------------------------------------------
// File: DesignWidgetFactoryMock.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.02.2015
//
// Description:
// Mock class for design widget factory.
//-----------------------------------------------------------------------------

#include "DesignWidgetFactoryMock.h"
#include "HWDesignWidgetMock.h"

//-----------------------------------------------------------------------------
// Function: DesignWidgetFactoryMock::DesignWidgetFactoryMock()
//-----------------------------------------------------------------------------
DesignWidgetFactoryMock::DesignWidgetFactoryMock(LibraryInterface* libInterface):
libInterface_(libInterface)
{

}

//-----------------------------------------------------------------------------
// Function: DesignWidgetFactoryMock::DesignWidgetFactoryMock()
//-----------------------------------------------------------------------------
DesignWidgetFactoryMock::~DesignWidgetFactoryMock()
{

}

//-----------------------------------------------------------------------------
// Function: DesignWidgetFactoryMock::makeHWDesignWidget()
//-----------------------------------------------------------------------------
DesignWidget* DesignWidgetFactoryMock::makeHWDesignWidget(QWidget* parent )
{
    DesignWidget* hwMockDesign (new HWDesignWidgetMock(libInterface_, parent));

    return hwMockDesign;
}
