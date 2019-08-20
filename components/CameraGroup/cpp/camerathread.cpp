#include "camerathread.h"
#include "utils.h"



CameraTask::CameraTask(std::vector<bbox_t> *results, BetterVideoCapture *camera, QVideoFrame *videoFrame, unsigned char *cvImageBuf, int width, int height)
{
    this->running = true;
    this->camera = camera;
    this->videoFrame = videoFrame;
    this->cvImageBuf = cvImageBuf;
    this->width = width;
    this->height = height;
    this->results = results;

}

CameraTask::~CameraTask()
{
    //Leave camera and videoFrame alone, they will be destroyed elsewhere
}

void CameraTask::stop()
{
    imageReady();
    running = false;
}

void CameraTask::doWork()
{

   //  //读取网络摄像头
   //
   //  cv::VideoCapture capture;
   //  cv::Mat frame;
   //
   //  //注意下面的连接部分，admin:123（账号密码），
   //  //@符号之后的是局域网ip地址(打开app后，点击下方“打开IP摄像头服务器”，会有显示局域网ip)
   //  //即：http://<USERNAME>:<PASSWORD>@<IP_ADDRESS>/<the value of src>
   //  capture.open("http://admin:123456@192.168.0.105:8081/");
   // //循环显示每一帧
   // while (1)
   // {
   //     capture >> frame;//读取当前每一帧画面
   //     imshow("读取视频", frame);//显示当前帧
   //     //cv::waitKey(30);//延时30ms
   // }
//   / return;

    running = true;


    if(videoFrame)
        videoFrame->map(QAbstractVideoBuffer::ReadOnly);

    cv::Mat screenImage;
    if(videoFrame)
        screenImage = cv::Mat(height,width,CV_8UC4,videoFrame->bits());

    while(running && videoFrame != NULL && camera != NULL)
    {
        if(!camera->grabFrame())
            continue;
        unsigned char* cameraFrame = camera->retrieveFrame();
        int rate= camera->getFPS(CV_CAP_PROP_FPS);

        long currentFrame = 0;
        //Get camera image into screen frame buffer
        if(videoFrame)
        {
            // Assuming desktop, RGB camera image and RGBA QVideoFrame
   //         cv::Mat tempMat(height, width, CV_8UC3, cameraFrame);
   //
   //         std::shared_ptr<image_t> img = detector->mat_to_image(tempMat);
   //         image_t *iii = img.get();
   //         std::vector<bbox_t> tmpres = detector->detect(*iii);
   //         results->assign(tmpres.begin(), tmpres.end());
   //
   //         qDebug() << "Person numbers: " << results->size();
   //         cv::putText(tempMat, getCurrentTime2().toStdString(), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
   //         videoWriter->write(tempMat);
   //
   //         int nohat = 0;
   //         for(size_t i = 0; i < results->size(); i++)
   //         {
   //             int classid = (*results)[i].obj_id;
   //             int x = (*results)[i].x;
   //             int y = (*results)[i].y;
   //             if(classid == 1)
   //             {
   //                 nohat++;
   //                 cv::putText(tempMat, "No Hardhat!", cv::Point(x, y-10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
   //                 //cv::Rect rec((*results)[i].x, (*results)[i].y, (*results)[i].w, (*results)[i].h);
   //                 //cv::rectangle(tempMat, rec, cv::Scalar(0, 0, 255), 4);
   //             }
   //             else
   //             {
   //                 cv::putText(tempMat, "Hardhat!", cv::Point(x, y-10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
   //
   //                 //cv::Rect rec((*results)[i].x, (*results)[i].y, (*results)[i].w, (*results)[i].h);
   //                // cv::rectangle(tempMat, rec, cv::Scalar(0, 250, 0), 4);
   //             }
    //        }
            // cv::putText(tempMat, "[Total] " + QString::number(results->size(), 10).toStdString() + "   [No Hardhat] " +
            //             QString::number(nohat, 10).toStdString(), cv::Point(10, tempMat.rows - 10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);




            cv::Mat src(height, width, CV_8UC3, cameraFrame);
            // Assuming desktop, RGB camera image and RGBA QVideoFrame
            cv::Mat tempMat;//(height, width, CV_8UC3, cameraFrame);

            cv::Size src_sz = src.size();
            cv::Size dst_sz(src_sz.width, src_sz.height);
            if(src_sz.width==1920&&src_sz.height==1080)
            {
               //旋转角度
               double angle = -90;

               //指定旋转中心
               cv::Point2f center(src_sz.width / 2., src_sz.height / 2.);

               //获取旋转矩阵（2x3矩阵）
               cv::Mat rot_mat = cv::getRotationMatrix2D(center, angle, 0.7);

               //根据旋转矩阵进行仿射变换
               cv::warpAffine(src, tempMat, rot_mat, dst_sz);
            }
            else
            {
                   tempMat=src;
            }
            if (currentFrame%10==0)
            {
                std::shared_ptr<image_t> img = detector->mat_to_image(tempMat);

                image_t *iii = img.get();
                clock_t start,finish;
                double totaltime;
                start=clock();
                std::vector<bbox_t> tmpres = detector->detect(*iii);

                results->assign(tmpres.begin(), tmpres.end());

                finish=clock();
                totaltime=(float)(finish-start)/CLOCKS_PER_SEC;
                std::cout<<"\n此程序的运行时间为"<<totaltime<<"秒！"<<std::endl;

                float rate_x=1;//(float)width/(float)detector->get_net_width();
                float rate_y=1;//(float)height/(float)detector->get_net_height();
                int nohat = 0;
                for(size_t i = 0; i < results->size(); i++)
                {
                    uint classid = (*results)[i].obj_id;

                    //when classid==0 is person
                    if(classid==0)
                    {
                        //expand rectangle size
                        float x = (*results)[i].x*rate_x;
                        float y = (*results)[i].y*rate_y;
                        float w = (*results)[i].w*rate_x;
                        float h = (*results)[i].h*rate_y;
                        if(w<2.0f*h/3.0f)
                        {
                            w =2.0f*h/3.0f;
                            if(x+w>width)
                            {
                                x=width-w;
                            }
                        }
                        if(y>15)
                        {
                            y=y-15;
                            h=h+15;
                        }
                        else
                        {
                            y=0;
                        }
                        if(x>15)
                        {
                            x=x-20;
                        }else {x=0;}
                        //if(y>15)
                        //{
                        //    y=y-15;
                        //}else {y=0;}


                        if(x<0)
                            x=0;
                        if(y<0)
                            y=0;
                        if(x+w>width)
                        {
                           w=width-x;
                        }
                        if(y+h>height)
                        {
                           h=height-y;
                        }
                        cv::Rect picrec((int)x, (int)y, (int)w, (int)h);
                        cv::Rect realrec((int)((*results)[i].x*rate_x),
                                         (int)((*results)[i].y*rate_y),
                                         (int)((*results)[i].w*rate_x),
                                         (int)((*results)[i].h*rate_y));


                        //Check SafeHat
                        std::shared_ptr<image_t> imghat=detectorHat->mat_to_image(tempMat);//(picrec)
                        image_t *iiihat = imghat.get();
                         std::vector<bbox_t> hatrs= detectorHat->detect(*iiihat);
                         results->assign(hatrs.begin(), hatrs.end());
                         for(size_t i = 0; i < results->size(); i++)
                        {
                          int classid = (*results)[i].obj_id;
                                       int x = (*results)[i].x*rate_x;
                                       int y = (*results)[i].y*rate_y;

                                      // cv::Rect rec((*results)[i].x*rate_x, (*results)[i].y*rate_y, (*results)[i].w*rate_x, (*results)[i].h*rate_y);
                                       if(classid == 1)
                                       {
                                           nohat++;
                                           cv::putText(tempMat, "No Hardhat!", cv::Point(x, y+10), cv::FONT_HERSHEY_SIMPLEX, 02, cv::Scalar(0, 0, 255), 2);

                                          // cv::rectangle(tempMat, rec, cv::Scalar(0, 0, 255), 4);
                                       }
                                       else
                                       {
                                           cv::putText(tempMat, "Hardhat!", cv::Point(x*rate_x, y*rate_y+10), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 2);
                                           //cv::Rect rec((*results)[i].x, (*results)[i].y, (*results)[i].w, (*results)[i].h);
                                          // cv::rectangle(tempMat, rec, cv::Scalar(0, 250, 0), 4);
                                       }
                        }

                        std::shared_ptr<image_t> img2 = detector2->mat_to_image_resize(tempMat(picrec));
                        image_t *iii2 = img2.get();
                        float *predition = detector2->detect2(*iii2);

                        //0,Posture
                        //6,SafeHat
                        //1,Smoke
                        //2,SafetyBelt

                        //3,ShortShirt
                        //4,Other
                        int classNum = 4;
                        float maxPredict = -1;
                        for (int var = 0; var < 5; ++var) {
                            if ( predition[var]>maxPredict) {
                                classNum = var;
                                maxPredict = predition[var];
                            }
                           std::cout<< predition[var]<< " " << var<< std::endl;
                        }

                        //cv::putText(tempMat, "Person", cv::Point(x, y-10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 1);


                        cv::String classstr="";
                        if(classNum == 0){

                            classstr="Posture";
                            //image_num++;
                            //if((*results)[i].w>0&&(*results)[i].h>0)
                            //{
                            //    cv::imwrite(std::to_string(image_num)+"-Person-"+getCurrentTime2().toStdString()+".jpg",tempMat(rec));
                            //}
                        }

                        else if (classNum == 1)
                        {
                           classstr="Smoke";
                        }
                        else if (classNum == 2)
                        {
                           classstr="SafetyBelt";
                        }
                        else if (classNum == 3)
                        {
                           //classstr="ShortShirt";
                        }
                        else if (classNum == 6)
                        {
                           classstr="SafeHat";
                        }
                        else {
                            //classstr="Other";
                        }


                        cv::rectangle(tempMat, picrec, cv::Scalar(0, 250, 0), 2);
                        cv::putText(tempMat, classstr, cv::Point(picrec.x, picrec.y+20), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 3);
                    }
                    }
                }
                else {
                    //QThread::msleep(1000/(rate*4));
                }

            cv::cvtColor(tempMat, screenImage, cv::COLOR_RGB2RGBA);
        }

        if(cvImageBuf)
        {
            // Assuming desktop, RGB camera image
            memcpy(cvImageBuf, cameraFrame, height*width*3);
        }

        emit imageReady();
    }

}


CameraThread::CameraThread(std::vector<bbox_t> *results, BetterVideoCapture *camera, QVideoFrame *videoFrame, unsigned char *cvImageBuf, int width, int height)
{
    task = new CameraTask(results, camera,videoFrame,cvImageBuf,width,height);
    task->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(started()), task, SLOT(doWork()));
    connect(task, SIGNAL(imageReady()), this, SIGNAL(imageReady()));
}

CameraThread::~CameraThread()
{
    stop();
    delete task;
}

void CameraThread::start()
{
    if(!workerThread.isRunning())
        workerThread.start();
    else
    {
        task->doWork();
    }
}

void CameraThread::stop()
{
    if(task != NULL)
        task->stop();
    workerThread.quit();
    workerThread.wait();
}














