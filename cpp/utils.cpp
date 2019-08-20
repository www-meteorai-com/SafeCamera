#include "utils.h"
#include <QDesktopServices>
#include <QDir>
#include <QDebug>

//Detector *detector = new Detector("yolov3.cfg", "yolov3.weights");
//home/taing/workspace/Meteor-AI-master/cpp
Detector *detectorHat = new Detector("perceptron.cfg", "perceptron.weights");
//home/taing/workspace/Meteor-AI-master/cpp/
Detector *detector = new Detector("yolov3-tiny.cfg", "yolov3-tiny.weights");
//Detector *detector = new Detector("/home/taing/workspace/darknet/cfg/yolov3-spp.cfg", "/home/taing/sharefolder/Weight/yolov3-spp.weights");
//home/taing/workspace/darknet/cfg/      /home/taing/workspace/darknet/backup_adddata/safe_darknet_50370
Detector *detector2 = new Detector("safe_darknet.cfg", "safe_darknet_50370.weights");
//Detector *detector2 = new Detector("/home/taing/workspace/darknet/cfg/darknet53.cfg", "/home/taing/sharefolder/Weight/darknet53.weights");
//Detector *detector2 = new Detector("/home/taing/workspace/darknet/cfg/safe_d53.cfg", "/home/taing/workspace/darknet/backup_darknet53/safe_d53_16561.weights");
DataFace *df = new DataFace();

cv::VideoWriter *videoWriter = new cv::VideoWriter();

QSound *sound = new QSound("beep.wav");

QString getCurrentTime()
{
    QDateTime time = QDateTime::currentDateTime();
    return time.toString("yyyy-MM-dd-hh-mm-ss");
}

QString getCurrentTime2()
{
    QDateTime time = QDateTime::currentDateTime();
    return time.toString("yyyy-MM-dd hh:mm:ss");
}

QString getVideoDir()
{
    QString videoDir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    videoDir += "/hardhat_video_data/";
    QDir dir(videoDir);
    if(!dir.exists())
        dir.mkdir(videoDir);//只创建一级子目录，即必须保证上级目录存在

    // qDebug() << "videoDir: " << videoDir;
    return videoDir;
}
