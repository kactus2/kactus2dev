//-----------------------------------------------------------------------------
// File: ComponentWizardPages.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.09.2014
//
// Description:
// Component wizard page enumerations.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDPAGES_H
#define COMPONENTWIZARDPAGES_H


namespace ComponentWizardPages
{
    //! Enumeration for component wizard pages.
    enum Page
    {
        INTRO = 0,
        GENERAL,        
        FILES,
        DEPENDENCY,
        VHDL_IMPORT,
        CONCLUSION
	 };

};

#endif // COMPONENTWIZARDPAGES_H
