/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: KWAutoFormat (header)                                  */
/******************************************************************/

#include "autoformat.h"
#include "parag.h"
#include "kword_doc.h"
#include "fc.h"

/******************************************************************/
/* Class: KWAutoFormatEntry                                       */
/******************************************************************/

/*================================================================*/
KWAutoFormatEntry::KWAutoFormatEntry()
{
  checkFamily = checkColor = checkSize = checkBold = checkItalic = checkUnderline = checkVertAlign = false;
  expr = "";
  family = "times";
  color = Qt::black;
  size = 12;
  bold = italic = underline = false;
  vertAlign = KWFormat::VA_NORMAL;
  caseSensitive = false;
  regexp = false;
  wildcard = false;
}

/******************************************************************/
/* Class: KWAutoFormat                                            */
/******************************************************************/

/*================================================================*/
KWAutoFormat::KWAutoFormat(KWordDocument *_doc)
  : typographicQuotes(), enabled(false), lastWasDotSpace(false), convertUpperCase(true)
{
  doc = _doc;
  tmpBuffer = 0L;
}

/*================================================================*/
void KWAutoFormat::startAutoFormat(KWParag *parag,KWFormatContext *fc)
{
  if (!enabled)
    return;

  lastWasDotSpace = false;
  
  tmpBuffer = new KWString(doc);
}

/*================================================================*/
bool KWAutoFormat::doAutoFormat(KWParag *parag,KWFormatContext *fc)
{
  if (!enabled)
    return false;

  tmpBuffer->append(parag->getKWString()->data()[fc->getTextPos()]);

  return false;
}

/*================================================================*/
void KWAutoFormat::endAutoFormat(KWParag *parag,KWFormatContext *fc)
{
  if (!enabled)
    return;

  if (tmpBuffer)
    delete tmpBuffer;
  tmpBuffer = 0L;
}

/*================================================================*/
bool KWAutoFormat::doTypographicQuotes(KWParag *parag,KWFormatContext *fc)
{
  if (!typographicQuotes.replace || !enabled)
    return false;

  if (parag->getKWString()->data()[fc->getTextPos()].c == QChar('\"') ||
      parag->getKWString()->data()[fc->getTextPos()].c == typographicQuotes.begin ||
      parag->getKWString()->data()[fc->getTextPos()].c == typographicQuotes.end)
    {
      if (fc->getTextPos() == 0 || fc->getTextPos() > 0 &&
	  parag->getKWString()->data()[fc->getTextPos() - 1].c == QChar(' '))
	parag->getKWString()->data()[fc->getTextPos()].c = typographicQuotes.begin;
      else
	parag->getKWString()->data()[fc->getTextPos()].c = typographicQuotes.end;

      return true;
    }

  return false;
}

/*================================================================*/
bool KWAutoFormat::doUpperCase(KWParag *parag,KWFormatContext *fc)
{
  if (!enabled || !convertUpperCase)
    return false;

  bool converted = false;
  
  if (lastWasDotSpace &&
      parag->getKWString()->data()[fc->getTextPos()].c != QChar('.') &&
      parag->getKWString()->data()[fc->getTextPos()].c != QChar(' '))
    {
      parag->getKWString()->data()[fc->getTextPos()].c.toUpper();
      converted = true;
    }
      
  if (parag->getKWString()->data()[fc->getTextPos()].c == QChar('.'))
    lastWasDotSpace = true;
  else if (parag->getKWString()->data()[fc->getTextPos()].c != QChar('.') &&
	   parag->getKWString()->data()[fc->getTextPos()].c != QChar(' '))
    lastWasDotSpace = false;

  return converted;
}
