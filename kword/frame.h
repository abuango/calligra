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
/* Module: Frame (header)                                         */
/******************************************************************/

#ifndef frame_h
#define frame_h

#include <qrect.h>
#include <qpoint.h>
#include <qsize.h>

#include <qlist.h>
#include <qcursor.h>
#include <qregion.h>
#include <qpainter.h>
#include <qdom.h>

#include "paraglayout.h"

#include <koMainWindow.h>
#include <openparts.h>

class KWFrame;
class KWordDocument;
class KWordChild;
class KWordFrame;
class KWGroupManager;

enum FrameType {FT_BASE = 0, FT_TEXT = 1, FT_PICTURE = 2, FT_PART};
enum FrameInfo {FI_BODY = 0, FI_FIRST_HEADER = 1, FI_ODD_HEADER = 2, FI_EVEN_HEADER = 3, FI_FIRST_FOOTER = 4, 
		FI_ODD_FOOTER = 5, FI_EVEN_FOOTER = 6, FI_FOOTNOTE = 7};
enum RunAround {RA_NO = 0, RA_BOUNDINGRECT = 1, RA_CONTUR = 2};

/******************************************************************/
/* Class: KWFrame                                                 */
/******************************************************************/

class KWFrame : public QRect
{
public:
    KWFrame();
    KWFrame( const QPoint &topleft, const QPoint &bottomright );
    KWFrame( const QPoint &topleft, const QSize &size );
    KWFrame( int left, int top, int width, int height );
    KWFrame( int left, int top, int width, int height, RunAround _ra, KWUnit _gap );
    KWFrame( const QRect &_rect );

    void setRunAround( RunAround _ra ) { runAround = _ra; }
    RunAround getRunAround() { return runAround; }

    void setSelected( bool _selected )
    { selected = _selected; }
    bool isSelected()
    { return selected; }

    void addIntersect( QRect &_r );
    void clearIntersects()
    { intersections.clear(); }

    int getLeftIndent( int _y, int _h );
    int getRightIndent( int _y, int _h );

    bool hasIntersections()
    { return !intersections.isEmpty(); }

    QCursor getMouseCursor( int mx, int my, bool table );

    KWUnit getRunAroundGap()
    { return runAroundGap; }
    void setRunAroundGap( KWUnit gap )
    { runAroundGap = gap; }

    bool isMostRight() { return mostRight; }
    void setMostRight( bool _mr ) { mostRight = _mr; }

    void setPageNum( int i ) { pageNum = i; }
    int getPageNum() { return pageNum; }

    KWParagLayout::Border &getLeftBorder() { return brd_left; }
    KWParagLayout::Border &getRightBorder() { return brd_right; }
    KWParagLayout::Border &getTopBorder() { return brd_top; }
    KWParagLayout::Border &getBottomBorder() { return brd_bottom; }
    KWParagLayout::Border getLeftBorder2() { return brd_left; }
    KWParagLayout::Border getRightBorder2() { return brd_right; }
    KWParagLayout::Border getTopBorder2() { return brd_top; }
    KWParagLayout::Border getBottomBorder2() { return brd_bottom; }
    void setLeftBorder( KWParagLayout::Border _brd ) { brd_left = _brd; }
    void setRightBorder( KWParagLayout::Border _brd ) { brd_right = _brd; }
    void setTopBorder( KWParagLayout::Border _brd ) { brd_top = _brd; }
    void setBottomBorder( KWParagLayout::Border _brd ) { brd_bottom = _brd; }

    QRegion getEmptyRegion();

    QBrush getBackgroundColor() { return backgroundColor; }
    void setBackgroundColor( QBrush _color ) { backgroundColor = _color; }

    void saveLeftBrd2( QDomElement &e );
    void saveRightBrd2( QDomElement &e );
    void saveTopBrd2( QDomElement &e );
    void saveBottomBrd2( QDomElement &e );

    unsigned int getNextFreeYPos( unsigned int _y, unsigned int _h );

    void setBLeft( KWUnit b ) { bleft = b; }
    void setBRight( KWUnit b ) { bright = b; }
    void setBTop( KWUnit b ) { btop = b; }
    void setBBottom( KWUnit b ) { bbottom = b; }

    KWUnit getBLeft() { return bleft; }
    KWUnit getBRight() { return bright; }
    KWUnit getBTop() { return btop; }
    KWUnit getBBottom() { return bbottom; }

protected:
    RunAround runAround;
    bool selected;
    KWUnit runAroundGap;
    bool mostRight;
    int pageNum;

    QList<QRect> intersections;

    KWParagLayout::Border brd_left, brd_right, brd_top, brd_bottom;
    QBrush backgroundColor;

    KWUnit bleft, bright, btop, bbottom;

private:
    KWFrame &operator=( const KWFrame &_frame );
    KWFrame ( const KWFrame &_frame );

};

/******************************************************************/
/* Class: KWFrameSet                                              */
/******************************************************************/

class KWFrameSet
{
public:
    KWFrameSet( KWordDocument *_doc );
    virtual ~KWFrameSet()
    {; }

    virtual FrameType getFrameType()
    { return FT_BASE; }
    virtual FrameInfo getFrameInfo()
    { return frameInfo; }
    void setFrameInfo( FrameInfo fi ) { frameInfo = fi; }

    virtual void addFrame( KWFrame _frame );
    virtual void addFrame( KWFrame *_frame );
    virtual void delFrame( unsigned int _num );
    virtual void delFrame( KWFrame *frm, bool remove = TRUE );

    virtual int getFrame( int _x, int _y );
    virtual KWFrame *getFrame( unsigned int _num );
    virtual unsigned int getNumFrames()
    { return frames.count(); }

    virtual bool isPTYInFrame( unsigned int /*_frame*/, unsigned int /*_ypos */ )
	{ return true; }

    virtual void update()
    {; }

    virtual void clear()
    { frames.clear(); }

    virtual bool contains( unsigned int mx, unsigned int my );
    /**
     * Return 1, if a frame gets selected which was not selected before,
     * 2, if a frame gets selected which was already selected
     */
    virtual int selectFrame( unsigned int mx, unsigned int my, bool simulate = false );
    virtual void deSelectFrame( unsigned int mx, unsigned int my );
    virtual QCursor getMouseCursor( unsigned int mx, unsigned int my );

    virtual QDomElement save( QDomDocument &doc );

    int getNext( QRect _rect );
    int getPageOfFrame( int i ) { return frames.at( i )->getPageNum(); }

    void setCurrent( int i ) { current = i; }
    int getCurrent() { return current; }

    void setGroupManager( KWGroupManager *gm ) { grpMgr = gm; }
    KWGroupManager *getGroupManager() { return grpMgr; }

    void setIsRemoveableHeader( bool _h )
    { removeableHeader = _h; }
    bool isRemoveableHeader()
    { return removeableHeader; }

    bool hasSelectedFrame();

    bool isVisible() { return visible; }
    void setVisible( bool v ) { visible = v; }

    QString getName() const { return name; }
    void setName( const QString &_name ) { name = _name; }

protected:
    virtual void init()
    {; }

    // document
    KWordDocument *doc;

    // frames
    QList<KWFrame> frames;

    FrameInfo frameInfo;
    int current;
    KWGroupManager *grpMgr;
    bool removeableHeader, visible;
    QString name;

};

/******************************************************************/
/* Class: KWTextFrameSet                                          */
/******************************************************************/

class KWTextFrameSet : public KWFrameSet
{
public:
    KWTextFrameSet( KWordDocument *_doc )
        : KWFrameSet( _doc )
    {; }
    ~KWTextFrameSet();

    virtual FrameType getFrameType()
    { return FT_TEXT; }

    virtual void update();

    /**
     * If another parag becomes the first one it uses this function
     * to tell the document about it.
     */
    void setFirstParag( KWParag *_parag );
    KWParag* getFirstParag();

    /**
     * WARNING: This methode is _NOT_ efficient! It iterates through all parags!
     */
    KWParag *getLastParag();

    virtual bool isPTYInFrame( unsigned int _frame, unsigned int _ypos );

    void deleteParag( KWParag *_parag );
    void joinParag( KWParag *_parag1, KWParag *_parag2 );
    void insertParag( KWParag *_parag, InsertPos _pos );
    void splitParag( KWParag *_parag, unsigned int _pos );

    virtual QDomElement save( QDomDocument &doc );
    virtual void load( KOMLParser&, vector<KOMLAttrib>& );

    bool getAutoCreateNewFrame() { return autoCreateNewFrame; }
    void setAutoCreateNewFrame( bool _auto ) { autoCreateNewFrame = _auto; }

    void updateCounters();
    void updateAllStyles();

    // this function is optimized for framesets in tables and doesn't work for other purposes
    void assign( KWTextFrameSet *fs );

protected:
    virtual void init();

    // pointer to the first parag of the list of parags
    KWParag *parags;
    bool autoCreateNewFrame;

};

/******************************************************************/
/* Class: KWPictureFrameSet                                       */
/******************************************************************/

class KWPictureFrameSet : public KWFrameSet
{
public:
    KWPictureFrameSet( KWordDocument *_doc )
        : KWFrameSet( _doc )
    { image = 0L; }
    virtual ~KWPictureFrameSet()
    {; }

    virtual FrameType getFrameType()
    { return FT_PICTURE; }

    virtual void setImage( KWImage *_image )
    { image = _image; }
    void setFileName( QString _filename );
    void setFileName( QString _filename, QSize _imgSize );
    void setSize( QSize _imgSize );

    virtual KWImage* getImage()
    { return image; }
    QString getFileName() { return filename; }

    virtual QDomElement save( QDomDocument &doc );
    virtual void load( KOMLParser&, vector<KOMLAttrib>& );

protected:
    KWImage *image;
    QString filename;

};

/******************************************************************/
/* Class: KWPartFrameSet                                          */
/******************************************************************/

class KWPartFrameSet : public KWFrameSet
{
public:
    KWPartFrameSet( KWordDocument *_doc, KWordChild *_child );
    virtual ~KWPartFrameSet();

    virtual FrameType getFrameType()
    { return FT_PART; }

    virtual QPicture *getPicture();

    virtual void activate( QWidget *_widget, int diffx, int diffy, int diffxx );
    virtual void deactivate();

    void setView( KOffice::View_var kv )
    { view = KOffice::View::_narrow( kv ); }

    KWordChild *getChild() { return child; }

    virtual void update();

    void enableDrawing( bool f ) { _enableDrawing = f; }

protected:
    KWordFrame *frame;
    KWordChild *child;
    OpenParts::Id parentID;
    KOffice::View_var view;

    bool _enableDrawing;

};

/******************************************************************/
/* Class: KWGroupManager                                          */
/******************************************************************/

class KWGroupManager
{
public:
    struct Cell
    {
        KWFrameSet *frameSet;
        unsigned int row, col;
        unsigned int rows, cols;
    };

    KWGroupManager( KWordDocument *_doc ) : showHeaderOnAllPages( true ), hasTmpHeaders( false ), active( true )
    { doc = _doc; cells.setAutoDelete( true ); rows = 0; cols = 0; };

    void addFrameSet( KWFrameSet *fs, unsigned int row, unsigned int col );
    KWFrameSet *getFrameSet( unsigned int row, unsigned int col );
    bool getFrameSet( KWFrameSet *fs, unsigned int &row, unsigned int &col );

    bool isTableHeader( KWFrameSet *fs );

    void init( unsigned int x, unsigned int y, unsigned int width, unsigned int height );
    void init();
    void recalcCols();
    void recalcRows();

    unsigned int getRows() { return rows; }
    unsigned int getCols() { return cols; }

    QRect getBoundingRect();

    unsigned int getNumCells() { return cells.count(); }
    Cell *getCell( int i ) { return cells.at( i ); }
    Cell *getCell( unsigned int row, unsigned int col );

    bool hasSelectedFrame();

    void moveBy( unsigned int dx, unsigned int dy );
    void drawAllRects( QPainter &p, int xOffset, int yOffset );

    void deselectAll();

    void setName( QString _name ) { name = _name; }
    QString getName() { return name; }

    void selectUntil( KWFrameSet *fs );
    bool isOneSelected( KWFrameSet *fs, unsigned int &row, unsigned int &col );

    void insertRow( unsigned int _idx, bool _recalc = true, bool _removeable = false );
    void insertCol( unsigned int _idx );

    void deleteRow( unsigned int _idx, bool _recalc = true );
    void deleteCol( unsigned int _idx );

    void setShowHeaderOnAllPages( bool s )
    { showHeaderOnAllPages = s; }
    bool getShowHeaderOnAllPages()
    { return showHeaderOnAllPages; }

    void updateTempHeaders();
    bool hasTempHeaders() { return hasTmpHeaders; }

    void ungroup();

    bool isActive() { return active; }

    bool joinCells();
    bool splitCell();

protected:
    QList<Cell> cells;
    unsigned int rows, cols;
    KWordDocument *doc;
    QString name;
    bool showHeaderOnAllPages, hasTmpHeaders;
    bool active;

};

bool isAHeader( FrameInfo fi );
bool isAFooter( FrameInfo fi );
bool isAWrongHeader( FrameInfo fi, KoHFType t );
bool isAWrongFooter( FrameInfo fi, KoHFType t );


#endif
