//-----------------------------------------------------------------------------
// File: MemoryViewGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.04.2016
//
// Description:
// Creates a CSV listing of all slave memories within a design hierarchy.
//-----------------------------------------------------------------------------

#ifndef MEMORYVIEWGENERATOR_H
#define MEMORYVIEWGENERATOR_H

#include <QList>
#include <QString>
#include <QSharedPointer>

#include "memoryviewgenerator_global.h"

#include <designEditors/MemoryDesigner/ConnectivityGraphFactory.h>

#include <IPXACTmodels/Component/BusInterface.h>

#include <IPXACTmodels/Design/ActiveInterface.h>
#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/generaldeclarations.h>

class AddressBlock;
class BusInterface;
class Component;
class ExpressionParser;
class Field;
class ConnectivityInterface;
class LibraryInterface;
class MemoryMap;
class MemoryItem;
class Register;

//-----------------------------------------------------------------------------
//! Creates a CSV listing of all slave memories within a design hierarchy.
//-----------------------------------------------------------------------------
class MEMORYVIEWGENERATOR_EXPORT MemoryViewGenerator
{

public:
    
    //! The constructor.
    MemoryViewGenerator(LibraryInterface* library);

    //! The destructor.
    ~MemoryViewGenerator();

    /*!
     *  Generates slave memory listing for the given component.
     *
     *      @param [in] topComponent    The top component in the hierarchy to generate listing for.
     *      @param [in] outputPath      Path to the output file.
     */
    void generate(QSharedPointer<Component> topComponent, QString const& activeView, QString const& outputPath);
  
private:

    // Disable copying.
    MemoryViewGenerator(MemoryViewGenerator const& rhs);
    MemoryViewGenerator& operator=(MemoryViewGenerator const& rhs);

    VLNV getConfigurationOrDesign(QSharedPointer<Component> component, QString const& activeView);

  
    /*!
     *  Writes the listing into a given file.
     *
     *      @param [in] outputPath   The path to the output file.
     */
    void writeFile(QString const& outputPath, QVector<QVector<QSharedPointer<ConnectivityInterface> > >  masterRoutes);

    /*!
     *  Writes the given memory element into output.
     *
     *      @param [in] item            The memory element to write.
     *      @param [in] addressOffset   Offset for the memory item.
     *      @param [in] outputStream    The output to write the element into.
     */
    void writeItem(QSharedPointer<MemoryItem> item, int addressOffset, QTextStream& outputStream);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The available IP-XACT library.
    LibraryInterface* library_;

    //!
    ConnectivityGraphFactory locator_;

    //! Parser for resolving expressions.
    ExpressionParser* expressionParser_;
};

#endif // MEMORYVIEWGENERATOR_H
