//-----------------------------------------------------------------------------
// File: CpuVisualizer.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.09.2023
//
// Description:
// The visualizer to display the regions of a CPU.
//-----------------------------------------------------------------------------

#ifndef CPUVISUALIZER_H
#define CPUVISUALIZER_H

#include <editors/ComponentEditor/itemvisualizer.h>
#include <editors/ComponentEditor/cpus/RegionScene.h>

#include <QSharedPointer>
#include <QGraphicsView>
#include <QSize>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The visualizer to display the regions of a CPU.
//-----------------------------------------------------------------------------
class CpuVisualizer : public ItemVisualizer
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *    @param [in] cpu				The CPU being visualized.
     *    @param [in] memoryMaps			The available memory maps for the CPU to reference.
     *    @param [in] expressionParser   The expression parser to use.
	 *    @param [in] parent             The owner of the visualizer.
	 */
    CpuVisualizer(QSharedPointer<Cpu> cpu,
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
        QSharedPointer<ExpressionParser> expressionParser,
		QWidget *parent = 0);
	
	//! The destructor.
	virtual ~CpuVisualizer() = default;

    //! No copying.
    CpuVisualizer(const CpuVisualizer& other) = delete;

    //! No assignment.
	CpuVisualizer& operator=(const CpuVisualizer& other) = delete;

	//! Update the contents of the visualizer.
	void refresh();

	/*!
     *  The minimum size hint for the visualizer.
	 *
	 *    @return The minimum size hint.
	 */
	QSize minimumSizeHint() const override;

	/*!
     *  The size hint for the visualizer.
	 *
	 *    @return The size hint.
	 */
	QSize sizeHint() const override;

protected:

    //! Handler for showing the widget.
    void showEvent(QShowEvent* event) override;

private:
	
	//! The cpu being visualized.
	QSharedPointer<Cpu> cpu_;

	//! The view displaying the visualization items.
	QGraphicsView* view_;

	//! The scene that manages the visualization items.
	RegionScene* scene_;
};

#endif // CPUVISUALIZER_H
