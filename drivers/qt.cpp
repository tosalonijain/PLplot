/*

  This software is provided under the LGPL in March 2009 by the
  Cluster Science Centre
  QSAS team,
  Imperial College, London

  Copyright (C) 2009  Imperial College, London

  This is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Lesser Public License as published
  by the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  To received a copy of the GNU Library General Public License
  write to the Free Software Foundation, Inc., 
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
  
*/

#include "qt.h"
// #include "moc_qt.cxx"

PLDLLIMPEXP_DRIVER const char* plD_DEVICE_INFO_qt = 
#if defined(PLD_rasterqt)
  "rasterqt:Qt Raster driver:0:qt:66:rasterqt\n"
#endif
#if defined(PLD_svgqt)
  "svgqt:Qt SVG driver:0:qt:67:svgqt\n"
#endif
#if defined(PLD_epspdfqt)
  "epspdfqt:Qt EPS/PDF driver:0:qt:68:epspdfqt\n"
#endif
#if defined(PLD_qtwidget)
  "qtwidget:Qt Widget:0:qt:69:qtwidget\n"
#endif
;

#if defined(PLD_rasterqt)
void plD_dispatch_init_rasterqt(PLDispatchTable *pdt);
void plD_init_rasterqt(PLStream *);
void plD_eop_rasterqt(PLStream *);
#endif

#if defined(PLD_svgqt)
void plD_dispatch_init_svgqt(PLDispatchTable *pdt);
void plD_init_svgqt(PLStream *);
void plD_eop_svgqt(PLStream *);
#endif

#if defined(PLD_epspdfqt)
void plD_dispatch_init_epspdfqt(PLDispatchTable *pdt);
void plD_init_epspdfqt(PLStream *);
void plD_eop_epspdfqt(PLStream *);
#endif

#if defined(PLD_qtwidget)
void plD_dispatch_init_qtwidget(PLDispatchTable *pdt);
void plD_init_qtwidget(PLStream *);
void plD_eop_qtwidget(PLStream *);
#endif

void plD_line_qt(PLStream *, short, short, short, short);
void plD_polyline_qt(PLStream *, short*, short*, PLINT);
void plD_bop_qt(PLStream *){}
void plD_tidy_qt(PLStream *){}
void plD_state_qt(PLStream *, PLINT);
void plD_esc_qt(PLStream *, PLINT, void*);

///////////// Generic Qt driver class /////////////////

QtPLDriver::QtPLDriver(PLINT i_iWidth, PLINT i_iHeight)
{
	m_dWidth=i_iWidth;
	m_dHeight=i_iHeight;
	downscale=1.;
}

QtPLDriver::~QtPLDriver()
{}

void QtPLDriver::drawLine(short x1, short y1, short x2, short y2)
{
	QLineF line(	(PLFLT)x1*downscale,
			m_dHeight-(PLFLT)y1*downscale,
			(PLFLT)x2*downscale,
			m_dHeight-(PLFLT)y2*downscale
		   );

	m_painterP->drawLine(line);
}

void QtPLDriver::drawPolyline(short * x, short * y, PLINT npts)
{
	QPointF * polyline=new QPointF[npts];
	for(int i=0; i<npts; ++i)
	{
		polyline[i].setX((PLFLT)x[i]*downscale);
		polyline[i].setY(m_dHeight-(PLFLT)y[i]*downscale);
	}
	m_painterP->drawPolyline(polyline, npts);
	delete[] polyline;
}

void QtPLDriver::drawPolygon(short * x, short * y, PLINT npts)
{
	QPointF * polygon=new QPointF[npts];
	for(int i=0; i<npts; ++i)
	{
		polygon[i].setX((PLFLT)x[i]*downscale);
		polygon[i].setY(m_dHeight-(PLFLT)y[i]*downscale);
	}
	m_painterP->drawPolygon(polygon, npts);
	delete[] polygon;
}

void QtPLDriver::setColor(int r, int g, int b)
{
	QPen p=m_painterP->pen();
	p.setColor(QColor(r, g, b));
	m_painterP->setPen(p);
	
	QBrush B=m_painterP->brush();
	B.setColor(QColor(r, g, b));
	m_painterP->setBrush(B);
}

void QtPLDriver::setWidth(PLINT w)
{
	QPen p=m_painterP->pen();
	p.setWidth(w);
	m_painterP->setPen(p);
}

void QtPLDriver::setDashed(PLINT nms, PLINT* mark, PLINT* space)
{
	// 72 dpi
	// => 72/25400 = 0.00283 dot for 1 micron
	QVector<qreal> vect;
	for(int i=0; i<nms; ++i)
	{
		vect << (PLFLT)mark[i]*0.00283;
		vect << (PLFLT)space[i]*0.00283;
	}
	QPen p=m_painterP->pen();
	p.setDashPattern(vect);
	m_painterP->setPen(p);
}

void QtPLDriver::setSolid()
{
	QPen p=m_painterP->pen();
	p.setStyle(Qt::SolidLine);
	m_painterP->setPen(p);
}

void QtPLDriver::savePlot(char* fileName)
{
}

//////////// Buffered driver ///////////////////////

QtPLBufferedDriver::QtPLBufferedDriver(PLINT i_iWidth, PLINT i_iHeight):
	QtPLDriver(i_iWidth, i_iHeight)
{}

QtPLBufferedDriver::~QtPLBufferedDriver()
{
	clearBuffer();
}

void QtPLBufferedDriver::clearBuffer()
{
	for(QLinkedList<BufferElement>::iterator i=m_listBuffer.begin(); i!=m_listBuffer.end(); ++i)

	{
		switch(i->Element)
		{
			case POLYLINE:
			case POLYGON:
				delete[] i->Data.PolylineStruct.x;
				delete[] i->Data.PolylineStruct.y;
				break;
                
			case SET_DASHED:
				delete[] i->Data.PolylineStruct.x;
				break;
            
			default:
				break;
		}
	}
    
	m_listBuffer.clear();
}

void QtPLBufferedDriver::drawLine(short x1, short y1, short x2, short y2)
{
	BufferElement el;
	el.Element=LINE;
	el.Data.LineStruct.x1=(PLFLT)x1*downscale;
	el.Data.LineStruct.y1=m_dHeight-(PLFLT)(y1*downscale);
	el.Data.LineStruct.x2=(PLFLT)x2*downscale;
	el.Data.LineStruct.y2=m_dHeight-(PLFLT)(y2*downscale);
    
	m_listBuffer.append(el);
}

void QtPLBufferedDriver::drawPolyline(short * x, short * y, PLINT npts)
{
	BufferElement el;
	el.Element=POLYLINE;
	el.Data.PolylineStruct.npts=(PLINT)npts;
	el.Data.PolylineStruct.x=new PLFLT[npts];
	el.Data.PolylineStruct.y=new PLFLT[npts];
	for(int i=0; i<npts; ++i)
	{
		el.Data.PolylineStruct.x[i]=(PLFLT)(x[i])*downscale;
		el.Data.PolylineStruct.y[i]=m_dHeight-(PLFLT)((y[i])*downscale);
	}
    
	m_listBuffer.append(el);
}

void QtPLBufferedDriver::drawPolygon(short * x, short * y, PLINT npts)
{
	BufferElement el;
	el.Element=POLYGON;
	el.Data.PolylineStruct.npts=(PLINT)npts;
	el.Data.PolylineStruct.x=new PLFLT[npts];
	el.Data.PolylineStruct.y=new PLFLT[npts];
	for(int i=0; i<npts; ++i)
	{
		el.Data.PolylineStruct.x[i]=(PLFLT)(x[i])*downscale;
		el.Data.PolylineStruct.y[i]=m_dHeight-(PLFLT)((y[i])*downscale);
	}
	if(el.Data.PolylineStruct.x[0]==el.Data.PolylineStruct.x[npts-1] && el.Data.PolylineStruct.y[0]==el.Data.PolylineStruct.y[npts-1])
	{
		--el.Data.PolylineStruct.npts;
	}
    
	m_listBuffer.append(el);
}

void QtPLBufferedDriver::setColor(int r, int g, int b)
{
	BufferElement el;
	el.Element=SET_COLOUR;
	el.Data.ColourStruct.R=r;
	el.Data.ColourStruct.G=g;
	el.Data.ColourStruct.B=b;
    
	m_listBuffer.append(el);
}

void QtPLBufferedDriver::setWidth(PLINT w)
{
	BufferElement el;
	el.Element=SET_WIDTH;
	el.Data.intParam=w;
	m_listBuffer.append(el);
}

void QtPLBufferedDriver::setDashed(PLINT nms, PLINT* mark, PLINT* space)
{
	BufferElement el;
	el.Element=SET_DASHED;
	el.Data.PolylineStruct.npts=2*nms;
	el.Data.PolylineStruct.x=new PLFLT[2*nms];
	for(int i=0; i<nms; i++)
	{
		el.Data.PolylineStruct.x[2*i]=(PLFLT)mark[i]*0.00283;
		el.Data.PolylineStruct.x[2*i+1]=(PLFLT)space[i]*0.00283;
	}
    
	m_listBuffer.append(el);
}

void QtPLBufferedDriver::setSolid()
{
	BufferElement el;
	el.Element=SET_SOLID;
	m_listBuffer.append(el);
}

void QtPLBufferedDriver::savePlot(char* fileName)
{
}

void QtPLBufferedDriver::doPlot(QPainter* i_painterP, double x_fact, double y_fact, double x_offset, double y_offset)
{
	QLineF line;
	QPointF * polyline;
	PLINT npts;
	QVector<qreal> vect;
	
	if(i_painterP==NULL) i_painterP=m_painterP;
	
	QPen p=i_painterP->pen();
	QBrush b=i_painterP->brush();
	b.setStyle(Qt::SolidPattern);
	i_painterP->setBrush(b);
	
	i_painterP->fillRect(x_offset, y_offset, m_dWidth*x_fact, m_dHeight*y_fact, QBrush(Qt::black));
	
	if(m_listBuffer.empty())
	{
		return;
	}
	for(QLinkedList<BufferElement>::const_iterator i=m_listBuffer.begin(); i!=m_listBuffer.end(); ++i)
	{
		switch(i->Element)
		{
			case LINE:
				line=QLineF(i->Data.LineStruct.x1*x_fact+x_offset, i->Data.LineStruct.y1*y_fact+y_offset, i->Data.LineStruct.x2*x_fact+x_offset, i->Data.LineStruct.y2*y_fact+y_offset);
				i_painterP->drawLine(line);
				break;
            
			case POLYLINE:
				npts=i->Data.PolylineStruct.npts;
				polyline=new QPointF[npts];
				for(int j=0; j<npts; ++j)
				{
					polyline[j].setX(i->Data.PolylineStruct.x[j]*x_fact+x_offset);
					polyline[j].setY(i->Data.PolylineStruct.y[j]*y_fact+y_offset);
				}
				i_painterP->drawPolyline(polyline, npts);
				delete[] polyline;
				break;
                
			case POLYGON:
				npts=i->Data.PolylineStruct.npts;
				polyline=new QPointF[npts];
				for(int j=0; j<npts; ++j)
				{
					polyline[j].setX(i->Data.PolylineStruct.x[j]*x_fact+x_offset);
					polyline[j].setY(i->Data.PolylineStruct.y[j]*y_fact+y_offset);
				}
				i_painterP->drawPolygon(polyline, npts);

				delete[] polyline;
				break;
                
			case SET_WIDTH:
				
				p.setWidthF(i->Data.intParam);
				i_painterP->setPen(p);
				break;
                
			case SET_COLOUR:
				p.setColor(QColor(i->Data.ColourStruct.R, i->Data.ColourStruct.G, i->Data.ColourStruct.B));
				i_painterP->setPen(p);
				b.setColor(QColor(i->Data.ColourStruct.R, i->Data.ColourStruct.G, i->Data.ColourStruct.B));
				i_painterP->setBrush(b);
				break;
                
			case SET_DASHED:
				vect.clear();
				for(int j=0; j<i->Data.PolylineStruct.npts; j++)
				{
					if(i->Data.PolylineStruct.x[j]>=1.) //TODO remove that when bug #228554 of Qt is solved on Windows...
						vect << i->Data.PolylineStruct.x[j];
					else
						vect <<1.;
				}
				p.setDashPattern(vect);
				i_painterP->setPen(p);
				break;
                
			case SET_SOLID:
				p.setStyle(Qt::SolidLine);
				i_painterP->setPen(p);
				break;
			
			case SET_SMOOTH:
				i_painterP->setRenderHint(QPainter::Antialiasing, i->Data.intParam);
				break;
                
			default:
				break;
		}
	}
}

void QtPLBufferedDriver::getPlotParameters(double & io_dXFact, double & io_dYFact, double & io_dXOffset, double & io_dYOffset)
{
	io_dXFact=1.;
	io_dYFact=1.;
	io_dXOffset=0.;
	io_dYOffset=0.;
}

//////////////////// Generic driver interface ///////////////

void plD_line_qt(PLStream * pls, short x1a, short y1a, short x2a, short y2a)
{
	QtPLDriver * widget=NULL;
#if defined(PLD_rasterqt)
	if(widget==NULL) widget=dynamic_cast<QtRasterDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_svgqt)
	if(widget==NULL) widget=dynamic_cast<QtSVGDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_epspdfqt)
	if(widget==NULL) widget=dynamic_cast<QtEPSDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_qtwidget)
	if(widget==NULL) widget=dynamic_cast<QtPLWidget*>((QWidget *) pls->dev);
#endif
	if(widget==NULL) return;
	
	widget->setColor(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b);
	widget->drawLine(x1a, y1a, x2a, y2a);
}

void plD_polyline_qt(PLStream *pls, short *xa, short *ya, PLINT npts)
{
	QtPLDriver * widget=NULL;
#if defined(PLD_rasterqt)
	if(widget==NULL) widget=dynamic_cast<QtRasterDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_svgqt)
	if(widget==NULL) widget=dynamic_cast<QtSVGDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_epspdfqt)
	if(widget==NULL) widget=dynamic_cast<QtEPSDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_qtwidget)
	if(widget==NULL) widget=dynamic_cast<QtPLWidget*>((QWidget *) pls->dev);
#endif
	if(widget==NULL) return;
	
	widget->setColor(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b);
	widget->drawPolyline(xa, ya, npts);
}

void plD_esc_qt(PLStream * pls, PLINT op, void* ptr)
{
	short *xa, *ya;
	PLINT i, j;
	QtPLDriver * widget=NULL;
#if defined(PLD_rasterqt)
	if(widget==NULL) widget=dynamic_cast<QtRasterDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_svgqt)
	if(widget==NULL) widget=dynamic_cast<QtSVGDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_epspdfqt)
	if(widget==NULL) widget=dynamic_cast<QtEPSDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_qtwidget)
	if(widget==NULL) widget=dynamic_cast<QtPLWidget*>((QWidget *) pls->dev);
#endif
	if(widget==NULL) return;
		    
	switch(op)
	{
		case PLESC_DASH:
			widget->setDashed(pls->nms, pls->mark, pls->space);
			widget->setColor(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b);
			widget->drawPolyline(pls->dev_x, pls->dev_y, pls->dev_npts);
			widget->setSolid();
			break;
        
		case PLESC_FILL:
			xa=new short[pls->dev_npts];
			ya=new short[pls->dev_npts];
            
			for (i = 0; i < pls->dev_npts; i++)
			{
				xa[i] = pls->dev_x[i];
				ya[i] = pls->dev_y[i];
			}
			widget->setColor(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b);
			widget->drawPolygon(xa, ya, pls->dev_npts);
            
			delete[] xa;
			delete[] ya;
			break;
            
			default: break;
	}
}

void plD_state_qt(PLStream * pls, PLINT op)
{
	QtPLDriver * widget=NULL;
#if defined(PLD_rasterqt)
	if(widget==NULL) widget=dynamic_cast<QtRasterDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_svgqt)
	if(widget==NULL) widget=dynamic_cast<QtSVGDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_epspdfqt)
	if(widget==NULL) widget=dynamic_cast<QtEPSDevice*>((QtPLDriver *) pls->dev);
#endif
#if defined(PLD_qtwidget)
	if(widget==NULL) widget=dynamic_cast<QtPLWidget*>((QWidget *) pls->dev);
#endif
	if(widget==NULL) return;
    
	switch(op)
	{
		case PLSTATE_WIDTH:
			widget->setWidth(pls->width);
			break;
       
		case PLSTATE_COLOR0:
			widget->setColor(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b);
			break;
			
		case PLSTATE_COLOR1:
			widget->setColor(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b);
			break;

            
			default: break;
	}
}

#if defined(PLD_rasterqt)
QtRasterDevice::QtRasterDevice(int i_iWidth, int i_iHeight):
	QtPLDriver(i_iWidth, i_iHeight),
	QImage(i_iWidth, i_iHeight, QImage::Format_RGB32)
{
	m_painterP=new QPainter(this);
	QBrush b=m_painterP->brush();
	b.setStyle(Qt::SolidPattern);
	m_painterP->setBrush(b);
	m_painterP->setRenderHint(QPainter::Antialiasing, true);
	m_painterP->fillRect(0, 0, width(), height(), QBrush(Qt::black));
	pageCounter=0;
}

QString QtRasterDevice::getFileName(char* fileName)
{
	QString fn(fileName);
			
	if(pageCounter==0) return fn;
			
	int pos=fn.lastIndexOf(".");
	if(pos<0) pos=fn.length();
	QString res=fn.insert(pos, QString("_page%1").arg(pageCounter+1));
	return res;
}

void QtRasterDevice::savePlot(char* fileName)
{
	m_painterP->end();
	save(getFileName(fileName), 0, 85);
			
	++pageCounter;
	m_painterP->begin(this);
	m_painterP->setRenderHint(QPainter::Antialiasing, true);
	QBrush b=m_painterP->brush();
	b.setStyle(Qt::SolidPattern);
	m_painterP->setBrush(b);
	m_painterP->fillRect(0, 0, width(), height(), QBrush(Qt::black));
}

void plD_dispatch_init_rasterqt(PLDispatchTable *pdt)
{
#ifndef ENABLE_DYNDRIVERS
	pdt->pl_MenuStr  = "Qt Raster Driver";
	pdt->pl_DevName  = "rasterqt";
#endif
	pdt->pl_type     = plDevType_FileOriented;
	pdt->pl_seq      = 66;
	pdt->pl_init     = (plD_init_fp)     plD_init_rasterqt;
	pdt->pl_line     = (plD_line_fp)     plD_line_qt;
	pdt->pl_polyline = (plD_polyline_fp) plD_polyline_qt;
	pdt->pl_eop      = (plD_eop_fp)      plD_eop_rasterqt;
	pdt->pl_bop      = (plD_bop_fp)      plD_bop_qt;
	pdt->pl_tidy     = (plD_tidy_fp)     plD_tidy_qt;
	pdt->pl_state    = (plD_state_fp)    plD_state_qt;
	pdt->pl_esc      = (plD_esc_fp)      plD_esc_qt;
}

void plD_init_rasterqt(PLStream * pls)
{
	/* Stream setup */
	pls->color = 1;
	pls->plbuf_write=0;
	pls->dev_fill0 = 1;
	pls->dev_fill1 = 0;
	pls->dev_dash=1;
	pls->dev_flush=1;
	pls->dev_clear=1;
	pls->termin=0;
	
	pls->dev=new QtRasterDevice;
	
	if (pls->xlength <= 0 || pls->ylength <= 0)
	{
		pls->xlength = ((QtRasterDevice*)(pls->dev))->m_dWidth;
		pls->ylength = ((QtRasterDevice*)(pls->dev))->m_dHeight;
	}
	
	if (pls->xlength > pls->ylength)
		((QtRasterDevice*)(pls->dev))->downscale = (PLFLT)pls->xlength/(PLFLT)(PIXELS_X-1);
	else
		((QtRasterDevice*)(pls->dev))->downscale = (PLFLT)pls->ylength/(PLFLT)PIXELS_Y;
	
	plP_setphy((PLINT) 0, (PLINT) (pls->xlength / ((QtRasterDevice*)(pls->dev))->downscale), (PLINT) 0, (PLINT) (pls->ylength / ((QtRasterDevice*)(pls->dev))->downscale));
	
	plP_setpxl(DPI/25.4/((QtRasterDevice*)(pls->dev))->downscale, DPI/25.4/((QtRasterDevice*)(pls->dev))->downscale);
	
	printf("The file format will be determined by the file name extension\n");
	printf("Possible extensions are: bmp, jpg, png, ppm, tiff, xbm, xpm\n");
	plOpenFile(pls);
}

void plD_eop_rasterqt(PLStream *pls)
{
	((QtRasterDevice *)pls->dev)->savePlot(pls->FileName);
}
#endif


#if defined(PLD_svgqt)
QtSVGDevice::QtSVGDevice(int i_iWidth, int i_iHeight):
	QtPLBufferedDriver(i_iWidth, i_iHeight)
{
	setSize(QSize(m_dWidth, m_dHeight));
	pageCounter=0;
}

QtSVGDevice::~QtSVGDevice()
{
	clearBuffer();
}

QString QtSVGDevice::getFileName(char* fileName)
{
	QString fn(fileName);
			
	if(pageCounter==0) return fn;
			
	int pos=fn.lastIndexOf(".");
	if(pos<0) pos=fn.length();
	QString res=fn.insert(pos, QString("_page%1").arg(pageCounter+1));
	return res;
}

void QtSVGDevice::savePlot(char* fileName)
{
	setFileName(getFileName(fileName));
	setResolution(72);

	m_painterP=new QPainter(this);
	doPlot();
	m_painterP->end();
}

void plD_dispatch_init_svgqt(PLDispatchTable *pdt)
{
#ifndef ENABLE_DYNDRIVERS
	pdt->pl_MenuStr  = "Qt SVG Driver";
	pdt->pl_DevName  = "svgqt";
#endif
	pdt->pl_type     = plDevType_FileOriented;
	pdt->pl_seq      = 67;
	pdt->pl_init     = (plD_init_fp)     plD_init_svgqt;
	pdt->pl_line     = (plD_line_fp)     plD_line_qt;
	pdt->pl_polyline = (plD_polyline_fp) plD_polyline_qt;
	pdt->pl_eop      = (plD_eop_fp)      plD_eop_svgqt;
	pdt->pl_bop      = (plD_bop_fp)      plD_bop_qt;
	pdt->pl_tidy     = (plD_tidy_fp)     plD_tidy_qt;
	pdt->pl_state    = (plD_state_fp)    plD_state_qt;
	pdt->pl_esc      = (plD_esc_fp)      plD_esc_qt;
}

void plD_init_svgqt(PLStream * pls)
{
	/* Stream setup */
	pls->color = 1;
	pls->plbuf_write=0;
	pls->dev_fill0 = 1;
	pls->dev_fill1 = 0;
	pls->dev_dash=1;
	pls->dev_flush=1;
	pls->dev_clear=1;
	pls->termin=0;
	
	pls->dev=new QtSVGDevice;
	
	if (pls->xlength <= 0 || pls->ylength <= 0)
	{
		pls->xlength = ((QtSVGDevice*)(pls->dev))->m_dWidth;
		pls->ylength = ((QtSVGDevice*)(pls->dev))->m_dHeight;
	}
	
	if (pls->xlength > pls->ylength)
		((QtSVGDevice*)(pls->dev))->downscale = (PLFLT)pls->xlength/(PLFLT)(PIXELS_X-1);
	else
		((QtSVGDevice*)(pls->dev))->downscale = (PLFLT)pls->ylength/(PLFLT)PIXELS_Y;
	
	plP_setphy((PLINT) 0, (PLINT) (pls->xlength / ((QtSVGDevice*)(pls->dev))->downscale), (PLINT) 0, (PLINT) (pls->ylength / ((QtSVGDevice*)(pls->dev))->downscale));
	
	plP_setpxl(DPI/25.4/((QtSVGDevice*)(pls->dev))->downscale, DPI/25.4/((QtSVGDevice*)(pls->dev))->downscale);
	
	plOpenFile(pls);
}

void plD_eop_svgqt(PLStream *pls)
{
	double downscale;
	int pageCounter;
	QSize s;
	
	((QtSVGDevice *)pls->dev)->savePlot(pls->FileName);
	downscale=((QtSVGDevice *)pls->dev)->downscale;
	pageCounter=((QtSVGDevice *)pls->dev)->pageCounter;
	s=((QtSVGDevice *)pls->dev)->size();
	delete ((QtSVGDevice *)pls->dev);
	
	pls->dev=new QtSVGDevice(s.width(), s.height());
	((QtSVGDevice *)pls->dev)->downscale=downscale;
	((QtSVGDevice *)pls->dev)->pageCounter=pageCounter+1;
}

#endif

#if defined (PLD_epspdfqt)
QtEPSDevice::QtEPSDevice()
{
	pageCounter=0;
	setPageSize(QPrinter::A4);
	setResolution(DPI);
	setColorMode(QPrinter::Color);
	setOrientation(QPrinter::Landscape);
			
	m_dWidth=pageRect().width();
	m_dHeight=pageRect().height();
}

QtEPSDevice::~QtEPSDevice()
{
	clearBuffer();
}

QString QtEPSDevice::getFileName(char* fileName)
{
	QString fn(fileName);
			
	if(pageCounter==0) return fn;
			
	int pos=fn.lastIndexOf(".");
	if(pos<0) pos=fn.length();
	QString res=fn.insert(pos, QString("_page%1").arg(pageCounter+1));
	return res;
}

void QtEPSDevice::savePlot(char* fileName)
{
	setOutputFileName(getFileName(fileName));
	if(QString(fileName).endsWith(".ps") || QString(fileName).endsWith(".eps"))
	{
		setOutputFormat(QPrinter::PostScriptFormat);
	}
	else if(QString(fileName).endsWith(".pdf"))
	{
		setOutputFormat(QPrinter::PdfFormat);
	}
	else
	{
		std::cerr << "Unhandled file format: " << fileName << std::endl;
		return;
	}
				
	m_painterP=new QPainter(this);
	doPlot();
	m_painterP->end();
}

void plD_dispatch_init_epspdfqt(PLDispatchTable *pdt)
{
#ifndef ENABLE_DYNDRIVERS
	pdt->pl_MenuStr  = "Qt EPS/PDF Driver";
	pdt->pl_DevName  = "epspdfqt";
#endif
	pdt->pl_type     = plDevType_FileOriented;
	pdt->pl_seq      = 68;
	pdt->pl_init     = (plD_init_fp)     plD_init_epspdfqt;
	pdt->pl_line     = (plD_line_fp)     plD_line_qt;
	pdt->pl_polyline = (plD_polyline_fp) plD_polyline_qt;
	pdt->pl_eop      = (plD_eop_fp)      plD_eop_epspdfqt;
	pdt->pl_bop      = (plD_bop_fp)      plD_bop_qt;
	pdt->pl_tidy     = (plD_tidy_fp)     plD_tidy_qt;
	pdt->pl_state    = (plD_state_fp)    plD_state_qt;
	pdt->pl_esc      = (plD_esc_fp)      plD_esc_qt;
}

void plD_init_epspdfqt(PLStream * pls)
{
	/* Stream setup */
	pls->color = 1;
	pls->plbuf_write=0;
	pls->dev_fill0 = 1;
	pls->dev_fill1 = 0;
	pls->dev_dash=1;
	pls->dev_flush=1;
	pls->dev_clear=1;
	pls->termin=0;
	
	int argc=0;
	char argv[]={'\0'};
	QApplication * app=new QApplication(argc, (char**)&argv);
	pls->dev=new QtEPSDevice;
	delete app;
	
	if (pls->xlength <= 0 || pls->ylength <= 0)
	{
		pls->xlength = ((QtSVGDevice*)(pls->dev))->m_dWidth;
		pls->ylength = ((QtSVGDevice*)(pls->dev))->m_dHeight;
	}
	
	if (pls->xlength > pls->ylength)
		((QtEPSDevice*)(pls->dev))->downscale = (PLFLT)pls->xlength/(PLFLT)(PIXELS_X-1);
	else
		((QtEPSDevice*)(pls->dev))->downscale = (PLFLT)pls->ylength/(PLFLT)PIXELS_Y;
	
	plP_setphy((PLINT) 0, (PLINT) (pls->xlength / ((QtEPSDevice*)(pls->dev))->downscale), (PLINT) 0, (PLINT) (pls->ylength / ((QtEPSDevice*)(pls->dev))->downscale));
	
	plP_setpxl(DPI/25.4/((QtEPSDevice*)(pls->dev))->downscale, DPI/25.4/((QtEPSDevice*)(pls->dev))->downscale);
	
	printf("The file format will be determined by the file name extension\n");
	printf("Possible extensions are: eps, pdf\n");
	plOpenFile(pls);
}

void plD_eop_epspdfqt(PLStream *pls)
{
	double downscale;
	int pageCounter;
	
	int argc=0;
	char argv[]={'\0'};
	
	((QtEPSDevice *)pls->dev)->savePlot(pls->FileName);
	downscale=((QtSVGDevice *)pls->dev)->downscale;
	pageCounter=((QtSVGDevice *)pls->dev)->pageCounter;
	delete ((QtEPSDevice *)pls->dev);
	
	QApplication * app=new QApplication(argc, (char**)&argv);
	pls->dev=new QtEPSDevice;
	((QtEPSDevice *)pls->dev)->downscale=downscale;
	((QtEPSDevice *)pls->dev)->pageCounter=pageCounter+1;
	delete app;
}

#if defined (PLD_qtwidget)

QtPLWidget::QtPLWidget(int i_iWidth, int i_iHeight, QWidget* parent):
	 QWidget(parent), QtPLBufferedDriver(i_iWidth, i_iHeight)
{
	m_painterP=new QPainter;
	
	m_dAspectRatio=(double)i_iWidth/(double)i_iHeight;
	
	cursorParameters.isTracking=false;
	m_pixPixmap=NULL;
	m_iOldSize=0;
	
	resize(i_iWidth, i_iHeight);

}

QtPLWidget::~QtPLWidget()
{
	clearBuffer();
	if(m_pixPixmap!=NULL) delete m_pixPixmap;
}

void QtPLWidget::setSmooth(bool b)
{
	BufferElement el;
	el.Element=SET_SMOOTH;
	el.Data.intParam=b;
	m_listBuffer.append(el);
}

void QtPLWidget::clearWidget()
{
	clearBuffer();
	m_bAwaitingRedraw=true;
	update();
}

void QtPLWidget::captureMousePlotCoords(double * xi, double* yi, double * xf, double * yf)
{
	setMouseTracking(true);
	cursorParameters.isTracking=true;
	cursorParameters.cursor_start_x=
		cursorParameters.cursor_start_y=
		cursorParameters.cursor_end_x=
		cursorParameters.cursor_end_y=-1.;
	cursorParameters.step=1;
	do
	{
		QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
        
	} while(cursorParameters.isTracking);
    
	PLFLT a,b;
	PLINT c;
	plcalc_world(cursorParameters.cursor_start_x, 1.-cursorParameters.cursor_start_y, &a, &b, &c);
	*xi=a;
	*yi=b;
	plcalc_world(cursorParameters.cursor_end_x, 1.-cursorParameters.cursor_end_y, &a, &b, &c);
	*xf=a;
	*yf=b;
    
}

void QtPLWidget::captureMouseDeviceCoords(double * xi, double* yi, double * xf, double * yf)
{
	setMouseTracking(true);
	cursorParameters.isTracking=true;
	cursorParameters.cursor_start_x=
			cursorParameters.cursor_start_y=
			cursorParameters.cursor_end_x=
			cursorParameters.cursor_end_y=-1.;
	cursorParameters.step=1;
	do
	{
		QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
        
	} while(cursorParameters.isTracking);
    
	*xi=cursorParameters.cursor_start_x;
	*yi=cursorParameters.cursor_start_y;
	*xf=cursorParameters.cursor_end_x;
	*yf=cursorParameters.cursor_end_y;
}

void QtPLWidget::resizeEvent( QResizeEvent * )
{
	m_bAwaitingRedraw=true;
	if(m_pixPixmap!=NULL)
	{
		delete m_pixPixmap;
		m_pixPixmap=NULL;
	}
}

void QtPLWidget::paintEvent( QPaintEvent * )
{
	double x_fact, y_fact, x_offset(0.), y_offset(0.); //Parameters to scale and center the plot on the widget
	getPlotParameters(x_fact, y_fact, x_offset, y_offset);
	
	// If actual redraw
	if(m_bAwaitingRedraw || m_pixPixmap==NULL || m_listBuffer.size()!=m_iOldSize  )
	{
		if(m_pixPixmap!=NULL) delete m_pixPixmap;
		m_pixPixmap=new QPixmap(width(), height());
		QPainter * painter=new QPainter;
		painter->begin(m_pixPixmap);
		painter->fillRect(0, 0, width(), height(), QBrush(Qt::white));
		painter->fillRect(0, 0, width(), height(), QBrush(Qt::gray, Qt::Dense4Pattern));
		painter->fillRect((int)x_offset, (int)y_offset, (int)floor(x_fact*m_dWidth+0.5), (int)floor(y_fact*m_dHeight+0.5), QBrush(Qt::black));
		doPlot(painter, x_fact, y_fact, x_offset, y_offset);
		painter->end();
		m_bAwaitingRedraw=false;
		m_iOldSize=m_listBuffer.size();
	}
	
	m_painterP->begin(this);
	
	// repaint plot
	m_painterP->drawPixmap(0, 0, *m_pixPixmap);

	// now paint the cursor tracking patterns
	if(cursorParameters.isTracking)
	{
		QPen p=m_painterP->pen();
		p.setColor(Qt::white);
		if(cursorParameters.step==1)
		{
			m_painterP->setPen(p);
			m_painterP->drawLine((int)(cursorParameters.cursor_start_x*x_fact*m_dWidth+x_offset), 0, (int)(cursorParameters.cursor_start_x*x_fact*m_dWidth+x_offset), height());
			m_painterP->drawLine(0, (int)(cursorParameters.cursor_start_y*y_fact*m_dHeight+y_offset), width(), (int)(cursorParameters.cursor_start_y*y_fact*m_dHeight+y_offset));
		}
		else
		{
			p.setStyle(Qt::DotLine);
			m_painterP->setPen(p);
			m_painterP->drawLine((int)(cursorParameters.cursor_start_x*x_fact*m_dWidth+x_offset), 0, (int)(cursorParameters.cursor_start_x*x_fact*m_dWidth+x_offset), height());
			m_painterP->drawLine(0, (int)(cursorParameters.cursor_start_y*y_fact*m_dHeight+y_offset), width(), (int)(cursorParameters.cursor_start_y*y_fact*m_dHeight+y_offset));
			p.setStyle(Qt::SolidLine);
			m_painterP->setPen(p);
			m_painterP->drawLine((int)(cursorParameters.cursor_end_x*x_fact*m_dWidth+x_offset), 0, (int)(cursorParameters.cursor_end_x*x_fact*m_dWidth+x_offset), height());
			m_painterP->drawLine(0, (int)(cursorParameters.cursor_end_y*y_fact*m_dHeight+y_offset), width(), (int)(cursorParameters.cursor_end_y*y_fact*m_dHeight+y_offset));
		}
	}
    
	m_painterP->end();
}

void QtPLWidget::mousePressEvent(QMouseEvent* event)
{
	if(!cursorParameters.isTracking) return;
    
	double x_fact, y_fact, x_offset, y_offset; //Parameters to scale and center the plot on the widget
    
	getPlotParameters(x_fact, y_fact, x_offset, y_offset);
	
	PLFLT X=(PLFLT)(event->x()-x_offset)/(m_dWidth*x_fact);
	PLFLT Y=(PLFLT)(event->y()-y_offset)/(m_dHeight*y_fact);
	
	if(cursorParameters.step==1)
	{
		cursorParameters.cursor_start_x=X;
		cursorParameters.cursor_start_y=Y;
		cursorParameters.step=2; // First step of selection done, going to the next one
		update();
	}
}

void QtPLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	double x_fact, y_fact, x_offset, y_offset; //Parameters to scale and center the plot on the widget
    
	getPlotParameters(x_fact, y_fact, x_offset, y_offset);
	
	PLFLT X=(PLFLT)(event->x()-x_offset)/(m_dWidth*x_fact);
	PLFLT Y=(PLFLT)(event->y()-y_offset)/(m_dHeight*y_fact);
    
	if(cursorParameters.step!=1)
	{
		cursorParameters.cursor_end_x=X;
		cursorParameters.cursor_end_y=Y;
		cursorParameters.isTracking=false;
		setMouseTracking(false);
		update();
	}
}

void QtPLWidget::mouseMoveEvent(QMouseEvent* event)
{
	this->activateWindow();
	this->raise();
	
	if(!cursorParameters.isTracking) return;
    
	double x_fact, y_fact, x_offset, y_offset; //Parameters to scale and center the plot on the widget
    
	getPlotParameters(x_fact, y_fact, x_offset, y_offset);
	
	PLFLT X=(PLFLT)(event->x()-x_offset)/(m_dWidth*x_fact);
	PLFLT Y=(PLFLT)(event->y()-y_offset)/(m_dHeight*y_fact);
    
	if(cursorParameters.step==1)
	{
		cursorParameters.cursor_start_x=X;
		cursorParameters.cursor_start_y=Y;
	}
	else
	{
		cursorParameters.cursor_end_x=X;
		cursorParameters.cursor_end_y=Y;
	}

	update();
}

void QtPLWidget::getPlotParameters(double & io_dXFact, double & io_dYFact, double & io_dXOffset, double & io_dYOffset)
{
	double w=(double)width();
	double h=(double)height();
	
	if(w/h>m_dAspectRatio) //Too wide, h is the limitating factor
	{
		io_dYFact=h/m_dHeight;
		io_dXFact=h*m_dAspectRatio/m_dWidth;//io_dYFact*m_dAspectRatio;
		io_dYOffset=0.;
		io_dXOffset=(w-io_dXFact*m_dWidth)/2.;
	}
	else
	{
		io_dXFact=w/m_dWidth;
		io_dYFact=w/m_dAspectRatio/m_dHeight;//io_dXFact/m_dAspectRatio;
		io_dXOffset=0.;
		io_dYOffset=(h-io_dYFact*m_dHeight)/2.;
	}
}


void plD_dispatch_init_qtwidget(PLDispatchTable *pdt)
{
#ifndef ENABLE_DYNDRIVERS
	pdt->pl_MenuStr  = "Qt Widget";
	pdt->pl_DevName  = "qtwidget";
#endif
	pdt->pl_type     = plDevType_Interactive;
	pdt->pl_seq      = 69;
	pdt->pl_init     = (plD_init_fp)     plD_init_qtwidget;
	pdt->pl_line     = (plD_line_fp)     plD_line_qt;
	pdt->pl_polyline = (plD_polyline_fp) plD_polyline_qt;
	pdt->pl_eop      = (plD_eop_fp)      plD_eop_qtwidget;
	pdt->pl_bop      = (plD_bop_fp)      plD_bop_qt;
	pdt->pl_tidy     = (plD_tidy_fp)     plD_tidy_qt;
	pdt->pl_state    = (plD_state_fp)    plD_state_qt;
	pdt->pl_esc      = (plD_esc_fp)      plD_esc_qt;
}

void plsqtdev(QtPLWidget* dev)
{
	if(dev==NULL) return;
	
	PLINT w, h;
	plsc->dev = (void*)dev;
	plsc->xlength = dev->m_dWidth;
	plsc->ylength = dev->m_dHeight;
	
	if (plsc->xlength > plsc->ylength)
		dev->downscale = (PLFLT)plsc->xlength/(PLFLT)(PIXELS_X-1);
	else
		dev->downscale = (PLFLT)plsc->ylength/(PLFLT)PIXELS_Y;
	
	plP_setphy((PLINT) 0, (PLINT) (plsc->xlength / dev->downscale), (PLINT) 0, (PLINT) (plsc->ylength / dev->downscale));
	
	plP_setpxl(DPI/25.4/dev->downscale, DPI/25.4/dev->downscale);
}

void plD_init_qtwidget(PLStream * pls)
{
	if ((pls->phyxma == 0) || (pls->dev == NULL)) {
		plexit("Must call plsqtdev first to set user plotting widget!");
	}

	pls->color = 1;		/* Is a color device */
	pls->plbuf_write=0;
	pls->dev_fill0 = 1;	/* Handle solid fills */
	pls->dev_fill1 = 0;
	pls->dev_dash=1;
	pls->dev_flush=1;
	pls->dev_clear=1;
	pls->termin=1;
}

void plD_eop_qtwidget(PLStream *pls)
{
	((QtPLWidget *)pls->dev)->clearWidget();
}
#endif

#endif
