//-----------------------------------------------------------------------------
// File: PortEditorAdapter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.09.2014
//
// Description:
// Adapter for PortsEditor to implement PortsVisualizer. 
//-----------------------------------------------------------------------------

#ifndef PORTEDITORADAPTER_H
#define PORTEDITORADAPTER_H

#include <Plugins/PluginSystem/ImportPlugin/PortVisualizer.h>

class Port;
class PortsEditor;

//-----------------------------------------------------------------------------
//! Adapter for PortsEditor to implement PortsVisualizer. 
//-----------------------------------------------------------------------------
class PortEditorAdapter : public PortVisualizer
{
public:
    
    /*!
     *  The constructor.
     *
     *      @param [in] editor   The editor used for visualization.
     */
    PortEditorAdapter(PortsEditor* editor);

    //! The destructor.
    ~PortEditorAdapter();

    /*!
     *  Called when a port should be added to the editor.
     *
     *      @param [in] portToAdd   The port to add.
     */
    virtual void addPort(QSharedPointer<Port> portToAdd);
    
    /*!
     *  Called when a port should be removed from the editor.
     *
     *      @param [in] portToRemove   The port to remove.
     */
    virtual void removePort(QSharedPointer<Port> portToRemove);

private:
    
    //! The editor used for visualization.
    PortsEditor* editor_;
};

#endif // PORTEDITORADAPTER_H
