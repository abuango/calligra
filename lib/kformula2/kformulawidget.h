/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KFORMULAWIDGET_H
#define __KFORMULAWIDGET_H

#include <qdom.h>
#include <qwidget.h>

//#include <kaccel.h>
//#include <kaction.h>

#include "formuladefs.h"


class BasicElement;
class FormulaCursor;
class FormulaElement;
class KFormulaContainer;


/**
 * The view. A widget that shows the formula. There are methods
 * to move the cursor around. To edit the formula use the document.
 */
class KFormulaWidget : public QWidget {
    Q_OBJECT

public:
    KFormulaWidget(KFormulaContainer*, QWidget* parent=0, const char* name=0, WFlags f=0);
    ~KFormulaWidget();

protected:

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    virtual void paintEvent(QPaintEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);

public slots:
    
    void slotSelectAll();

    void slotMoveLeft(MoveFlag flag);
    void slotMoveRight(MoveFlag flag);
    void slotMoveUp(MoveFlag flag);
    void slotMoveDown(MoveFlag flag);
    void slotMoveHome(MoveFlag flag);
    void slotMoveEnd(MoveFlag flag);

protected slots:

    void slotFormulaChanged();
    void slotFormulaLoaded(FormulaElement*);
    void slotElementWillVanish(BasicElement*);

protected:

    MoveFlag movementFlag(int state);

    void hideCursor();
    void showCursor();

    KFormulaContainer* getDocument() { return document; }

    FormulaCursor* getCursor() { return cursor; }
    
private:
    
    bool cursorVisible;

//     KAccel* accel;
    
//     KAction* openFile;
//     KAction* saveFile;
//     KAction* quitAction;
//     KAction* undoAction;
//     KAction* redoAction;

//     KAction* cutAction;
//     KAction* copyAction;
//     KAction* pasteAction;
//     KAction* selectAllAction;

//     KAction* integralElement;
//     KAction* productElement;
//     KAction* sumElement;
//     KAction* rootElement;
//     KAction* fractionElement;
//     KAction* matrixElement;

//     KAction* generalUpperIndex;
//     KAction* generalLowerIndex;

//     KAction* upperLeftIndex;
//     KAction* lowerLeftIndex;
//     KAction* upperRightIndex;
//     KAction* lowerRightIndex;
    
    KFormulaContainer* document;
    FormulaCursor* cursor;
};

#endif // __KFORMULAWIDGET_H
