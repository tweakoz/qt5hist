/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSensors module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
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

//Import the declarative plugins
import QtQuick 2.0
import Explorer 1.0

Rectangle {
    id: controlrect
    anchors.margins: 5
    color: "transparent"

    property PropertyInfo selectedItem: null;
    property alias listmodel: itemList.model

    onListmodelChanged: {
        itemList.currentIndex = -1;
        selectedItem = null;
    }

    Rectangle {
        id: itemListRect
        anchors.fill: parent
        color: "transparent"

        ListView {
            id: itemList
            anchors.fill: itemListRect
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            focus: true
            currentIndex: -1
            delegate: itemListDelegate
            clip: true

            Rectangle {
                id: scrollbar
                anchors.right: itemList.right
                y: itemList.visibleArea.yPosition * itemList.height
                width: 2
                height: itemList.visibleArea.heightRatio * itemList.height
                color: "black"
            }
        }

        Component {
            id: itemListDelegate

            Rectangle {
                width: itemList.width
                height: itemNameText.font.pixelSize + 3
                color: "transparent"

                Image {
                    id: backgroundImage
                    anchors.fill: parent
                    source: "content/listitem_select.png"
                    visible: itemList.currentIndex === index
                }

                Text {
                    id: itemNameText
                    text: model.modelData.name
                    font.pixelSize: 15
                    color: (itemList.currentIndex === index ? "#1c94ff" : "black")
                }

                Text {
                    id: itemValueText
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignRight
                    text: model.modelData.value
                    color: (itemList.currentIndex === index ? "#1c94ff" : "black")
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        itemList.currentIndex = index;
                        selectedItem = model.modelData;
                    }
                }
            }
        }
    }
}
