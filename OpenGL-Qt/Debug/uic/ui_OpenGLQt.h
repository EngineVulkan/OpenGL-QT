/********************************************************************************
** Form generated from reading UI file 'OpenGLQt.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENGLQT_H
#define UI_OPENGLQT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OpenGLQtClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *OpenGLQtClass)
    {
        if (OpenGLQtClass->objectName().isEmpty())
            OpenGLQtClass->setObjectName(QString::fromUtf8("OpenGLQtClass"));
        OpenGLQtClass->resize(600, 400);
        menuBar = new QMenuBar(OpenGLQtClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        OpenGLQtClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(OpenGLQtClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        OpenGLQtClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(OpenGLQtClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        OpenGLQtClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(OpenGLQtClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        OpenGLQtClass->setStatusBar(statusBar);

        retranslateUi(OpenGLQtClass);

        QMetaObject::connectSlotsByName(OpenGLQtClass);
    } // setupUi

    void retranslateUi(QMainWindow *OpenGLQtClass)
    {
        OpenGLQtClass->setWindowTitle(QApplication::translate("OpenGLQtClass", "OpenGLQt", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OpenGLQtClass: public Ui_OpenGLQtClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENGLQT_H
