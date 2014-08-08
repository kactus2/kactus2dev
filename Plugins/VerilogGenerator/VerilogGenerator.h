//-----------------------------------------------------------------------------
// File: VerilogGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Verilog file generator.
//-----------------------------------------------------------------------------

#ifndef VERILOGGENERATOR_H
#define VERILOGGENERATOR_H


#include <QTextStream>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/design.h>

#include <library/LibraryManager/libraryinterface.h>

#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>
#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>
#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>

//-----------------------------------------------------------------------------
// Verilog file generator.
//-----------------------------------------------------------------------------
class VerilogGenerator
{
public:
    //! The constructor.
    VerilogGenerator(LibraryInterface* library);

    //! The destructor.
    ~VerilogGenerator();

    /*!
     *  Generates the component Verilog to a given file.
     *
     *      @param [in] outputPath   The path to the output file.
     *
     *      @remark If parse() is not called before generate(), nothing is generated.
     */
    void generate(QString const& outputPath) const;

    /*!
     *  Parses a given component for generation.
     *
     *      @param [in] component   The component to parse for generation.
     *
     *      @remark If parse() is not called before generate(), nothing is generated.
     */
    void parse(QSharedPointer<Component> component, 
        QSharedPointer<Design> design = QSharedPointer<Design>());

private:
    
    /*!
    *  Checks if the generator should write nothing.
    *
    *      @return True, if the generator has nothing to write, otherwise false.
    */
    bool nothingToWrite() const;

        /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    void parseComponentInstances(QSharedPointer<Design> design);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    void parseHierarchicalConnectionsForInstance(QSharedPointer<Component> instanceComponent, 
        ComponentInstanceVerilogWriter* instanceWriter, 
        QSharedPointer<Design> design);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    void mapInterfacesInInstance(QSharedPointer<BusInterface> topIf, QSharedPointer<BusInterface> instanceIf, QSharedPointer<Component> instanceComponent, ComponentInstanceVerilogWriter* instanceWriter);
   
    QPair<General::PortBounds,General::PortBounds> getPhysicalPortBounds(QSharedPointer<General::PortMap> portMap1,
        QSharedPointer<Component> component1, QSharedPointer<General::PortMap> portMap2, QSharedPointer<Component> component2);
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Writer for generating file header.
    VerilogHeaderWriter* headerWriter_;

    //! Writer for top level component.
    ComponentVerilogWriter* topWriter_;

    //! The top level component.
    QSharedPointer<Component> topComponent_;

    QSharedPointer<PortSorter> sorter_;

    LibraryInterface* library_;
};

#endif // VERILOGGENERATOR_H
