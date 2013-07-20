/*
 *  Copyright 2013 Kreogist Dev Team
 *
 *      Wang Luming <wlm199558@126.com>
 *      Miyanaga Saki <tomguts@126.com>
 *      Zhang Jiayi <bf109g2@126.com>
 *
 *  This file is part of Kreogist-Cute-IDE.
 *
 *    Kreogist-Cute-IDE is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *    Kreogist-Cute-IDE is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kreogist-Cute-IDE.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kcilinenumpanel.h"

static const int normal_color=120;
static const int curr_line_color=255;

kciLinenumPanel::kciLinenumPanel(QWidget *parent) :
    kciPanel(parent)
{
    textColor.setRgb(normal_color,normal_color,normal_color);
}

void kciLinenumPanel::draw(QPainter *painter, QTextBlock *block,
                           int x, int y, int w, int h,
                           bool isCurrentLine)
{
    if(isCurrentLine)
    {
        QFont font=painter->font();
        font.setBold(true);
        painter->setFont(font);

        textColor.setRgb(curr_line_color,curr_line_color,curr_line_color);
    }

    QPen pen(painter->pen ());
    pen.setColor(textColor);
    painter->setPen(pen);

    painter->drawText(x - 3, y, w, h,
            Qt::AlignRight |  Qt::AlignTop,
            QString::number(block->blockNumber()+1));

    if(isCurrentLine)
    {
        QFont font=painter->font();
        font.setBold(false);
        painter->setFont(font);

        textColor.setRgb(normal_color,normal_color,normal_color);
    }

}