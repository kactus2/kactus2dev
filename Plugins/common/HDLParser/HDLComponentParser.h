//-----------------------------------------------------------------------------
// File: HDLComponentParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 11.08.2016
//
// Description:
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------

#ifndef HDLCOMPONENTPARSER_H
#define HDLCOMPONENTPARSER_H

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/Port.h>

#include <Plugins/common/PortSorter/PortSorter.h>
#include <Plugins/common/HDLParser/HDLParserCommon.h>
#include <Plugins/common/HDLParser/MetaInstance.h>

#include <QSharedPointer>
#include <QTextStream>
#include <QList>

class ExpressionFormatter;
class AddressBlock;
class LibraryInterface;

//-----------------------------------------------------------------------------
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------
class HDLComponentParser : public MetaInstance
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               The component to write to Verilog.
     *      @param [in] activeView              The active view for the component.
	 */
    HDLComponentParser(LibraryInterface* library, QSharedPointer<Component> component,
        QSharedPointer<View> activeView);

	//! The destructor.
	~HDLComponentParser();

    /*!
     *  Sorts list of parameters based on their interdependencies.
     *
     *      @param [in] refParameters		    The list containing the parameters, to be used as a reference.
     *      @param [out] sortParameters         The list containing the same parameters as in refParameters, that will be sorted.
     */
	static void sortParameters(QList<QSharedPointer<Parameter> >& refParameters,
        QList<QSharedPointer<Parameter> >& sortParameters);

    //! The parsed remap states.
    QList<QSharedPointer<GenerationRemapState> > remapStates_;
    //! The parsed remaps, including the default memory map.
    QList<QSharedPointer<GenerationRemap> > remaps_;
    //! The addressable unit bits to be used within the component.
    QString aub_;
    //! Total memory within the component counted in AUBs.
    QString totalRange_;

protected:
    /*!
    *  Parses the found parameter declarations.
    */
    virtual void formatParameters();
    
    /*!
    *  Culls and parses the ports of the component.
    */
    virtual void formatPorts();

private:
	// Disable copying.
	HDLComponentParser(HDLComponentParser const& rhs);
    HDLComponentParser& operator=(HDLComponentParser const& rhs);
    
    /*!
    *  Culls the memory of the component.
    */
	void parseMemoryMaps();
    
    /*!
    *  Parses the relevant data of the given address block.
    */
    void parseAddressBlock(QSharedPointer<AddressBlock> ab, QSharedPointer<GenerationRemap> target);
    
    /*!
    *  Goes through the remap states, connects remaps to correct ports.
    */
    void parseRemapStates();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The formatter for expressions.
    QSharedPointer<ExpressionFormatter> formatter_;
};

#endif // HDLCOMPONENTPARSER_H
