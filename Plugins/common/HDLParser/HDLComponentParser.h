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

    void parseComponent(QSharedPointer<View> activeView);

    QSharedPointer<GenerationComponent> getParsedComponent(){return retval_;}

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

    void parseInterfaces(QSharedPointer<GenerationComponent> retval);

    void parsePorts(QSharedPointer<GenerationComponent> retval);
    
	void parseMemoryMaps(QSharedPointer<GenerationComponent> target) const;

    void parseAddressBlock(QSharedPointer<AddressBlock> ab, QSharedPointer<GenerationRemap> target) const;

     /*!
      *  Culls and formats parameter declarations for the module.
      */
    void findParameters(QSharedPointer<GenerationComponent> target) const;

    void parseRemapStates(QSharedPointer<GenerationComponent> target) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component library.
    LibraryInterface* library_;

    //! The componet parsed for generation.
    QSharedPointer<GenerationComponent> retval_;

    //! The component active view.
    QSharedPointer<View> activeView_;

    //! The formatter for expressions.
    QSharedPointer<ExpressionFormatter> formatter_;

    //! The interfaces utilized by the component.
    QMap<QSharedPointer<BusInterface>, QSharedPointer<GenerationInterface> > interfaces_;
};

#endif // HDLCOMPONENTPARSER_H
