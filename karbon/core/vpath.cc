/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>
#include <qpainter.h>
#include <qwmatrix.h>

#include <koPoint.h>
#include <koRect.h>

#include "vfill.h"
#include "vpainter.h"
#include "vpath.h"
#include "vsegment.h"
#include "vstroke.h"
#include "vvisitor.h"

#include <kdebug.h>


VPath::VPath( VObject* parent, VState state )
	: VObject( parent, state )
{
	m_segmentLists.setAutoDelete( true );

	// add an initial segmentlist:
	m_segmentLists.append( new VSegmentList( this ) );

	// we need a stroke for boundingBox() at anytime:
	m_stroke = new VStroke( this );
	m_fill = new VFill();
}

VPath::VPath( const VPath& path )
	: VObject( path )
{
	m_segmentLists.setAutoDelete( true );

	VSegmentList* list;

	QPtrListIterator<VSegmentList> itr( path.m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		list = itr.current()->clone();
		list->setParent( this );
		m_segmentLists.append( list );
	}
}

VPath::~VPath()
{
}

void
VPath::draw( VPainter *painter, const KoRect& rect ) const
{
	if(
		state() == state_deleted ||
		state() == state_hidden ||
		state() == state_hidden_locked )
	{
		return;
	}

	double zoomFactor = painter->zoomFactor();

	if( zoomFactor != 1 && !rect.intersects( boundingBox() ) )
		return;

	painter->save();
	//painter->setZoomFactor( zoomFactor );

	QPtrListIterator<VSegmentList> itr( m_segmentLists );

	if( state() != state_edit )
	{
		// paint fill:
		painter->newPath();
		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				if( jtr.current()->type() == segment_curve  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_curve1  )
					painter->curveTo(
						jtr.current()->knot1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_curve2  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->knot2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_line )
				{
					painter->lineTo( jtr.current()->knot2() );
				}
				else
					painter->moveTo( jtr.current()->knot2() );
			}
		}

		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( Qt::NoPen );
		painter->setBrush( *fill() );
		painter->fillPath();

		// draw stroke:
		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( *stroke() );
		painter->setBrush( Qt::NoBrush );
		painter->strokePath();
	}

	// draw simplistic contour:
	if( state() == state_edit )//|| state() == state_selected )
	{
		for( itr.toFirst(); itr.current(); ++itr )
		{
			painter->newPath();
			painter->setRasterOp( Qt::XorROP );
			painter->setPen( Qt::yellow );
			painter->setBrush( Qt::NoBrush );

			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				if( jtr.current()->type() == segment_curve  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_curve1  )
					painter->curveTo(
						jtr.current()->knot1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_curve2  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->knot2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_line )
				{
					painter->lineTo( jtr.current()->knot2() );
				}
				else
					painter->moveTo( jtr.current()->knot2() );
			}

			painter->strokePath();
		}

		// draw a "knot" at the center:
		const KoPoint center = boundingBox().center();

		painter->newPath();
		painter->setRasterOp( Qt::XorROP );
		painter->setPen( Qt::NoPen );
		painter->setBrush( Qt::yellow );

		painter->moveTo(
			KoPoint(
				center.x() - 2 / zoomFactor,
				center.y() - 2 / zoomFactor ) );
		painter->lineTo(
			KoPoint(
				center.x() + 2 / zoomFactor,
				center.y() - 2 / zoomFactor ) );
		painter->lineTo(
			KoPoint(
				center.x() + 2 / zoomFactor,
				center.y() + 2 / zoomFactor ) );
		painter->lineTo(
			KoPoint(
				center.x() - 2 / zoomFactor,
				center.y() + 2 / zoomFactor ) );
		painter->fillPath();
	}

	// draw small boxes for path nodes:
	if( state() == state_selected || state() == state_edit )
	{
		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				painter->newPath();
				painter->setRasterOp( Qt::NotROP );
				painter->setPen( Qt::NoPen );
				painter->setBrush( Qt::blue.light() );

				painter->moveTo(
					KoPoint(
						jtr.current()->knot2().x() - 2 / zoomFactor,
						jtr.current()->knot2().y() - 2 / zoomFactor ) );
				painter->lineTo(
					KoPoint(
						jtr.current()->knot2().x() + 2 / zoomFactor,
						jtr.current()->knot2().y() - 2 / zoomFactor ) );
				painter->lineTo(
					KoPoint(
						jtr.current()->knot2().x() + 2 / zoomFactor,
						jtr.current()->knot2().y() + 2 / zoomFactor ) );
				painter->lineTo(
					KoPoint(
						jtr.current()->knot2().x() - 2 / zoomFactor,
						jtr.current()->knot2().y() + 2 / zoomFactor ) );
				painter->fillPath();
			}
		}

		// draw a "knot" at the center:
		const KoPoint center = boundingBox().center();

		painter->newPath();
		painter->setRasterOp( Qt::NotROP );
		painter->setPen( Qt::NoPen );
		painter->setBrush( Qt::blue.light() );

		painter->moveTo(
			KoPoint(
				center.x() - 2 / zoomFactor,
				center.y() - 2 / zoomFactor ) );
		painter->lineTo(
			KoPoint(
				center.x() + 2 / zoomFactor,
				center.y() - 2 / zoomFactor ) );
		painter->lineTo(
			KoPoint(
				center.x() + 2 / zoomFactor,
				center.y() + 2 / zoomFactor ) );
		painter->lineTo(
			KoPoint(
				center.x() - 2 / zoomFactor,
				center.y() + 2 / zoomFactor ) );
		painter->fillPath();
	}

	painter->restore();
}

const KoPoint&
VPath::currentPoint() const
{
	return m_segmentLists.getLast()->currentPoint();
}

bool
VPath::moveTo( const KoPoint& p )
{
	if( isClosed() ) return false;

	// move "begin" when path is still empty:
	if( m_segmentLists.getLast()->moveTo( p ) )
	{
		return true;
	}
	// otherwise create a new subpath:
	else
	{
		// add an initial segmentlist:
		VSegmentList* list = new VSegmentList( this );
		list->moveTo( p );
		m_segmentLists.append( list );
	}

	return false;
}

bool
VPath::lineTo( const KoPoint& p )
{
	return m_segmentLists.getLast()->lineTo( p );
}

bool
VPath::curveTo(
	const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
{
	return m_segmentLists.getLast()->curveTo( p1, p2, p3 );
}

bool
VPath::curve1To( const KoPoint& p2, const KoPoint& p3 )
{
	return m_segmentLists.getLast()->curve1To( p2, p3 );
}

bool
VPath::curve2To( const KoPoint& p1, const KoPoint& p3 )
{
	return m_segmentLists.getLast()->curve2To( p1, p3 );
}

bool
VPath::arcTo( const KoPoint& p1, const KoPoint& p2, const double r )
{
	return m_segmentLists.getLast()->arcTo( p1, p2, r );
}

void
VPath::close()
{
	m_segmentLists.getLast()->close();
}

bool
VPath::isClosed() const
{
	return m_segmentLists.getLast()->isClosed();
}

void
VPath::combine( const VPath& path )
{
	QPtrListIterator<VSegmentList> itr( path.m_segmentLists );
	for( ; itr.current(); ++itr )
	{
		combineSegmentList( *( itr.current() ) );
	}
}

void
VPath::combineSegmentList( const VSegmentList& segmentList )
{
	VSegmentList* list = segmentList.clone();
	list->setParent( this );
	m_segmentLists.append( list );
}

void
VPath::transform( const QWMatrix& m )
{
	QPtrListIterator<VSegmentList> itr( m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		itr.current()->transform( m );
	}

	if( m_stroke->type() == stroke_gradient )
		m_stroke->gradient().transform( m );
	if( m_fill->type() == fill_gradient )
		m_fill->gradient().transform( m );
}

const KoRect&
VPath::boundingBox() const
{
	if( m_boundingBoxIsInvalid )
	{
		// clear:
		m_boundingBox = KoRect();

		QPtrListIterator<VSegmentList> itr( m_segmentLists );
		for( itr.toFirst(); itr.current(); ++itr )
		{
			m_boundingBox |= itr.current()->boundingBox();
		}

		// take line width into account:
		m_boundingBox.setCoords(
			m_boundingBox.left()   - 0.5 * stroke()->lineWidth(),
			m_boundingBox.top()    - 0.5 * stroke()->lineWidth(),
			m_boundingBox.right()  + 0.5 * stroke()->lineWidth(),
			m_boundingBox.bottom() + 0.5 * stroke()->lineWidth() );

		m_boundingBoxIsInvalid = false;
	}

	return m_boundingBox;
}

VPath*
VPath::clone() const
{
	return new VPath( *this );
}

void
VPath::save( QDomElement& element ) const
{
	if( state() != state_deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "PATH" );
		element.appendChild( me );

		VObject::save( me );

		// save segmentlists:
		QPtrListIterator<VSegmentList> itr( m_segmentLists );
		for( itr.toFirst(); itr.current(); ++itr )
		{
			itr.current()->save( me );
		}
	}
}

void
VPath::load( const QDomElement& element )
{
	m_segmentLists.clear();
	setState( state_normal );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement child = list.item( i ).toElement();

			if( child.tagName() == "SEGMENTS" )
			{
				VSegmentList sl( this );
				sl.load( child );
				combineSegmentList( sl );
			}
			else
			{
				VObject::load( child );
			}
		}
	}
}

void
VPath::accept( VVisitor& visitor )
{
	visitor.visitVPath( *this );
}

