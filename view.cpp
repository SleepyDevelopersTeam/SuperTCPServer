#include "view.h"

View::View()
{
    w = new Server();
    w->show();
    connect(w,SIGNAL(imageObtained()),this,SLOT(showImage()));

}

void View::YUV_NV21_TO_RGB(QVector<char> &argb,const QVector<char> &yuv,const int width,const int height)
{
     int frameSize = width * height;
     argb.clear();
          int ii = 0;
         int ij = 0;
         int di = +1;
         int dj = +1;

         for (int i = 0, ci = ii; i < height; ++i, ci += di) {
             for (int j = 0, cj = ij; j < width; ++j, cj += dj) {
                 int y = (0xff & ((int) yuv[ci * width + cj]));
                 int v = (0xff & ((int) yuv[frameSize + (ci >> 1) * width + (cj & ~1) + 0]));
                 int u = (0xff & ((int) yuv[frameSize + (ci >> 1) * width + (cj & ~1) + 1]));
                 y = y < 16 ? 16 : y;

                 int r = (int) (1.164f * (y - 16) + 1.596f * (v - 128));
                 int g = (int) (1.164f * (y - 16) - 0.813f * (v - 128) - 0.391f * (u - 128));
                 int b = (int) (1.164f * (y - 16) + 2.018f * (u - 128));

                 r = r < 0 ? 0 : (r > 255 ? 255 : r);
                 g = g < 0 ? 0 : (g > 255 ? 255 : g);
                 b = b < 0 ? 0 : (b > 255 ? 255 : b);

                 argb.push_back(b);//0xff000000 | (r << 16) | (g << 8) | b;
                 argb.push_back(g);
                 argb.push_back(r);
             }
         }
}

void View::rgbToMat()
{
    int k=0;
    for( int i = 0; i < img.rows; i++ )
        for( int j = 0; j < img.cols; j++ )
        {
            img.at<cv::Vec3b>(i,j)[0]=rgbPicture[k++];
            img.at<cv::Vec3b>(i,j)[1]=rgbPicture[k++] ;
            img.at<cv::Vec3b>(i,j)[2]=rgbPicture[k++] ;
        }
}

void View::showImage()
{
    YUV_NV21_TO_RGB(rgbPicture,w->data,w->width,w->height);
    img.create(w->height,w->width,CV_8UC3);
    rgbToMat();
    cv::imshow("lol",img);
}

