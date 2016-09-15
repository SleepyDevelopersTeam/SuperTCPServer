#ifndef VIEW_H
#define VIEW_H
#include <server.h>

#include <QVector>
#include <QWidget>
#include <opencv2/opencv.hpp>

class View : public QWidget
{
     Q_OBJECT

public:
    View();

private:
    QVector<char> rgbPicture;
    Server* w;
    void YUV_NV21_TO_RGB(QVector<char> &argb, const QVector<char> &yuv, const int width, const int height);
    void rgbToMat();
    cv::Mat img;
private slots:
    void showImage();
};

#endif // VIEW_H
