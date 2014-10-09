/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: percentrect.h.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/


#ifndef PERCENTRECT_H
#define PERCENTRECT_H

#include <QWidget>

namespace Ui {
class PercentRect;
}

class PercentRect : public QWidget
{
    Q_OBJECT

public:
    explicit PercentRect(float percent, int rowHeight, int percentRectWidth, QWidget *parent = 0);
//  PercentRect(float percent, int rowHeight, int percentRectWidth = 100);
    ~PercentRect();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::PercentRect *ui;
    int m_percentRectWidth;
    int m_rowHeight;
    float m_percent;
};

#endif // PERCENTRECT_H
