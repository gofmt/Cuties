#include <QDebug>
#include <QMouseEvent>
#include <QPainter>

#include "kctextblockdata.h"

#include "kcunibodypanel.h"

KCUnibodyPanel::KCUnibodyPanel(QWidget *parent) :
    KCTextPanel(parent)
{
    setContentsMargins(0,0,0,0);
    foldMark.load(":/SmartPanel/image/folder.png");
    foldEndMark.load(":/SmartPanel/image/folderEnd.png");
    foldedMark.load(":/SmartPanel/image/folded.png");
    compileErrorMark.load(":/SmartPanel/image/compile_error.png");
    foldMarkWidth=foldMark.width();
    foldMarkHeight=foldMark.height();
    errorMarkWidth=compileErrorMark.width();
    errorMarkHeight=compileErrorMark.height();

    setFixedWidth(errorMarkWidth+4);
}

void KCUnibodyPanel::drawContent(int x,
                                 int y,
                                 int width,
                                 int height,
                                 QTextBlock block,
                                 QTextCursor cursor)
{
    Q_UNUSED(x);
    Q_UNUSED(cursor);
    KCTextBlockData *data=static_cast<KCTextBlockData *>(block.userData());
    if(data!=NULL)
    {
        QPainter painter(this);
        codeLevelUnit codeLevelInfo=data->getCodeLevelInfo();
        QPoint codeLevelBegin(x, y);
        codeLevelInfo.codeLevelRect.setTopLeft(codeLevelBegin);
        codeLevelInfo.codeLevelRect.setWidth(width);
        codeLevelInfo.codeLevelRect.setHeight(height);
        data->setCodeLevelInfo(codeLevelInfo);

        if(codeLevelInfo.hasError)
        {
            painter.drawPixmap((width-errorMarkWidth)/2,
                                y+(height-errorMarkHeight)/2,
                                errorMarkWidth,
                                errorMarkHeight,
                                compileErrorMark);
        }

        if(codeLevelInfo.codeLevelUp)
        {
            if(codeLevelInfo.hasFolded)
            {
                painter.drawPixmap((width-foldMarkWidth)/2,
                                   y+(height-foldMarkHeight)/2,
                                   foldMarkWidth,
                                   foldMarkHeight,
                                   foldedMark);
            }
            else
            {
                painter.drawPixmap((width-foldMarkWidth)/2,
                                   y+(height-foldMarkHeight)/2,
                                   foldMarkWidth,
                                   foldMarkHeight,
                                   foldMark);
            }
        }
    }
}

void KCUnibodyPanel::setPanelWidth(int lineNumberPanelWidth)
{
    Q_UNUSED(lineNumberPanelWidth);
    return;
}

void KCUnibodyPanel::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        isPressed=true;
        pressedPos=event->pos();
    }
    QWidget::mousePressEvent(event);
}

void KCUnibodyPanel::mouseReleaseEvent(QMouseEvent *event)
{
    if(isPressed)
    {
        QTextBlock block=getFirstBlock();
        int lastBlockNumber=getLastBlock().blockNumber();

        for(; block.blockNumber() <= lastBlockNumber && block.isValid(); block=block.next())
        {
            KCTextBlockData *data=static_cast<KCTextBlockData *>(block.userData());
            if(block.isVisible() && data!=NULL)
            {
                codeLevelUnit codeLevelInfo=data->getCodeLevelInfo();
                if(codeLevelInfo.codeLevelRect.contains(pressedPos) &&
                   codeLevelInfo.codeLevelRect.contains(event->pos()))
                {
                    if(codeLevelInfo.codeLevelUp)
                    {
                        if(codeLevelInfo.hasFolded)
                        {
                            //Unfold
                            emit requireUnfoldStartAt(block.blockNumber());
                            codeLevelInfo.hasFolded=false;
                        }
                        else
                        {
                            //Fold
                            emit requireFoldStartAt(block.blockNumber());
                            codeLevelInfo.hasFolded=true;
                        }
                        data->setCodeLevelInfo(codeLevelInfo);
                    }
                    break;
                }
            }
        }
        isPressed=false;
    }
    QWidget::mouseReleaseEvent(event);
}