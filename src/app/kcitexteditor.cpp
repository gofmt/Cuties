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

#include "kcitexteditor.h"

kciTextEditor::kciTextEditor(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setContentsMargins(0,0,0,0);

    setFrameStyle(QFrame::NoFrame);
    setFont(QString("Monaco"));
    setAcceptDrops(false);

    QPalette pal = palette();
    pal.setColor(QPalette::Base,QColor(0x38,0x38,0x38));
    pal.setColor(QPalette::Text,QColor(255,255,255));
    pal.setColor(QPalette::Button,QColor(83,83,83));
    setPalette(pal);
    setFrameStyle(0);

    lineColor = QColor(0x64,0x64,0x64);
    searchResultColor = QColor(0xf7,0xcf,0x3d);

    //Solve the default line's bug.
    updateHighlights();

    connect(this,&kciTextEditor::cursorPositionChanged,
            this,&kciTextEditor::updateHighlights);
    connect(verticalScrollBar(),SIGNAL(valueChanged(int)),
            this,SLOT(updateHighlights()));
}

void kciTextEditor::paintEvent(QPaintEvent *e)
{
    QPlainTextEdit::paintEvent(e);
    emit updated();
}

void kciTextEditor::showSearchResultAt(int num)
{
    searchResult result=resultList[num];
    setDocumentCursor(result.lineNum,result.startPos);
}

void kciTextEditor::setDocumentCursor(int nLine, int linePos)
{
     QTextCursor cursor = textCursor();
     cursor.setPosition(document()->findBlockByNumber(nLine).position());
     cursor.movePosition(QTextCursor::NextCharacter,
                          QTextCursor::MoveAnchor,
                          linePos);
     setTextCursor(cursor);
}

void kciTextEditor::updateHighlights()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    highlightCurrentLine(extraSelections);
    highlightSearchResult(extraSelections);

    setExtraSelections(extraSelections);
}

void kciTextEditor::highlightCurrentLine(QList<QTextEdit::ExtraSelection>& selections)
{
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        selections.append(selection);
    }
}

void kciTextEditor::highlightSearchResult(QList<QTextEdit::ExtraSelection>& selections)
{
    QTextCursor _cursor(document());

    int lastLineNum=0;

    QTextBlock block=firstVisibleBlock();
    int firstBlockNumber=block.blockNumber();
    int bottom=height()/fontMetrics().lineSpacing();
    int lastBlockNumber=firstBlockNumber;

    for(;block.isValid() && bottom>0;block=block.next(),lastBlockNumber++)
    {
        bottom-=block.lineCount();
    }

    auto i=resultList.begin();
    auto l=resultList.end();
    while(i<l && i->lineNum < firstBlockNumber)
    {
        i++;
    }

    for(;i<l && i->lineNum <= lastBlockNumber;i++)
    {
        QTextEdit::ExtraSelection selection;

        _cursor.clearSelection();
        _cursor.movePosition(QTextCursor::NextBlock,
                             QTextCursor::MoveAnchor,
                             i->lineNum-lastLineNum);
        _cursor.movePosition(QTextCursor::StartOfBlock,
                             QTextCursor::MoveAnchor);
        _cursor.movePosition(QTextCursor::NextCharacter,
                             QTextCursor::MoveAnchor,
                             i->startPos);
        _cursor.movePosition(QTextCursor::NextCharacter,
                             QTextCursor::KeepAnchor,
                             i->length);
        selection.cursor=_cursor;

        selection.format.setBackground(searchResultColor);
        selections.append(selection);

        lastLineNum=i->lineNum;
    }
}

void kciTextEditor::setSearchResults(QList<searchResult> *results)
{
    resultList=*results;

    updateHighlights();
}