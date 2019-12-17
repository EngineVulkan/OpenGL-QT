/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <Qt3DCore/qentity.h>

//#define PROGRAM_VERTEX_ATTRIBUTE 0
//#define PROGRAM_TEXCOORD_ATTRIBUTE 1

GLWidget::GLWidget(QWidget *parent)
	: QOpenGLWidget(parent),
	clearColor(Qt::black),
	xRot(0),
	yRot(0),
	zRot(0),
	program(0)
{
	memset(textures, 0, sizeof(textures));
	//model matrix
	model.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
	model.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
	model.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);
	model.translate(0.0f, 0.0f, -20.0f);

	//view matrix
	view.lookAt(QVector3D(0, 0, 0), QVector3D(0, 0, -10), QVector3D(0, 1, 0));

	//prject matrix
	project.frustum(-0.5f, +0.5f, +0.5f, -0.5f, 4.0f, 200.0f);

	xMov = 0;
	yMov = 0;
	zMov = 0;

}

GLWidget::~GLWidget()
{
	makeCurrent();
	vbo.destroy();
	for (int i = 0; i < 6; ++i)
		delete textures[i];
	delete program;
	doneCurrent();
	Qt3DCore::QEntity *test = new Qt3DCore::QEntity();
}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(800, 800);
}

QSize GLWidget::sizeHint() const
{
	return QSize(800, 800);
}

void GLWidget::rotateBy(int xAngle, int yAngle, int zAngle)
{
	xRot += xAngle;
	yRot += yAngle;
	zRot += zAngle;
	update();
}

void GLWidget::moveBy(int x,int y,int z)
{
	xMov += x;
	yMov += y;
	zMov += z;
	update();
}

void GLWidget::setClearColor(const QColor &color)
{
	clearColor = color;
	update();
}

void GLWidget::initializeGL()
{
	initializeOpenGLFunctions();
}

void GLWidget::paintGL()
{
	float div = 64;
	view.rotate(xRot / 64.0f, 1.0f, 0.0f, 0.0f);
	view.rotate(yRot / 64.0f, 0.0f, 1.0f, 0.0f);
	view.rotate(zRot / 64.0f, 0.0f, 0.0f, 1.0f);
	genShadowMap();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapID, 0);
	initDepthMaterail();

	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	initFrontMaterail();
	initPlane();

}

void GLWidget::resizeGL(int width, int height)
{
	int side = qMin(width, height);
	//glViewport((width - side) / 2, (height - side) / 2, side, side);
	glViewport(0, 0, width, height);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	if (event->buttons() & Qt::LeftButton) {
		rotateBy(8 * dy, 8 * dx, 0);
		//moveBy(dy,dx,0);
	}
	else if (event->buttons() & Qt::RightButton) {
		//rotateBy(8 * dy, 0, 8 * dx);
		moveBy(dy,0,dx);
	}
	lastPos = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */)
{
	emit clicked();
}

void GLWidget::genShadowMap()
{
	glGenFramebuffers(1, &depthMapFBO);
	int width = 200;
	int height = 200;
	glGenTextures(1, &shadowMapID);
	glBindTexture(GL_TEXTURE_2D, shadowMapID);
	QOpenGLTexture text (QOpenGLTexture::Target::Target2D);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void GLWidget::initDepthMaterail()
{
	makeObject();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
#define PROGRAM_NORMAL_ATTRIBUTE 2

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	bool result = vshader->compileSourceFile(QString(":/OpenGLQt/Resources/Shader/shadowMap.vert"));
	if (!result)
	{
		int i = 0;
	}

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	bool fResult = fshader->compileSourceFile(QString(":/OpenGLQt/Resources/Shader/shadowMap.frag"));
	if (!fResult)
	{
		int i = 0;
	}

	program = new QOpenGLShaderProgram;
	program->addShader(vshader);
	program->addShader(fshader);
	program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	program->bindAttributeLocation("normal",PROGRAM_NORMAL_ATTRIBUTE);
	program->link();

	program->bind();
	program->setUniformValue("texture", 0);

	glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	program->setUniformValue("matrix", project*model*view);
	program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
	program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
	program->enableAttributeArray(PROGRAM_NORMAL_ATTRIBUTE);
	program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
	program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
	program->setAttributeBuffer(PROGRAM_NORMAL_ATTRIBUTE, GL_FLOAT, 5 * sizeof(GLfloat), 3, 8 * sizeof(GLfloat));

	for (int i = 0; i < 6; ++i) {
		glBindTexture(GL_TEXTURE_2D, shadowMapID);

		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
	}
}

void GLWidget::initFrontMaterail()
{
	initializeOpenGLFunctions();

	makeObject();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	bool result = vshader->compileSourceFile(QString(":/OpenGLQt/Resources/Shader/simpleShader.vert"));
	if (!result)
	{
		int i = 0;
	}

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	bool fResult=fshader->compileSourceFile(QString(":/OpenGLQt/Resources/Shader/simpleShader.frag"));
	if (!fResult)
	{
		int i = 0;
	}

	program = new QOpenGLShaderProgram;
	program->addShader(vshader);
	program->addShader(fshader);
	program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	program->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
	program->link();

	program->bind();
	program->setUniformValue("texture", 0);

	glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program->setUniformValue("matrix", project*model*view);
	program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
	program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
	program->enableAttributeArray(PROGRAM_NORMAL_ATTRIBUTE);
	program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
	program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
	program->setAttributeBuffer(PROGRAM_NORMAL_ATTRIBUTE, GL_FLOAT, 5 * sizeof(GLfloat), 3, 8 * sizeof(GLfloat));
	for (int i = 0; i < 6; ++i) {
		/*if (i == 0)
		{
			glBindTexture(GL_TEXTURE_2D, shadowMapID);
		}
		else
		{
			textures[i]->bind();
		}*/

		glBindTexture(GL_TEXTURE_2D, shadowMapID);

		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
	}
}


QVector3D GLWidget::calcNormal(QVector3D v1, QVector3D v2, QVector3D v3)
{
	QVector3D direction1 = v2 - v1;
	QVector3D direction2 = v1 - v3;
	QVector3D result = QVector3D::crossProduct(direction1, direction2);
	result = result.normalized();
	return result;
}

void GLWidget::initPlane()
{
	static const int coords[4][3] = { { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } };
	QVector<GLfloat> vertData;
	QVector<GLuint> indexData;
	QVector3D position1(coords[0][0], coords[0][1], coords[0][2]);
	QVector3D position2(coords[1][0], coords[1][1], coords[1][2]);
	QVector3D position3(coords[2][0], coords[2][1], coords[2][2]);
	QVector3D normal = calcNormal(position1,position2,position3);
	for (int i = 0; i < 4; i++)
	{
		//verctex
		vertData.append(0.5*coords[i][0]);
		vertData.append(0.5*coords[i][1]);
		vertData.append(0.5*coords[i][2]);
	    
		//uv
		vertData.append(i == 0 || i == 3);
		vertData.append(i == 0 || i == 1);

		//normal

		vertData.append(normal.x());
		vertData.append(normal.y());
		vertData.append(normal.z());
	}

	indexData.append(0);
	indexData.append(3);
	indexData.append(2);

	indexData.append(0);
	indexData.append(2);
	indexData.append(1);

	vbo.create();
	vbo.bind();
	vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));

	GLuint ebo;
	glGenBuffers(1,&ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertData.count() * sizeof(GLfloat),indexData.constData(),GL_STATIC_DRAW);

		
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
#define PROGRAM_NORMAL_ATTRIBUTE 2

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	bool result = vshader->compileSourceFile(QString(":/OpenGLQt/Resources/Shader/planeShader.vert"));
	if (!result)
	{
		int i = 0;
	}

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	bool fResult = fshader->compileSourceFile(QString(":/OpenGLQt/Resources/Shader/planeShader.frag"));
	if (!fResult)
	{
		int i = 0;
	}


	program = new QOpenGLShaderProgram;
	program->addShader(vshader);
	program->addShader(fshader);
	program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	program->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
	program->link();

	program->bind();
	program->setUniformValue("texture", 0);

	//glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program->setUniformValue("matrix", project*model*view);
	program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
	program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
	program->enableAttributeArray(PROGRAM_NORMAL_ATTRIBUTE);
	program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
	program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
	program->setAttributeBuffer(PROGRAM_NORMAL_ATTRIBUTE, GL_FLOAT, 5 * sizeof(GLfloat), 3, 8 * sizeof(GLfloat));

	glBindTexture(GL_TEXTURE_2D, shadowMapID);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indexData.constData());
}
void GLWidget::makeObject()
{
	static const int coords[6][4][3] = {
		{ { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
		{ { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
		{ { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
		{ { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
		{ { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
		{ { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
	};

	for (int j = 0; j < 6; ++j)
		textures[j] = new QOpenGLTexture(QImage(QString(":/OpenGLQt/Resources/images/side%1.png").arg(j + 1)).mirrored());
	QVector<GLfloat> vertData;
	for (int i = 0; i < 6; ++i) {
		QVector3D position1(coords[i][0][0], coords[i][0][1], coords[i][0][2]);
		QVector3D position2(coords[i][1][0], coords[i][1][1], coords[i][1][2]);
		QVector3D position3(coords[i][2][0], coords[i][2][1], coords[i][2][2]);
		QVector3D normal = calcNormal(position1, position2, position3);
		for (int j = 0; j < 4; ++j) {
			// vertex position
			vertData.append(0.2 * coords[i][j][0]);
			vertData.append(0.2 * coords[i][j][1]);
			vertData.append(0.2 * coords[i][j][2]);
			// texture coordinate
			vertData.append(j == 0 || j == 3);
			vertData.append(j == 0 || j == 1);
			//normal 
			GLfloat x = normal.x();
			GLfloat y = normal.y();
			GLfloat z = normal.z();
			vertData.append(x);
			vertData.append(y);
			vertData.append(z);
		}
	}

	vbo.create();
	vbo.bind();
	vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}



