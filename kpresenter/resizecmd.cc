/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Resize Command                                         */
/******************************************************************/

#include "kpresenter_doc.h"
#include "resizecmd.h"
#include "resizecmd.moc"

/******************************************************************/
/* Class: ResizeCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
ResizeCmd::ResizeCmd(QString _name,QPoint _m_diff,QSize _r_diff,KPObject *_object,KPresenterDocument_impl *_doc)
  : Command(_name), m_diff(_m_diff), r_diff(_r_diff)
{
  object = _object;
  doc = _doc;
  object->incCmdRef();
}

/*======================== destructor ============================*/
ResizeCmd::~ResizeCmd()
{
  object->decCmdRef();
}

/*====================== execute =================================*/
void ResizeCmd::execute()
{
  QRect oldRect;

  oldRect = object->getBoundingRect(0,0);
  object->moveBy(m_diff);
  object->resizeBy(r_diff);
  doc->repaint(oldRect);
  doc->repaint(object);
}

/*====================== unexecute ===============================*/
void ResizeCmd::unexecute()
{
  QRect oldRect;

  oldRect = object->getBoundingRect(0,0);
  object->moveBy(-m_diff.x(),-m_diff.y());
  object->resizeBy(-r_diff.width(),-r_diff.height());
  doc->repaint(oldRect);
  doc->repaint(object);
}

