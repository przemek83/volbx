#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QStyleFactory>

#include "CommonProperties.h"

#include "Application.h"

const char* Application::cssSuffix_ = ".css";

void Application::setAdditionalApplicatioInfo(const char* productName)
{
    QApplication::setApplicationName(QLatin1String(productName));
    QApplication::setApplicationVersion(QLatin1String(VER_PRODUCTVERSION_STR));
    QApplication::setOrganizationName(QLatin1String(VER_COMPANYNAME_STR));
    QApplication::setOrganizationDomain(QLatin1String(VER_COMPANYDOMAIN_STR));
}

void Application::initStyle(QString nameFromConfig)
{
    QStringList qtStyles = QStyleFactory::keys();
    if (qtStyles.contains(nameFromConfig))
    {
        setQtStyle(nameFromConfig);
    }
    else
    {
        setCssStyle(nameFromConfig);
    }
}

QString Application::getStylePath(QString styleName)
{
    if (QFile::exists(getResStylePath(styleName)))
    {
        return getResStylePath(styleName);
    }

    return getLocalStyleFilePath(styleName);
}

QString Application::getResStylePath(QString styleName)
{
    const static QLatin1String resPrefix(":/Css/");
    return resPrefix + styleName + QLatin1String(cssSuffix_);
}

QString Application::getLocalStyleFilePath(QString styleName)
{
    QString stylePath;
    stylePath.append(QApplication::applicationDirPath());
    stylePath.append(QLatin1String("/"));
    stylePath.append(styleName);
    stylePath.append(QLatin1String(cssSuffix_));
    return stylePath;
}

void Application::setCssStyle(QString styleName)
{
    clearAppFocus();

    QFile styleFile(getStylePath(styleName));

    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString style(QLatin1String(styleFile.readAll()));

        qApp->setStyleSheet(style);
    }
}

void Application::setQtStyle(QString name)
{
    clearAppFocus();
    qApp->setStyleSheet(QString());
    qApp->setStyle(name);
}

void Application::clearAppFocus()
{
    QWidget* focusWidget = qApp->focusWidget();
    if (nullptr != focusWidget)
    {
        focusWidget->clearFocus();
    }
}

QString Application::getGroupBoxStyle()
{
    QString styleSheet(QLatin1String("QGroupBox::indicator:unchecked { image: "));
    styleSheet.append(QLatin1String("url(:/Stylesheet/Images/uncheckedGroupBox.png);}"));
    styleSheet.append(QLatin1String("QGroupBox::indicator:checked {image: "));
    styleSheet.append(QLatin1String("url(:/Stylesheet/Images/checkedGroupBox.png);}"));
    styleSheet.append(QLatin1String("QGroupBox::title { subcontrol-origin: margin;  "));
    styleSheet.append(QLatin1String("subcontrol-position: top left;}"));
    return styleSheet;
}

