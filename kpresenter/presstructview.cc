/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page Structure Viewer                                  */
/******************************************************************/

#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include "page.h"
#include "presstructview.h"
#include "presstructview.moc"

/******************************************************************/
/* Class: PresStructViewer                                        */
/******************************************************************/

/*================================================================*/
PresStructViewer::PresStructViewer(QWidget *parent,const char *name,KPresenterDocument_impl *_doc,KPresenterView_impl *_view)
  : QDialog(parent,name,false)
{
  doc = _doc;
  view = _view;
  lastSelected = 0;

  pageList.setAutoDelete(true);
  objList.setAutoDelete(true);

  panner = new KNewPanner(this,"panner",KNewPanner::Vertical,KNewPanner::Percent,30);

  treelist = new KTreeList(panner,"");
  setupTreeView();

  list = new KTabListBox(panner,"",2);
  list->setTableFlags(list->tableFlags() | Tbl_smoothHScrolling | Tbl_smoothVScrolling);
  list->setColumn(0,i18n("Description"),250);
  list->setColumn(1,i18n("Value"),190);

  setupToolBar();

  panner->activate(treelist,list);

  resize(600,400);
  
  toolbar->move(0,0);
  toolbar->resize(600,toolbar->height());

  panner->move(0,toolbar->height());
  panner->resize(600,400 - toolbar->height());
}

/*================================================================*/
void PresStructViewer::itemSelected(int _index)
{
  KTreeListItem *item = treelist->itemAt(_index);
  QString text(item->getText());

  // must be a page
  if (text.contains("Page",false) > 0)
    {
      ItemInfo *info = 0;
      for (unsigned int i = 0;i < pageList.count();i++)
	{
	  info = pageList.at(i);
	  if (info->item == item) fillWithPageInfo(doc->backgroundList()->at(info->num),info->num);
	}
    }

  // must be an object
  else
    {
      ItemInfo *info = 0;
      for (unsigned int i = 0;i < objList.count();i++)
	{
	  info = objList.at(i);
	  if (info->item == item) fillWithObjInfo(doc->objectList()->at(info->num),info->num);
	}
    }
}

/*================================================================*/
void PresStructViewer::resizeEvent(QResizeEvent *e)
{
  QDialog::resizeEvent(e);
  toolbar->resize(width(),toolbar->height());
  panner->resize(width(),height() - toolbar->height());
}

/*================================================================*/
void PresStructViewer::closeEvent(QCloseEvent *e)
{
  QDialog::closeEvent(e);
  emit presStructViewClosed();
}

/*================================================================*/
void PresStructViewer::setupTreeView()
{
  KTreeListItem *item = 0;
  ItemInfo *info = 0;

  QString pixdir = kapp->kde_toolbardir();
  QString page_name,obj_name;
  int pgnum;

  for (unsigned int i = 0;i < doc->backgroundList()->count();i++)
    {
      page_name.sprintf("%d. Page",i + 1);
      item = new KTreeListItem(page_name.data(),new QPixmap(pixdir + "/filenew.xpm"));
      treelist->insertItem(item,-1,false);
      info = new ItemInfo;
      info->num = i;
      info->item = item;
      pageList.append(info);
    }

  pixdir = kapp->kde_datadir();
  KPObject *kpobject = 0;
  for (unsigned int i = 0;i < doc->objectList()->count();i++)
    {
      kpobject = doc->objectList()->at(i);
      obj_name.sprintf("%s (%d)",i18n(ObjName[static_cast<int>(kpobject->getType())]),i + 1);

      QString str;
      switch (kpobject->getType())
	{
	case OT_RECT: str = "mini_rect.xpm";
	  break;
	case OT_PICTURE: str = "mini_picture.xpm";
	  break;
	case OT_CLIPART: str = "mini_clipart.xpm";
	  break;
	case OT_LINE: str = "mini_line.xpm";
	  break;
	case OT_ELLIPSE: str = "mini_circle.xpm";
	  break;
	case OT_TEXT: str = "mini_text.xpm";
	  break;
	case OT_AUTOFORM: str = "mini_autoform.xpm";
	  break;
	default: str = "dot.xpm";
	  break;
	}

      item = new KTreeListItem(obj_name.data(),new QPixmap(pixdir + "/kpresenter/toolbar/" + str));
      pgnum = doc->getPageOfObj(i,0,0);
      if (pgnum != -1)
	{
	  pgnum--;
	  treelist->addChildItem(item,treelist->itemIndex(pageList.at(pgnum)->item));;
	  info = new ItemInfo;
	  info->num = i;
	  info->item = item;
	  objList.append(info);
	}
    }

  connect(treelist,SIGNAL(highlighted(int)),this,SLOT(itemSelected(int)));
}

/*================================================================*/
void PresStructViewer::setupToolBar()
{
  toolbar = new KToolBar(this);

  QString pixdir = KApplication::kde_datadir() + "/kpresenter/toolbar/";
  
  toolbar->insertButton(QPixmap(pixdir + "style.xpm"),B_STYLE,SIGNAL(clicked()),this,SLOT(slotStyle()),false,i18n("Pen & Brush"));
  toolbar->insertButton(QPixmap(pixdir + "rotate.xpm"),B_ROTATE,SIGNAL(clicked()),this,SLOT(slotRotate()),false,i18n("Rotate"));
  toolbar->insertButton(QPixmap(pixdir + "shadow.xpm"),B_SHADOW,SIGNAL(clicked()),this,SLOT(slotShadow()),false,i18n("Shadow"));
  toolbar->insertButton(QPixmap(pixdir + "alignobjs.xpm"),B_ALIGN,SIGNAL(clicked()),this,SLOT(slotAlign()),false,i18n("Align"));
  toolbar->insertButton(QPixmap(pixdir + "effect.xpm"),B_EFFECT,SIGNAL(clicked()),this,SLOT(slotEffect()),false,i18n("Assign effect"));

  toolbar->insertSeparator();
  
  toolbar->insertButton(QPixmap(pixdir + "raise.xpm"),B_LOWER,SIGNAL(clicked()),this,SLOT(slotRaise()),false,i18n("Raise"));
  toolbar->insertButton(QPixmap(pixdir + "lower.xpm"),B_RAISE,SIGNAL(clicked()),this,SLOT(slotLower()),false,i18n("Lower"));

  toolbar->insertSeparator();

  toolbar->insertButton(QPixmap(pixdir + "delete.xpm"),B_DELETE,SIGNAL(clicked()),this,SLOT(slotDelete()),false,i18n("Delete"));
  toolbar->insertButton(QPixmap(pixdir + "text.xpm"),B_EDIT,SIGNAL(clicked()),this,SLOT(slotEdit()),false,i18n("Edit"));
  toolbar->insertButton(QPixmap(KApplication::kde_toolbardir() + "/fileopen.xpm"),B_CFILEN,
			SIGNAL(clicked()),this,SLOT(slotChangeFilename()),false,i18n("Change Filename"));

  toolbar->insertSeparator();

  toolbar->insertButton(QPixmap(pixdir + "picture.xpm"),B_BACK,SIGNAL(clicked()),this,SLOT(slotBackground()),false,i18n("Background"));
  toolbar->insertButton(QPixmap(KApplication::kde_toolbardir() + "/filenew.xpm"),B_CPAGES,
			SIGNAL(clicked()),this,SLOT(slotConfigPages()),false,i18n("Configure Pages"));

  toolbar->setFullWidth(true);
  toolbar->enableFloating(false);
  toolbar->setBarPos(KToolBar::Top);
  toolbar->enableMoving(false);
  toolbar->updateRects();
}

/*================================================================*/
void PresStructViewer::fillWithPageInfo(KPBackGround *_page,int _num)
{
  QString str;
  int r,g,b;
  QColor c;

  list->setNumRows(0);
  str.sprintf("%d",_num + 1);
  list->appendItem(i18n("Number")); 
  list->changeItemPart(str,list->count() - 1,1);

  list->appendItem(i18n("Back Type")); 
  list->changeItemPart(i18n(BackTypeName[static_cast<int>(_page->getBackType())]),
		       list->count() - 1,1);

  list->appendItem(i18n("Back View")); 
  list->changeItemPart(i18n(BackViewName[static_cast<int>(_page->getBackView())]),
		       list->count() - 1,1);

  c = _page->getBackColor1();
  c.rgb(&r,&g,&b);
  str.sprintf("#%02X%02X%02X",r,g,b);
  list->appendItem(i18n("Color1")); 
  list->changeItemPart(str,list->count() - 1,1);

  c = _page->getBackColor2();
  c.rgb(&r,&g,&b);
  str.sprintf("#%02X%02X%02X",r,g,b);
  list->appendItem(i18n("Color2")); 
  list->changeItemPart(str,list->count() - 1,1);

  list->appendItem(i18n("Picture Filename")); 
  list->changeItemPart(_page->getBackPixFilename(),list->count() - 1,1);

  list->appendItem(i18n("Clipart Filename")); 
  list->changeItemPart(_page->getBackClipFilename(),list->count() - 1,1);

  list->appendItem(i18n("Effect for changing to next page")); 
  list->changeItemPart(i18n(PageEffectName[static_cast<int>(_page->getPageEffect())]),
		       list->count() - 1,1);

  view->skipToPage(_num);

  disableAllFunctions();
  toolbar->setItemEnabled(B_BACK,true);
  toolbar->setItemEnabled(B_CPAGES,true);
}

/*================================================================*/
void PresStructViewer::fillWithObjInfo(KPObject *_obj,int _num)
{
  QString str;

  list->setNumRows(0);
  str.sprintf("%d",_num + 1);
  list->appendItem(i18n("Number")); 
  list->changeItemPart(str,list->count() - 1,1);

  list->appendItem(i18n("Type"));
  list->changeItemPart(i18n(ObjName[static_cast<int>(_obj->getType())]),
		       list->count() - 1,1);

  str.sprintf("%d",_obj->getOrig().x());
  list->appendItem(i18n("X-Coordinate"));
  list->changeItemPart(str,list->count() - 1,1);

  str.sprintf("%d",_obj->getOrig().y());
  list->appendItem(i18n("Y-Coordinate"));
  list->changeItemPart(str,list->count() - 1,1);

  str.sprintf("%d",_obj->getSize().width());
  list->appendItem(i18n("Width"));
  list->changeItemPart(str,list->count() - 1,1);

  str.sprintf("%d",_obj->getSize().height());
  list->appendItem(i18n("Height"));
  list->changeItemPart(str,list->count() - 1,1);

  str.sprintf("%g",_obj->getAngle());
  list->appendItem(i18n("Angle"));
  list->changeItemPart(str,list->count() - 1,1);

  str.sprintf("%d",_obj->getShadowDistance());
  list->appendItem(i18n("Shadow Distance"));
  list->changeItemPart(str,list->count() - 1,1);

  list->appendItem(i18n("Shadow Direction"));
  list->changeItemPart(i18n(ShadowDirectionName[static_cast<int>(_obj->getShadowDirection())]),
		       list->count() - 1,1);

  str.sprintf("%d",_obj->getPresNum());
  list->appendItem(i18n("Appearing number in the Presentation"));
  list->changeItemPart(str,list->count() - 1,1);

  list->appendItem(i18n("Effect"));
  list->changeItemPart(i18n(EffectName[static_cast<int>(_obj->getEffect())]),
		       list->count() - 1,1);

  list->appendItem(i18n("Objectspecific Effect"));
  list->changeItemPart(i18n(Effect2Name[static_cast<int>(_obj->getEffect2())]),
		       list->count() - 1,1);

  switch (_obj->getType())
    {
    case OT_RECT:
      {
	list->appendItem(i18n("Pen Color"));
	if (dynamic_cast<KPRectObject*>(_obj)->getPen().style() == NoPen)
	  list->changeItemPart("no pen",list->count() - 1,1);
	else
	  list->changeItemPart(getColor(dynamic_cast<KPRectObject*>(_obj)->getPen().color()),list->count() - 1,1);

	list->appendItem(i18n("Pen Style"));
	list->changeItemPart(i18n(PenStyleName[static_cast<int>(dynamic_cast<KPRectObject*>(_obj)->getPen().style())]),
			     list->count() - 1,1);

	str.sprintf("%d",dynamic_cast<KPRectObject*>(_obj)->getPen().width());
	list->appendItem(i18n("Pen Width"));
	list->changeItemPart(str,list->count() - 1,1);

	if (dynamic_cast<KPRectObject*>(_obj)->getFillType() == FT_BRUSH)
	  {
	    list->appendItem(i18n("Brush Color"));
	    if (dynamic_cast<KPRectObject*>(_obj)->getBrush().style() == NoBrush)
	      list->changeItemPart("no brush",list->count() - 1,1);
	    else
	      list->changeItemPart(getColor(dynamic_cast<KPRectObject*>(_obj)->getBrush().color()),list->count() - 1,1);
	    
	    list->appendItem(i18n("Brush Style"));
	    list->changeItemPart(i18n(BrushStyleName[static_cast<int>(dynamic_cast<KPRectObject*>(_obj)->getBrush().style())]),
				 list->count() - 1,1);
	  }
	else
	  {
	    list->appendItem(i18n("Gradient Color1"));
	    list->changeItemPart(getColor(dynamic_cast<KPRectObject*>(_obj)->getGColor1()),list->count() - 1,1);

	    list->appendItem(i18n("Gradient Color2"));
	    list->changeItemPart(getColor(dynamic_cast<KPRectObject*>(_obj)->getGColor2()),list->count() - 1,1);

	    list->appendItem(i18n("Gradient Type"));
	    list->changeItemPart(i18n(BackColorTypeName[static_cast<int>(dynamic_cast<KPRectObject*>(_obj)->getGType())]),
				 list->count() - 1,1);
	  }

	if (dynamic_cast<KPRectObject*>(_obj)->getRectType() == RT_ROUND)
	  {
	    int rndx,rndy;
	    dynamic_cast<KPRectObject*>(_obj)->getRnds(rndx,rndy);
	    
	    list->appendItem(i18n("Roundedness X"));
	    str.sprintf("%d",rndx);
	    list->changeItemPart(str,list->count() - 1,1);
	    
	    list->appendItem(i18n("Roundedness Y"));
	    str.sprintf("%d",rndy);
	    list->changeItemPart(str,list->count() - 1,1);
	  }
      } break;
    case OT_ELLIPSE:
      {
	list->appendItem(i18n("Pen Color"));
	if (dynamic_cast<KPEllipseObject*>(_obj)->getPen().style() == NoPen)
	  list->changeItemPart("no pen",list->count() - 1,1);
	else
	  list->changeItemPart(getColor(dynamic_cast<KPEllipseObject*>(_obj)->getPen().color()),list->count() - 1,1);

	list->appendItem(i18n("Pen Style"));
	list->changeItemPart(i18n(PenStyleName[static_cast<int>(dynamic_cast<KPEllipseObject*>(_obj)->getPen().style())]),
			     list->count() - 1,1);

	str.sprintf("%d",dynamic_cast<KPEllipseObject*>(_obj)->getPen().width());
	list->appendItem(i18n("Pen Width"));
	list->changeItemPart(str,list->count() - 1,1);

	if (dynamic_cast<KPEllipseObject*>(_obj)->getFillType() == FT_BRUSH)
	  {
	    list->appendItem(i18n("Brush Color"));
	    if (dynamic_cast<KPEllipseObject*>(_obj)->getBrush().style() == NoBrush)
	      list->changeItemPart("no brush",list->count() - 1,1);
	    else
	      list->changeItemPart(getColor(dynamic_cast<KPEllipseObject*>(_obj)->getBrush().color()),list->count() - 1,1);
	    
	    list->appendItem(i18n("Brush Style"));
	    list->changeItemPart(i18n(BrushStyleName[static_cast<int>(dynamic_cast<KPEllipseObject*>(_obj)->getBrush().style())]),
				 list->count() - 1,1);
	  }
	else
	  {
	    list->appendItem(i18n("Gradient Color1"));
	    list->changeItemPart(getColor(dynamic_cast<KPEllipseObject*>(_obj)->getGColor1()),list->count() - 1,1);

	    list->appendItem(i18n("Gradient Color2"));
	    list->changeItemPart(getColor(dynamic_cast<KPEllipseObject*>(_obj)->getGColor2()),list->count() - 1,1);

	    list->appendItem(i18n("Gradient Type"));
	    list->changeItemPart(i18n(BackColorTypeName[static_cast<int>(dynamic_cast<KPEllipseObject*>(_obj)->getGType())]),
				 list->count() - 1,1);
	  }
      } break;
    case OT_LINE:
      {
	list->appendItem(i18n("Direction"));
	list->changeItemPart(i18n(LineTypeName[static_cast<int>(dynamic_cast<KPLineObject*>(_obj)->getLineType())]),
			     list->count() - 1,1);

	list->appendItem(i18n("Pen Color"));
	if (dynamic_cast<KPLineObject*>(_obj)->getPen().style() == NoPen)
	  list->changeItemPart("no pen",list->count() - 1,1);
	else
	  list->changeItemPart(getColor(dynamic_cast<KPLineObject*>(_obj)->getPen().color()),list->count() - 1,1);

	list->appendItem(i18n("Pen Style"));
	list->changeItemPart(i18n(PenStyleName[static_cast<int>(dynamic_cast<KPLineObject*>(_obj)->getPen().style())]),
			     list->count() - 1,1);

	str.sprintf("%d",dynamic_cast<KPLineObject*>(_obj)->getPen().width());
	list->appendItem(i18n("Pen Width"));
	list->changeItemPart(str,list->count() - 1,1);

	list->appendItem(i18n("Line Begin"));
	list->changeItemPart(i18n(LineEndName[static_cast<int>(dynamic_cast<KPLineObject*>(_obj)->getLineBegin())]),
			     list->count() - 1,1);

	list->appendItem(i18n("Line End"));
	list->changeItemPart(i18n(LineEndName[static_cast<int>(dynamic_cast<KPLineObject*>(_obj)->getLineEnd())]),
			     list->count() - 1,1);
      } break;
    case OT_AUTOFORM:
      {
	list->appendItem(i18n("Name"));

	QString filename = dynamic_cast<KPAutoformObject*>(_obj)->getFileName();
	QFileInfo fi(filename);
	list->changeItemPart(fi.baseName(),list->count() - 1,1);

	list->appendItem(i18n("Pen Color"));
	if (dynamic_cast<KPAutoformObject*>(_obj)->getPen().style() == NoPen)
	  list->changeItemPart("no pen",list->count() - 1,1);
	else
	  list->changeItemPart(getColor(dynamic_cast<KPAutoformObject*>(_obj)->getPen().color()),list->count() - 1,1);

	list->appendItem(i18n("Pen Style"));
	list->changeItemPart(i18n(PenStyleName[static_cast<int>(dynamic_cast<KPAutoformObject*>(_obj)->getPen().style())]),
			     list->count() - 1,1);

	str.sprintf("%d",dynamic_cast<KPAutoformObject*>(_obj)->getPen().width());
	list->appendItem(i18n("Pen Width"));
	list->changeItemPart(str,list->count() - 1,1);

	list->appendItem(i18n("Line Begin"));
	list->changeItemPart(i18n(LineEndName[static_cast<int>(dynamic_cast<KPAutoformObject*>(_obj)->getLineBegin())]),
			     list->count() - 1,1);

	list->appendItem(i18n("Line End"));
	list->changeItemPart(i18n(LineEndName[static_cast<int>(dynamic_cast<KPAutoformObject*>(_obj)->getLineEnd())]),
			     list->count() - 1,1);

	if (dynamic_cast<KPAutoformObject*>(_obj)->getFillType() == FT_BRUSH)
	  {
	    list->appendItem(i18n("Brush Color"));
	    if (dynamic_cast<KPAutoformObject*>(_obj)->getBrush().style() == NoBrush)
	      list->changeItemPart("no brush",list->count() - 1,1);
	    else
	      list->changeItemPart(getColor(dynamic_cast<KPAutoformObject*>(_obj)->getBrush().color()),list->count() - 1,1);
	    
	    list->appendItem(i18n("Brush Style"));
	    list->changeItemPart(i18n(BrushStyleName[static_cast<int>(dynamic_cast<KPAutoformObject*>(_obj)->getBrush().style())]),
				 list->count() - 1,1);
	  }
	else
	  {
	    list->appendItem(i18n("Gradient Color1"));
	    list->changeItemPart(getColor(dynamic_cast<KPAutoformObject*>(_obj)->getGColor1()),list->count() - 1,1);

	    list->appendItem(i18n("Gradient Color2"));
	    list->changeItemPart(getColor(dynamic_cast<KPAutoformObject*>(_obj)->getGColor2()),list->count() - 1,1);

	    list->appendItem(i18n("Gradient Type"));
	    list->changeItemPart(i18n(BackColorTypeName[static_cast<int>(dynamic_cast<KPAutoformObject*>(_obj)->getGType())]),
				 list->count() - 1,1);
	  }
      } break;
    case OT_PICTURE:
      {
	list->appendItem(i18n("Filename"));
	list->changeItemPart(dynamic_cast<KPPixmapObject*>(_obj)->getFileName(),list->count() - 1,1);
      } break;
    case OT_CLIPART:
      {
	list->appendItem(i18n("Filename"));
	list->changeItemPart(dynamic_cast<KPClipartObject*>(_obj)->getFileName(),list->count() - 1,1);
      } break;
    default: break;
    }
  
  if (lastSelected)
    {
      lastSelected->setSelected(false);
      doc->repaint(lastSelected);
    }
  
  view->makeRectVisible(_obj->getBoundingRect(0,0));
  _obj->setSelected(true);
  doc->repaint(_obj);
  lastSelected = _obj;

  disableAllFunctions();

  if (_obj->getType() == OT_TEXT)
    toolbar->setItemEnabled(B_EDIT,true);
  else if (_obj->getType() == OT_CLIPART || _obj->getType() == OT_PICTURE)
    toolbar->setItemEnabled(B_CFILEN,true);
  else
    toolbar->setItemEnabled(B_STYLE,true);
  
  toolbar->setItemEnabled(B_ROTATE,true);
  toolbar->setItemEnabled(B_SHADOW,true);
  toolbar->setItemEnabled(B_ALIGN,true);
  toolbar->setItemEnabled(B_EFFECT,true);
  toolbar->setItemEnabled(B_LOWER,true);
  toolbar->setItemEnabled(B_RAISE,true);
  toolbar->setItemEnabled(B_DELETE,true);
}

/*================================================================*/
QString PresStructViewer::getColor(QColor _color)
{
  QFile rgbFile("/usr/X11R6/lib/X11/rgb.txt");
  QString str;
  
  if (rgbFile.exists())
    {
      int r,g,b;
      str.sprintf("(%d,%d,%d)",_color.red(),_color.green(),_color.blue());

      QTextStream t(&rgbFile);
      QString s = "";

      char name[255];

      if (rgbFile.open(IO_ReadOnly))
	{
	  while (!t.eof())
	    {
	      s = t.readLine();
	      sscanf(s,"%d %d %d %s",&r,&g,&b,name);
	      if (r == _color.red() && g == _color.green() && b == _color.blue())
		{
		  str.sprintf("%s",name);
		  str = str.stripWhiteSpace();
		  break;
		}
	    }
	  rgbFile.close();   
	}
    }
  else
    str.sprintf("(%d,%d,%d)",_color.red(),_color.green(),_color.blue());
    
  return str;
}

/*================================================================*/
void PresStructViewer::slotStyle()
{
  view->extraPenBrush();
}

/*================================================================*/
void PresStructViewer::slotRotate()
{
  view->extraRotate();
}

/*================================================================*/
void PresStructViewer::slotShadow()
{
  view->extraShadow();
}

/*================================================================*/
void PresStructViewer::slotAlign()
{
  view->extraAlignObj();
}

/*================================================================*/
void PresStructViewer::slotEffect()
{
  view->screenAssignEffect();
}

/*================================================================*/
void PresStructViewer::slotLower()
{
  view->extraLower();
}

/*================================================================*/
void PresStructViewer::slotRaise()
{
  view->extraRaise();
}

/*================================================================*/
void PresStructViewer::slotDelete()
{
  view->editDelete();
  view->restartPresStructView();
}

/*================================================================*/
void PresStructViewer::slotEdit()
{
  view->getPage()->editSelectedTextArea();
}

/*================================================================*/
void PresStructViewer::slotBackground()
{
  view->extraBackground();
}

/*================================================================*/
void PresStructViewer::slotConfigPages()
{
  view->screenConfigPages();
}

/*================================================================*/
void PresStructViewer::slotChangeFilename()
{
  if (lastSelected)
    {
      if (lastSelected->getType() == OT_PICTURE)
	view->getPage()->chPic();
      else
	view->getPage()->chClip();
    }
}

/*================================================================*/
void PresStructViewer::disableAllFunctions()
{
  toolbar->setItemEnabled(B_STYLE,false);
  toolbar->setItemEnabled(B_ROTATE,false);
  toolbar->setItemEnabled(B_SHADOW,false);
  toolbar->setItemEnabled(B_ALIGN,false);
  toolbar->setItemEnabled(B_EFFECT,false);
  toolbar->setItemEnabled(B_LOWER,false);
  toolbar->setItemEnabled(B_RAISE,false);
  toolbar->setItemEnabled(B_DELETE,false);
  toolbar->setItemEnabled(B_EDIT,false);
  toolbar->setItemEnabled(B_BACK,false);
  toolbar->setItemEnabled(B_CPAGES,false);
  toolbar->setItemEnabled(B_CFILEN,false);
}
