/* KDevelop
 *
 * Copyright 2011 Aleix Pol <aleixpol@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

import QtQuick 1.0
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1

Rectangle
{
    property Component tools: null
    property alias pageIcon: theIcon.icon
    property alias marginLeft: toolbar.width
    
    PlasmaCore.FrameSvgItem {
        imagePath: "widgets/tooltip"
        prefix: "base"
        id: toolbar
        anchors {
            top: parent.top
            left: parent.left
        }
        width: 256
        height: 100
        Loader {
            sourceComponent: tools
            anchors {
                fill: parent
                margins: 20
            }
        }
    }
    
    QIconItem {
        id: theIcon
        anchors {
            bottom: parent.bottom
            left: parent.left
            margins: 5
        }
        width: 256
        height: width
    }
}
