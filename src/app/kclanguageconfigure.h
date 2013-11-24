#ifndef KCLANGUAGECONFIGURE_H
#define KCLANGUAGECONFIGURE_H

#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QApplication>
#include <QPixmap>
#include <QStringList>
#include <QDebug>
#include <QTranslator>
#include <QApplication>
#include <QMetaEnum>

#include "kcconfigure.h"

class KCLanguageConfigure : public KCConfigure
{
    Q_OBJECT
public:
    void readConfigure();
    void writeConfigure();
    static KCLanguageConfigure *getInstance();

    bool setLanguage(const QString &newLanguageName);
    bool setLanguageIndex(int newLangaugeIndex);
    QStringList getLanguageList() const;
    QStringList getLanguageNameList() const;
    QList<QPixmap> getLanguageFileIcon() const;
    int getCurrentLanguageIndex() const;
    QString getCurrentLanguageName() const;
    QPixmap getCurrentLanguageIcon() const;
    QString getLanguageString(QLocale::Language language);
    QString getLocaleFileDir() const;

signals:
    void newLanguageSet();

private:
    bool applyLangaugeSet(int languageIndex);

    //Language file folder & language translation file
    QString localeFileDir, localeTranslation;

    //Cache list
    QStringList languageFileList, languageName;
    QList<QPixmap> languageFileIcon;

    QString defaultLanguageName;
    QString currentLanguageName;
    int currentLanguageIndex;

    KCLanguageConfigure();
    static KCLanguageConfigure *instance;

    void loadLanguageList();

    QTranslator appTrans;
};

#endif // KCLANGUAGECONFIGURE_H
