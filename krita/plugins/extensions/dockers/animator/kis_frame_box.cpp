/*
 *  Copyright (c) 2013 Somsubhra Bairi <somsubhra.bairi@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License, or(at you option)
 *  any later version..
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_frame_box.h"
#include <QPainter>
#include "kis_animation_layerbox.h"
#include "kis_layer_contents.h"
#include <kis_debug.h>
#include "kis_timeline_header.h"
#include "kis_animation_frame.h"
#include <kis_animation_doc.h>
#include <kis_view2.h>

KisFrameBox::KisFrameBox(KisTimeline *parent)
{
    this->m_dock = parent;
    m_layers = this->m_dock->getLayerBox()->getLayers();

    m_timelineHeader = new KisTimelineHeader(this);
    m_timelineHeader->setGeometry(QRect(0, 0, 100000, 20));

    KisLayerContents* firstContents = new KisLayerContents(this);
    m_layerContents << firstContents;
    firstContents->setGeometry(QRect(0, m_layerContents.length()*20, 100000, 20));
}

void KisFrameBox::addLayerUiUpdate()
{   
    this->setFixedHeight(this->height()+20);

    KisLayerContents* newContents = new KisLayerContents(this);
    m_layerContents << newContents;
    int y = 0;
    int noLayers = m_layerContents.length();

    for(int i = 0; i < noLayers - 1; i++) {
        y = m_layerContents.at(i)->geometry().y();
        m_layerContents.at(i)->setGeometry(QRect(0, y + 20, 100000, 20));
    }

    newContents->setGeometry(QRect(0, 20, 100000, 20));
    newContents->show();
}

void KisFrameBox::setSelectedFrame(KisAnimationFrame *selectedFrame)
{
    this->m_selectedFrame = selectedFrame;
    if(this->m_selectedFrame) {
        int layerIndex = this->m_selectedFrame->getParent()->getLayerIndex();
        QRect globalPosition(this->m_selectedFrame->x(),
                             layerIndex * 20,
                             this->m_selectedFrame->width(),
                             this->m_selectedFrame->height());
        emit frameSelectionChanged(globalPosition);
    }
}

KisAnimationFrame* KisFrameBox::getSelectedFrame()
{
    return m_selectedFrame;
}

KisLayerContents* KisFrameBox::getFirstLayer()
{

    if(m_layerContents.isEmpty()) {
        return 0;
    }

    return m_layerContents.at(0);
}

QList<KisLayerContents*> KisFrameBox::getLayerContents()
{
    return this->m_layerContents;
}