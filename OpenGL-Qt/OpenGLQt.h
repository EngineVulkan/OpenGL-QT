#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_OpenGLQt.h"

class OpenGLQt : public QMainWindow
{
	Q_OBJECT

public:
	OpenGLQt(QWidget *parent = Q_NULLPTR);

private:
	Ui::OpenGLQtClass ui;
};
