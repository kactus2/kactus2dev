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

#include <QSharedPointer>
#include <QTextStream>
#include <QList>

class ExpressionFormatter;
class AddressBlock;
class LibraryInterface;

//-----------------------------------------------------------------------------
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------
class HDLComponentParser
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               The component to write to Verilog.
     *      @param [in] activeView              The active view for the component.
	 */
	HDLComponentParser(LibraryInterface* library, QSharedPointer<Component> component);

	//! The destructor.
	~HDLComponentParser();
    
    /*!
    *  Parses the component using the given view.
    */
    void parseComponent(QSharedPointer<View> activeView);

    /*!
    *  Returns the parsed component.
    */
    QSharedPointer<GenerationComponent> getParsedComponent();

    /*!
     *  Sorts list of parameters based on their interdependencies.
     *
     *      @param [in] refParameters		    The list containing the parameters, to be used as a reference.
     *      @param [out] sortParameters         The list containing the same parameters as in refParameters, that will be sorted.
     */
	static void sortParameters(QList<QSharedPointer<Parameter> >& refParameters,
		QList<QSharedPointer<Parameter> >& sortParameters);

private:
	// Disable copying.
	HDLComponentParser(HDLComponentParser const& rhs);
    HDLComponentParser& operator=(HDLComponentParser const& rhs);

    /*!
    *  Culls and formats parameter declarations for the component.
    */
    void findParameters();
    
    /*!
    *  Culls the interfaces of the component.
    */
    void parseInterfaces();
    
    /*!
    *  Culls, sorts and formats the ports of the component.
    */
    void parsePorts();
    
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

    //! The component library.
    LibraryInterface* library_;

    //! The component parsed for generation.
    QSharedPointer<Component> component_;

    //! The parsing result of component_.
    QSharedPointer<GenerationComponent> retval_;

    //! The active view of the component.
    QSharedPointer<View> activeView_;

    //! The component instantiation referred by the active view.
    QSharedPointer<ComponentInstantiation> activeInstantiation_;

    //! The formatter for expressions.
    QSharedPointer<ExpressionFormatter> formatter_;

    //! The interfaces utilized by the component.
    QMap<QSharedPointer<BusInterface>, QSharedPointer<GenerationInterface> > interfaces_;
};

#endif // HDLCOMPONENTPARSER_H
