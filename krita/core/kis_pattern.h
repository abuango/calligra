/*
 *  kis_pattern.h - part of Krayon
 *
 *  Copyright (c) 2000 Matthias Elter  <elter@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __kis_pattern_h__
#define __kis_pattern_h__

#include <qsize.h>

#include <kio/job.h>

#include <koIconChooser.h>

#include "kis_resource.h"
#include "kis_resource.h"

class QPoint;
class QPixmap;
class QImage;

class KisPattern : public KisResource {
	typedef KisResource super;
	Q_OBJECT

public:
	KisPattern(const QString& file);
	virtual ~KisPattern();

	virtual bool loadAsync();
	virtual bool saveAsync();
	virtual QImage img() const;

	bool isValid() const { return m_valid; }
	QPoint hotSpot() const { return m_hotSpot; }

	QPixmap& pixmap() const;
	QPixmap& thumbPixmap() const;

private slots:
	void ioData(KIO::Job *job, const QByteArray& data);
	void ioResult(KIO::Job *job);

private:
	QByteArray m_data;	
	QPoint m_hotSpot;

	bool m_valid;
	bool m_validThumb;
	
	QImage m_img;
        QPixmap *m_pixmap;
        QPixmap *m_thumbPixmap;

};

#endif

