/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSensors module of the Qt Toolkit.
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

#include "qsensorgesturerecognizer.h"
#include "qsensorgesture_p.h"

#include "qsensorgesturemanager.h"
#ifdef SIMULATOR_BUILD
#include "qsensorgesturemanagerprivate_p.h"
#endif
QT_BEGIN_NAMESPACE

/*!
  \class QSensorGestureRecognizer
  \ingroup sensorgestures_recognizer
  \inmodule QtSensors

  \brief The QSensorGestureRecognizer class is the base class for a sensor gesture
 recognizer.

    QSensorGesture recognizer developers should sub-class this to implement their own recognizer.

    All sensor gesture recognizers have a detected(QString) signal. Implementors can use this
    and send recognizer specific gestures, such as detected("shake_left") or implement custom signals
    such as shakeLeft().

    These custom signals will be available in the QSensorGesture object at runtime.

    \sa QSensorGestureRecognizer::gestureSignals()

  */

/*!
  \fn void QSensorGestureRecognizer::create()

  Called by QSensorGesture object constructor to create the recognizers backend.

  Implementors would use this to instantiate QSensors and connect signals.

  */

/*!
  \fn QString QSensorGestureRecognizer::id() const
    Returns the identifier for this recognizer.
  */
/*!
  \fn  bool QSensorGestureRecognizer::start()

    Called by QSensorGesture::startDetection() to start this recognizer.
    Implementors should start the sensors.
    Returns true if the operation is successful.

  */
/*!
  \fn bool QSensorGestureRecognizer::stop()

    Called by QSensorGesture::stopDetection() to stop this recognizer.
    Returns true if the call succeeds, otherwise false.

    Implementors should stop the sensors.

  */
/*!
  \fn  bool QSensorGestureRecognizer::isActive()

  Returns true if this recognizer is active, otherwise false.
  */

/*!
  \fn QSensorGestureRecognizer::detected(const QString &gestureId)
  Signals when the \a gestureId gesture has been recognized.
  */

class QSensorGestureRecognizerPrivate
{
public:
    bool initialized;
    int count;
};


/*!
  Constructs the QSensorGestureRecognizer with \a parent as parent.
  */
QSensorGestureRecognizer::QSensorGestureRecognizer(QObject *parent)
    :QObject(parent),
      d_ptr(new QSensorGestureRecognizerPrivate())
{
}

/*!
    Destroy the QSensorGestureRecognizer
*/
QSensorGestureRecognizer::~QSensorGestureRecognizer()
{
   delete d_ptr;
}

/*!
  Returns a list of signals that this recognizer supports.

  Note that all signals declared will be exported to the QSensorGesture
  object. If you need to use signals that are not exported, you should use a private class
  to do so.

  */
QStringList QSensorGestureRecognizer::gestureSignals() const
{
    QStringList list;
    bool ok = false;
    for (int i = 0; i < this->metaObject()->methodCount(); i++) {
        //weed out objectsignals and slots
        const QByteArray sig(this->metaObject()->method(i).methodSignature());
        if (this->metaObject()->indexOfSignal(sig) != -1) {
            if (sig.contains("detected"))
                ok = true;
            if (ok)
                list.append(QString::fromLatin1(sig));
        }
    }
    return list;
}

/*!
    Calls QSensorGestureRecognizer::create() if the recognizer is valid.
*/
void QSensorGestureRecognizer::createBackend()
{
    if (d_ptr->initialized) {
        return;
    }
    d_ptr->initialized = true;
    create();
}

/*!
  Calls QSensorGestureRecognizer::start() if the recognizer isn't already initialized.
  This is called by the QSensorGesture object, so please use that instead.

\sa QSensorGesture::startDetection()

*/
void QSensorGestureRecognizer::startBackend()
{
  if (!d_ptr->initialized) {
        qWarning() << "Not starting. Gesture Recognizer not initialized";
        return;
    }
  if (d_ptr->count++ == 0) {
      start();
#ifdef SIMULATOR_BUILD
      QSensorGestureManagerPrivate::instance()->recognizerStarted(this);
#endif
  }
}

/*!
  Calls QSensorGestureRecognizer::stop() if no other clients are using it.
  This is called by the QSensorGesture object, so please use that instead.

\sa QSensorGesture::stopDetection()
*/
void QSensorGestureRecognizer::stopBackend()
{
    if (!d_ptr->initialized) {
        qWarning() << "Not stopping. Gesture Recognizer not initialized";
        return;
    }
    if (--d_ptr->count == 0) {
        stop();
#ifdef SIMULATOR_BUILD
        QSensorGestureManagerPrivate::instance()->recognizerStopped(this);
#endif
    }
}

QT_END_NAMESPACE
