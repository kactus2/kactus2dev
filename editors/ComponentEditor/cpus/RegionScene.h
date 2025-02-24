//-----------------------------------------------------------------------------
// File: RegionScene.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.10.2023
//
// Description:
// The graphics scene containing the regions and referenced memory map of a CPU.
//-----------------------------------------------------------------------------

#ifndef REGIONSCENE_H
#define REGIONSCENE_H


#include <editors/ComponentEditor/common/MemoryAlignScene.h>

#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/MemoryMap.h>

#include <QGraphicsScene>
#include <QSharedPointer>
#include <QMultiMap>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphics scene containing the segments and local memory map of an address space.
//-----------------------------------------------------------------------------
class RegionScene : public MemoryAlignScene
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] addrSpace         The address space being visualized.
     *    @param [in] expressionParser  The expression parser to use for local memory maps.
	 *    @param [in] parent            The owner of the scene.
	 */
    RegionScene(QSharedPointer<Cpu> cpu,
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps, 
        QSharedPointer<ExpressionParser> expressionParser,
		QObject *parent);
	
	//! The destructor.
	virtual ~RegionScene() = default;

    //! No copying.
    RegionScene(const RegionScene& other) = delete;

    //! No assignment.
    RegionScene& operator=(const RegionScene& other) = delete;


protected:

    /*!
     *  Get the width of the visualized memory.
     *
     *    @return The width of the memory.
     */
    QString getWidth() const final;

    /*!
     *  Get the range of the visualized memory.
     *
     *    @return The range of the memory.
     */
    QString getRange() const final;

    /*!
     *  Create the region/segment items placed on left.
     *
     */
    void createRegionItems() final;

    /*!
     *  Create the address block items placed on right.
     *
     */
    void createAddressBlockItems() final;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The cpu whose regions are being visualized.
	QSharedPointer<Cpu> cpu_;


    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps_;

};

#endif // REGIONSCENE_H
