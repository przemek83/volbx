#include "Application.h"

#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include <QWidget>

#include "CommonProperties.h"

namespace
{
QString getStyleFileExtension() { return QStringLiteral(".css"); }

QString getResStylePath(const QString& styleName)
{
    const static QLatin1String resPrefix(":/Css/");
    return resPrefix + styleName + getStyleFileExtension();
}

QString getLocalStyleFilePath(const QString& styleName)
{
    QString stylePath;
    stylePath.append(QApplication::applicationDirPath());
    stylePath.append(QStringLiteral("/"));
    stylePath.append(styleName);
    stylePath.append(getStyleFileExtension());
    return stylePath;
}

QString getStylePath(const QString& styleName)
{
    if (QFile::exists(getResStylePath(styleName)))
        return getResStylePath(styleName);

    return getLocalStyleFilePath(styleName);
}

void clearAppFocus()
{
    QWidget* focusWidget = qApp->focusWidget();
    if (focusWidget != nullptr)
        focusWidget->clearFocus();
}
}  // namespace

namespace Application
{
void setAdditionalApplicatioInfo(const char* productName)
{
    QApplication::setApplicationName(QLatin1String(productName));
    QApplication::setApplicationVersion(QStringLiteral(VER_PRODUCTVERSION_STR));
    QApplication::setOrganizationName(QStringLiteral(VER_COMPANYNAME_STR));
    QApplication::setOrganizationDomain(QLatin1String(VER_COMPANYDOMAIN_STR));
}

void setCssStyle(const QString& styleName)
{
    clearAppFocus();

    QFile styleFile(getStylePath(styleName));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const QString style(QLatin1String(styleFile.readAll()));
        qApp->setStyleSheet(style);
    }
}

void setQtStyle(const QString& name)
{
    clearAppFocus();
    qApp->setStyleSheet(QString());
    qApp->setStyle(name);
}

void initStyle(const QString& nameFromConfig)
{
    const QStringList qtStyles{QStyleFactory::keys()};
    if (qtStyles.contains(nameFromConfig))
        setQtStyle(nameFromConfig);
    else
        setCssStyle(nameFromConfig);
}
}  // namespace Application
