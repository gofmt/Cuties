/*
 *  Copyright 2013 Kreogist Dev Team
 *
 *  This file is part of Kreogist-Cuties.
 *
 *    Kreogist-Cuties is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *    Kreogist-Cuties is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kreogist-Cuties.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kcdragproxy.h"

#include "kcmainwindow.h"

KCMainWindow::KCMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    titleBar=new KCTitleBar(this);
    setMenuWidget(titleBar);
    setAcceptDrops(true);

#ifdef Q_OS_MACX
    QMainWindow::setUnifiedTitleAndToolBarOnMac(false);
#else
    setContentsMargins(2,2,2,2);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    dragProxy=new KCDragProxy(this);
    dragProxy->setBorderWidth(8,8,8,8);
    connect(titleBar, &KCTitleBar::dragProxyEnabled,
            dragProxy,&KCDragProxy::setEnabled);
#endif
}

KCTitleBar *KCMainWindow::getTitleBar() const
{
    return titleBar;
}

void KCMainWindow::setTitleBar(KCTitleBar *newTitleBar)
{
    titleBar = newTitleBar;
}

#ifndef Q_OS_MACX
void KCMainWindow::setMainButtonIcon(const QString &mainIcon)
{
    if(titleBar)
    {
        titleBar->setMainButtonIcon(mainIcon);
    }
}

void KCMainWindow::setMenu(QMenu *menu)
{
    if(titleBar)
    {
        titleBar->setMenu(menu);
    }
}
#endif
