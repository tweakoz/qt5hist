/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qandroidmediarecordercontrol.h"

#include "qandroidcapturesession.h"

QT_BEGIN_NAMESPACE

QAndroidMediaRecorderControl::QAndroidMediaRecorderControl(QAndroidCaptureSession *session)
    : QMediaRecorderControl()
    , m_session(session)
{
    connect(m_session, SIGNAL(stateChanged(QMediaRecorder::State)), this, SIGNAL(stateChanged(QMediaRecorder::State)));
    connect(m_session, SIGNAL(statusChanged(QMediaRecorder::Status)), this, SIGNAL(statusChanged(QMediaRecorder::Status)));
    connect(m_session, SIGNAL(durationChanged(qint64)), this, SIGNAL(durationChanged(qint64)));
    connect(m_session, SIGNAL(actualLocationChanged(QUrl)), this, SIGNAL(actualLocationChanged(QUrl)));
    connect(m_session, SIGNAL(error(int,QString)), this, SIGNAL(error(int,QString)));
}

QUrl QAndroidMediaRecorderControl::outputLocation() const
{
    return m_session->outputLocation();
}

bool QAndroidMediaRecorderControl::setOutputLocation(const QUrl &location)
{
    return m_session->setOutputLocation(location);
}

QMediaRecorder::State QAndroidMediaRecorderControl::state() const
{
    return m_session->state();
}

QMediaRecorder::Status QAndroidMediaRecorderControl::status() const
{
    return m_session->status();
}

qint64 QAndroidMediaRecorderControl::duration() const
{
    return m_session->duration();
}

bool QAndroidMediaRecorderControl::isMuted() const
{
    // No API for this in Android
    return false;
}

qreal QAndroidMediaRecorderControl::volume() const
{
    // No API for this in Android
    return 1.0;
}

void QAndroidMediaRecorderControl::applySettings()
{
    m_session->applySettings();
}

void QAndroidMediaRecorderControl::setState(QMediaRecorder::State state)
{
    m_session->setState(state);
}

void QAndroidMediaRecorderControl::setMuted(bool muted)
{
    // No API for this in Android
    Q_UNUSED(muted)
    qWarning("QMediaRecorder::setMuted() is not supported on Android.");
}

void QAndroidMediaRecorderControl::setVolume(qreal volume)
{
    // No API for this in Android
    Q_UNUSED(volume)
    qWarning("QMediaRecorder::setVolume() is not supported on Android.");
}

QT_END_NAMESPACE
