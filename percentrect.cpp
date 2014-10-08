/*
* DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL DAVID I. MARSH BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the State of Utah,
* without reference to its choice of law rules.
*/


#include "percentrect.h"
#include "ui_percentrect.h"

#include <QPainter>


PercentRect::PercentRect(float percent, int rowHeight, int percentRectWidth, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PercentRect),
    m_percentRectWidth (percentRectWidth),
    m_percent (percent),
    m_rowHeight(rowHeight)
{
    ui->setupUi(this);
}

PercentRect::~PercentRect()
{
    delete ui;
}


void PercentRect::paintEvent(QPaintEvent *event)
{
	//create a QPainter and pass a pointer to the device.
	//A paint device can be a QWidget, a QPixmap or a QImage
	QPainter painter(this);
/*
	//a simple line
	painter.drawLine(1,1,100,100);

	//create a black pen that has solid line
	//and the width is 2.
	QPen myPen(Qt::black, 2, Qt::SolidLine);
	painter.setPen(myPen);
	painter.drawLine(100,100,100,1);

	//draw a point
	myPen.setColor(Qt::red);
	painter.drawPoint(110,110);

	//draw a polygon
	QPolygon polygon;
	polygon << QPoint(130, 140) << QPoint(180, 170)
		<< QPoint(180, 140) << QPoint(220, 110)
		<< QPoint(140, 100);
	painter.drawPolygon(polygon);
*/
	//draw an ellipse
	//The setRenderHint() call enables antialiasing, telling QPainter to use different
	//color intensities on the edges to reduce the visual distortion that normally
	//occurs when the edges of a shape are converted into pixels
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
	painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
//	painter.drawRect(1, 3, m_percentRectWidth, 12);
	painter.drawRect(1, 2, m_percentRectWidth, m_rowHeight-3);

	painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
//	painter.drawRect(((int)m_percent)+1, 3, m_percentRectWidth-((int)m_percent), 12);
	painter.drawRect(((int)m_percent)+1, 3, m_percentRectWidth-((int)m_percent), m_rowHeight-3);

	// draw percent string on right end of bar
	QString s = QString("%1%").arg(m_percent);
	painter.drawText(104, 0, 44, 13, Qt::AlignRight, s);
}
