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

static const int SearchBarOffset = 20;

kciTextEditor::kciTextEditor(QWidget *parent) :
    QWidget(parent)
{
    //setWindowFlags(Qt::AnchorPoint);
    mainLayout=new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    setContentsMargins(0,0,0,0);
    mainLayout->setMargin(0);
    setLayout(mainLayout);

    strFileFilter = QObject::tr("All Support Files")+
            "(*.txt *.h *.hpp *.rh *.hh *.c *.cpp *.cc *.cxx *.c++ *.cp *.pas);;"+
            QObject::tr("Plain Text Files")+"(*.txt);;"+
            QObject::tr("Hearder Files")+"(*.h *.hpp *.rh *.hh);;"+
            QObject::tr("C Source Files")+"(*.c);;"+
            QObject::tr("C++ Source Files")+"(*.cpp *.cc *.cxx *.c++ *.cp);;"+
            QObject::tr("Pascal Source Files")+"(*.pas);;"+
            QObject::tr("All Files")+"(*.*)";

    linePanel=new kciLinenumPanel(this);
    mainLayout->addWidget(linePanel);

    markPanel=new kciMarkPanel(this);
    mainLayout->addWidget(markPanel);

    editor=new kciCodeEditor(this);
    linePanel->setKciCodeEditor(editor);
    markPanel->setKciCodeEditor(editor);
    document=editor->document();
    mainLayout->addWidget(editor);
    mainLayout->addSpacing(1);

    connect(editor->document(),SIGNAL(modificationChanged(bool)),
            this,SLOT(onModificationChanged(bool)));
    connect(editor,SIGNAL(cursorPositionChanged()),
            this,SLOT(cursorChanged()));

    m_langMode=new kciLanguageMode(this);

    QPalette pal = palette();
    pal.setColor(QPalette::Base,QColor(0x53,0x53,0x53));
    pal.setColor(QPalette::Text,QColor(255,255,255));
    setPalette(pal);

    filePath.clear();
    fileError=QFileDevice::NoError;

    searchBar=new kciSearchWindow(editor);
    searchBar->hide();
    connect(searchBar,SIGNAL(hideButtonPressed()),editor,SLOT(setFocus()));
}

void kciTextEditor::showSearchBar()
{
    if(!searchBar->isVisible())
    {
        QPropertyAnimation *searchAnime=new QPropertyAnimation(searchBar,"geometry");
        QRect animeEndPos=searchBar->rect();
        animeEndPos.setX(editor->width()-searchBar->width()-SearchBarOffset);
        QRect animeStartPos=animeEndPos;
        animeStartPos.setTop(-animeStartPos.height());
        searchAnime->setStartValue(animeStartPos);
        searchAnime->setDuration(300);
        searchAnime->setEndValue(animeEndPos);
        searchAnime->setEasingCurve(QEasingCurve::OutCubic);
        searchBar->show();
        searchAnime->start();
        searchBar->setTextFocus();
    }
}

bool kciTextEditor::open(const QString &fileName)
{
    QFile _file(fileName);

    if(_file.open(QIODevice::ReadOnly |QIODevice::Text))
    {
        QTextStream _textIn(&_file);

        editor->clear();
        editor->insertPlainText(QString(_textIn.readAll()));

        fileInfoChanged(_file);

        return true;
    }
    else
    {
        fileError=_file.error();
        return false;
    }
}

QFileDevice::FileError kciTextEditor::error()
{
    return fileError;
}

bool kciTextEditor::save()
{
    if(!filePath.isEmpty())
    {
        return saveAs(filePath);
    }
    else
    {
        if(!dosaveas(tr("save")))
        {
            if(fileError!=QFileDevice::AbortError)
            {
                QErrorMessage error(this);
                error.showMessage(tr("Saving file failed!"));
                error.exec();
            }
            return false;
        }
        else
        {
            return true;
        }
    }
}

bool kciTextEditor::saveAs()
{
    if(!dosaveas(tr("save as")))
    {
        if(fileError!=QFileDevice::AbortError)
        {
            QErrorMessage error(this);
            error.showMessage(tr("Saving file failed!"));
            error.exec();
        }
        return false;
    }
    else
    {
        return true;
    }
}

bool kciTextEditor::dosaveas(const QString &Caption)
{
    QSettings settings(kciGlobal::settingsFileName,QSettings::IniFormat);
    filePath=QFileDialog::getSaveFileName(this,Caption,settings.value("texteditor/historyDir").toString(),strFileFilter);

    if(!filePath.isEmpty())
    {
        return saveAs(filePath);
    }
    else
    {
        fileError=QFileDevice::AbortError;
        return false;
    }
}

bool kciTextEditor::saveAs(const QString &fileName)
{

    QFile _file(fileName);

    if(_file.open(QIODevice::WriteOnly |QIODevice::Text))
    {
        QTextStream _textOut(&_file);
        _textOut<<editor->toPlainText()<<flush;
        fileInfoChanged(_file);
        return true;
    }
    else
    {
        fileError=_file.error();
        return false;
    }
}

void kciTextEditor::closeEvent(QCloseEvent *e)
{
    if(editor->document()->isModified())
    {
        QMessageBox msgbox(this);
        QString strDisplayFileName;

        if(filePath.isEmpty())
        {
            strDisplayFileName=editor->documentTitle();
        }
        else
        {
            strDisplayFileName=filePath;
        }

        msgbox.setText(tr("Will you save changes to the the following file?") + "\n" +
                       strDisplayFileName);
        msgbox.setInformativeText(tr("If you don't save the changes, all the changes will be lost."));

        msgbox.setStandardButtons(QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
        msgbox.setDefaultButton(QMessageBox::Save);
        msgbox.setButtonText(QMessageBox::Save,tr("&Save"));
        msgbox.setButtonText(QMessageBox::Discard,tr("&Don't Save"));
        msgbox.setButtonText(QMessageBox::Cancel,tr("&Cancel"));

        int ret=msgbox.exec();

        switch (ret)
        {
        case QMessageBox::Save:
            // Save was clicked
            if(!save())
            {
                e->ignore();
                break;
            }
            else
            {
                e->accept();
                break;
            }
        case QMessageBox::Discard:
            // Don't Save was clicked
            e->accept();

            break;
        case QMessageBox::Cancel:
            // Cancel was clicked
            e->ignore();

            break;
        default:
            // should never be reached
            qWarning("codeeditor.cpp: switch(ret) reached an unexcepted line!");
            break;
        }
    }
    else
    {
        e->accept();
    }

    return ;
}

void kciTextEditor::setDocumentTitle(const QString& title)
{
    editor->setDocumentTitle(title);
    emit filenameChanged(title);
}

QString kciTextEditor::getDocumentTitle()
{
    return editor->documentTitle();
}

void kciTextEditor::redo()
{
    editor->redo();
}

void kciTextEditor::undo()
{
    editor->undo();
}

void kciTextEditor::copy()
{
    editor->copy();
}

void kciTextEditor::cut()
{
    editor->cut();
}

void kciTextEditor::paste()
{
    editor->paste();
}

void kciTextEditor::selectAll()
{
    editor->selectAll();
}

void kciTextEditor::setTextFocus()
{
    editor->setFocus();
}

void kciTextEditor::onModificationChanged(bool changed)
{
    if(changed)
    {
        emit filenameChanged(editor->documentTitle()+"*");
    }
    else
    {
        emit filenameChanged(editor->documentTitle());
    }
}

QString kciTextEditor::getFilePath()
{
    return filePath;
}

QString kciTextEditor::getSelectedText()
{
    return editor->textCursor().selectedText();
}

void kciTextEditor::cursorChanged()
{
    fileTextCursor=editor->textCursor();
    emit fileTextCursorChanged();
}

QTextCursor kciTextEditor::getTextCursor()
{
    return fileTextCursor;
}

int kciTextEditor::getTextLines()
{
    return editor->document()->blockCount();
}

void kciTextEditor::setDocumentCursor(int nLine, int linePos)
{
     editor->setDocumentCursor(nLine,linePos);
}

void kciTextEditor::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    searchBar->setGeometry(editor->width()-searchBar->width()-SearchBarOffset,
                           0,
                           searchBar->width(),
                           searchBar->height());
}

void kciTextEditor::fileInfoChanged(const QFile &file)
{
    QFileInfo _fileInfo(file);
    editor->setDocumentTitle(_fileInfo.fileName());
    emit filenameChanged(_fileInfo.fileName());

    m_langMode->setFileSuffix(_fileInfo.suffix());

    filePath=file.fileName();
    fileError=QFileDevice::NoError;
    editor->document()->setModified(false);

    QSettings settings(kciGlobal::settingsFileName,QSettings::IniFormat);
    settings.setValue("texteditor/historyDir",_fileInfo.absolutePath());
}

kciLanguageMode *kciTextEditor::langMode() const
{
    return m_langMode;
}

bool kciTextEditor::isModified()
{
    return editor->document()->isModified();
}