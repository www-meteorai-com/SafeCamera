﻿/*
    This file is part of JQTools

    Project introduce: https://github.com/188080501/JQTools

    Copyright: Jason

    Contact email: Jason@JasonServer.com

    GitHub: https://github.com/188080501/

    Edited by Lue Tao, 201806
*/

import QtQuick 2.7
import QtQuick.Controls 1.4
import "qrc:/MaterialUI/Interface/"
import Welcome 1.0

Item {
    id: welcome
    width: 820
    height: 590

    WelcomeManage {
        id: welcomeManage
    }

    MaterialLabel {
        x: 162
        y: 170
        text:
            "安全智能检测系统"
        font.pixelSize: 45
        anchors.verticalCenterOffset: -146
        anchors.horizontalCenterOffset: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    MaterialButton {
        x: 254
        y: 278
        width: 120
        height: 40
        text: "视频文件分析"
        anchors.verticalCenterOffset: 14
        anchors.horizontalCenterOffset: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        onClicked: {
            mainPageContains.showPage( "视频文件分析", "qrc:/Video/Video.qml" );
            // welcomeManage.openGitHubLink();
        }
    }

    MaterialButton {
        x: 254
        y: 278
        width: 120
        height: 40
        text: "实时视频分析"
        anchors.verticalCenterOffset: 74
        anchors.horizontalCenterOffset: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        onClicked: {
            mainPageContains.showPage( "实时视频分析", "qrc:/Camera/Camera.qml" );
            // welcomeManage.openLatestVersionDownloadLink();
        }
    }

    //MaterialButton {
    //    x: 254
    //    y: 278
    //    width: 120
    //    height: 40
    //    text: "信息查询"
    //    anchors.verticalCenterOffset: 134
    //    anchors.horizontalCenterOffset: 1
    //    anchors.horizontalCenter: parent.horizontalCenter
    //    anchors.verticalCenter: parent.verticalCenter
    //
    //    onClicked: {
    //        mainPageContains.showPage( "信息查询", "qrc:/Search/Search.qml" );
    //        // welcomeManage.openLatestVersionDownloadLink();
    //    }
    //}

    MaterialLabel {
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 3
        text: "作者：Perceptron,Meteor    当前版本：V" + welcomeManage.jqToolsVersionString();
        verticalAlignment: Text.AlignBottom
        horizontalAlignment: Text.AlignRight
        color: "#a1a1a1"
    }
}
