﻿#include "cvvideo.h"


CVVideo::CVVideo(QQuickItem *parent) :
    QQuickItem(parent)
{

    size = QSize(640,480);
    connect(this, &QQuickItem::parentChanged, this, &CVVideo::changeParent);
    connect(this, SIGNAL(fileUrlChanged()), this, SLOT(fileUrlGeted()));

    // Open video right away
    // update();
}

CVVideo::~CVVideo()
{
    if(thread)
        thread->stop();
    delete thread;
    delete video;
    //Video release is automatic when cv::VideoCapture is destroyed
}

void CVVideo::changeParent(QQuickItem* parent)
{
    //FIXME: we probably need to disconnect the previous parent
    //TODO: probably a good idea to stop the camera (and restart it if we are auto-starting in this context)
}

void CVVideo::videoControl()
{
    //    qDebug() << "frameCount: " << frameCount;
    //    qDebug() << "frameNow:   " << frameNow;
    if(frameCount == frameNow && frameCount > 0)
    {
        frameNow = 0;
        thread->stop();

        video->setProperty(CV_CAP_PROP_POS_AVI_RATIO, 0);
        clearQVariant();
        isrunning = true;
        thread->start();
        return;
    }

    if(isrunning == true)
    {
        isrunning = false;
        thread->stop();
    }
    else
    {
        isrunning = true;
        thread->start();
    }
}

int CVVideo::getFrameCount()
{
    return frameCount;
}

void CVVideo::fileUrlGeted()
{
    // Open video now
    qDebug() << "signal getUrl: " << fileUrl << endl;
    update();
}

QString CVVideo::getFileUrl() const
{
    return fileUrl;
}

void CVVideo::setFileUrl(QString fileUrl)
{
    if(fileUrl != NULL && this->fileUrl != fileUrl)
    {
        this->fileUrl = fileUrl;
        isrunning=true;
        // qDebug() << "set getUrl: " << fileUrl << endl;
        emit fileUrlChanged();
        // update();
    }
}

QSize CVVideo::getSize() const
{
    return size;
}

void CVVideo::setSize(QSize size)
{
    if(this->size.width() != size.width() || this->size.height() != size.height()){
        this->size = size;
        update();
        emit sizeChanged();
    }
}

cv::Mat CVVideo::getCvImage()
{
    if(!exportCvImage){
        exportCvImage = true;
        update();
    }
    return cvImage;
}

int CVVideo::getFrameNow() const
{
    return frameNow;
}

int CVVideo::getTotalNow() const
{
    return totalNow;
}

int CVVideo::getNoHatNow() const
{
    return noHatNow;
}

void CVVideo::setFrameNow(int f)
{
    frameNow = f;
}


void CVVideo::allocateCvImage()
{
    // delete[] cvImageBuf;
    cvImageBuf = new unsigned char[size.width() * size.height() * 3];
    cvImage = cv::Mat(size.height(), size.width(), CV_8UC3, cvImageBuf);
}

void CVVideo::allocateVideoFrame()
{
    videoFrame = new QVideoFrame(size.width() * size.height() * 4, size, size.width() * 4, VIDEO_OUTPUT_FORMAT);
}

void CVVideo::clearQVariant()
{
    qFrameNow.clear();
    qBoxID.clear();
    qObjID.clear();
    qX.clear();
    qY.clear();
    qW.clear();
    qH.clear();
    qProb.clear();
}

void CVVideo::update()
{
    BetterVideoCapture precap;
    #ifdef _WIN32
    fileUrl = fileUrl.mid(8);  //fix windows cannot parse "file://"
    #endif
    fileUrl ="/"+ fileUrl.mid(8);
    precap.open(fileUrl);
    int w = precap.getProperty(CV_CAP_PROP_FRAME_WIDTH);
    int h = precap.getProperty(CV_CAP_PROP_FRAME_HEIGHT);
    // precap.~BetterVideoCapture();
    qDebug() << "w: " << w << " h: " << h;
    size = QSize(w, h);
    frameCount = precap.getProperty(CV_CAP_PROP_FRAME_COUNT);
    fps = precap.getProperty(CV_CAP_PROP_FPS);
    frameNow = 0;

    qDebug() << "Opening vi " << fileUrl << ", Size: " << size;
    //qDebug("Opening video %s, width: %d, height: %d\n", fileUrl.toStdString(), size.width(), size.height());

    //Destroy old thread, camera accessor and buffers
    clearQVariant();
    delete thread;
    //video->close();
    delete video;
    if(videoFrame && videoFrame->isMapped())
        videoFrame->unmap();
    delete videoFrame;
    videoFrame = NULL;
    delete[] cvImageBuf;
    cvImageBuf = NULL;

    //Create new buffers, camera accessor and thread
    if(exportCvImage)
        allocateCvImage();
    if(videoSurface)
        allocateVideoFrame();

    video = new BetterVideoCapture();
    thread = new VideoThread(&results, video, videoFrame, cvImageBuf, size.width(), size.height());
    connect(thread, SIGNAL(imageReady()), this, SLOT(imageReceived()));

    //Open newly created device
    try{
        if(video->open(fileUrl)){

            // video->setProperty(CV_CAP_PROP_FRAME_WIDTH,size.width());
            // video->setProperty(CV_CAP_PROP_FRAME_HEIGHT,size.height());
            if(videoSurface){
                if(videoSurface->isActive())
                    videoSurface->stop();
                if(!videoSurface->start(QVideoSurfaceFormat(size, VIDEO_OUTPUT_FORMAT)))
                    DPRINT("Could not start QAbstractVideoSurface, error: %d",videoSurface->error());
            }
            thread->start();
            qDebug() << "Opened video " << fileUrl;
        }
        else
            qDebug() << "Could not open video " << fileUrl;
    }
    catch(int e){
        qDebug("Exception %d",e);
    }

}

void CVVideo::imageReceived()
{
    //Update VideoOutput
    if(videoSurface)
    {
        if(!videoSurface->present(*videoFrame))
            DPRINT("Could not present QVideoFrame to QAbstractVideoSurface, error: %d",videoSurface->error());

        frameNow++;
        qDebug() << "frameNow: " << frameNow << "/" << frameCount;
        // QVariantList qFrameNow, qBoxID, qObjID, qX, qY, qW, qH, qProb;
        for(size_t i = 0; i < results.size(); i++)
        {
            qFrameNow << frameNow;
            qBoxID << (int)i;
            qObjID << results[i].obj_id;
            qX << results[i].x;
            qY << results[i].y;
            qW << results[i].w;
            qH << results[i].h;
            qProb << results[i].prob;
        }

        noHatNow = 0;
        for(size_t i = 0; i < results.size(); i++)
        {
            int classid = results[i].obj_id;
            if(classid == 1)
                noHatNow++;
        }
        totalNow = results.size();
        if (noHatNow > 0)
        {
            if(sound->isFinished())
                sound->play();
        }

        if(frameNow == frameCount)
        {
            qDebug() << "Save detection results to database, size: " << qFrameNow.size();
            df->insertVideo(fileUrl, fps, frameCount);
            bool res = df->insertDetection(fileUrl, qFrameNow, qBoxID, qObjID, qX, qY, qW, qH, qProb);
            qDebug() << "Save detection status: " << res;
            qDebug() << "After save detection results, QVariantList size: " << qFrameNow.size();
        }
        emit frameNowChanged();
    }
    //Update exported CV image
    if(exportCvImage)
        emit cvImageChanged();
}

QAbstractVideoSurface* CVVideo::getVideoSurface() const
{
    return videoSurface;
}

void CVVideo::setVideoSurface(QAbstractVideoSurface* surface)
{
    if(videoSurface != surface){
        videoSurface = surface;
        // update();
    }
}
























