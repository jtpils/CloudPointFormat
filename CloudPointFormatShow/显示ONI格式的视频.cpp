#include <XnCppWrapper.h>
#include "highgui.h"
#include "cv.h"
using namespace xn; 
using namespace std; 
int ShowOni(string sFilename);
int main(int argc, char* argv[])
{
    string video = "1.oni";
    ShowOni(video);
    return 0;
}
int ShowOni(string sFilename)
{
    vector<IplImage*> depths,images;
    XnStatus nRetVal = XN_STATUS_OK;
    xn::Context g_context;     //���������Ķ���
    nRetVal=g_context.Init();     //�����Ķ����ʼ��
    g_context.OpenFileRecording(sFilename.c_str());     //�����е�oni�ļ�
    xn::ImageGenerator g_image;     //����image generator
    nRetVal=g_context.FindExistingNode(XN_NODE_TYPE_IMAGE,g_image);     //��ȡoni�ļ��е�image�ڵ�
    xn::DepthGenerator g_depth;     //����depth generator
    nRetVal=g_context.FindExistingNode(XN_NODE_TYPE_DEPTH,g_depth);     //��ȡoni�ļ��е�depth�ڵ�
    xn::ImageMetaData g_imd;     //����image�ڵ�Ԫ���ݶ���
    xn::DepthMetaData g_dmd;     //����depth�ڵ�Ԫ���ݶ���

    int nWidth;     //oni�����лҶ�/��ɫͼ��Ŀ��
    int nHeight;    //oni�����лҶ�/��ɫͼ��ĸ߶�
    IplImage *g_pImgColor=0;     //��������opencv��ʾ�Ĳ�ɫͼ��
    IplImage *g_pImgDepth=0;     //��������opencv��ʾ�����ͼ��
    IplImage *imgDepth16u=0;     //�洢�Ҷ���Ϣ
    IplImage *imgRGB8u=0;        //�洢��ɫ��Ϣ
    //����oni���ͼ��IplImage*��ʽvector
    XnUInt32 frameDepth = 0;
    while(1)
    {
        nRetVal = g_context.WaitOneUpdateAll(g_depth);     //�����������
        if(nRetVal!=XN_STATUS_OK)     //�ж��Ƿ���³ɹ�
        {
            printf("failed update depth image/n");
            continue;
        }
        if(g_depth.GetFrameID()<frameDepth)break;     //�ж��Ƿ�ѭ������
        else
        {
            g_depth.GetMetaData(g_dmd);     //��ȡg_depth��Ԫ����g_dmd
            if(g_pImgDepth==0)     //���ݵ�ǰ�����ͼ��Ԫ��������g_pImgDepth�Ĵ�С��ͨ����
            {
                nWidth=g_dmd.XRes();
                nHeight=g_dmd.YRes();
                g_pImgDepth=cvCreateImage(cvSize(nWidth,nHeight),8,1);
                imgDepth16u=cvCreateImage(cvSize(nWidth,nHeight),IPL_DEPTH_16U,1);
                cvZero(g_pImgDepth);
                cvZero(imgDepth16u);
            }
            memcpy(imgDepth16u->imageData,g_dmd.Data(),nWidth*nHeight*2);
            cvConvertScale(imgDepth16u,g_pImgDepth,255/4096.0,0);
            cvEqualizeHist(g_pImgDepth,g_pImgDepth);     //ֱ��ͼ���⻯��������Ȳ��
            depths.push_back(g_pImgDepth);     //���浽vector��
            frameDepth = g_depth.GetFrameID();//��¼��ǰframeID
            cvNamedWindow("���ͼ");
            cvShowImage("���ͼ",depths.front());     //�������
            char c=cvWaitKey(30);
            if(27==c)break;
        }

        //����oni��ɫͼ��IplImage*��ʽvector
        XnUInt32 frameImage = 0;
        nRetVal = g_context.WaitOneUpdateAll(g_depth);     //�����������
        if(nRetVal!=XN_STATUS_OK)     //�ж��Ƿ���³ɹ�
        {
            printf("failed update color image/n");
            continue;
        }
        if(g_image.GetFrameID()<frameImage)break;     //�ж��Ƿ�ѭ������
        else
        {
            g_image.GetMetaData(g_imd);     //��ȡg_image��Ԫ����g_imd
            if(g_pImgColor==0)     //���ݵ�ǰ�Ĳ�ɫͼ��Ԫ��������g_pImgColor�Ĵ�С��ͨ����
            {
                nWidth=g_imd.XRes();
                nHeight=g_imd.YRes();
                g_pImgColor=cvCreateImage(cvSize(nWidth,nHeight),8,3);
                imgRGB8u=cvCreateImage(cvSize(nWidth,nHeight),8,3);
                cvZero(g_pImgColor);
                cvZero(imgRGB8u);
            }
            memcpy(imgRGB8u->imageData,g_imd.Data(),nWidth*nHeight*3);
            cvCvtColor(imgRGB8u,g_pImgColor,CV_RGB2BGR);
            images.push_back(g_pImgColor);     //���浽vector��
            frameImage = g_image.GetFrameID(); //��¼��ǰframeID
            cvNamedWindow("RGBͼ");
            cvShowImage("RGBͼ",images.front());      //�������
            char c=cvWaitKey(30);
            if(27==c)break;
        }
    }

    //һ������
    if(g_pImgDepth)cvReleaseImage(&g_pImgDepth);
    if(g_pImgColor)cvReleaseImage(&g_pImgColor);
    if(imgDepth16u)cvReleaseImage(&imgDepth16u);
    if(imgRGB8u)cvReleaseImage(&imgRGB8u);
    g_context.StopGeneratingAll();
    g_context.Shutdown();
    cvDestroyAllWindows();
    return 0;
}




