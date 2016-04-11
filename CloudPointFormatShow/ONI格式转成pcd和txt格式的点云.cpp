#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <XnCppWrapper.h>
#include <vector>
#include "highgui.h"
#include "cv.h"
#include <fstream>
using namespace pcl;
using namespace pcl::console;
using namespace xn; 
using namespace std; 
void OniToPcd(string sFilename);
int ShowOni(string sFilename);
float  TimeStart,TimeEnd,TimeUsed,TimeStart1,TimeEnd1,TimeUsed1;
pcl::PointCloud<pcl::PointXYZ> cloud;
int main(int argc, char* argv[])
{
    string video = "1.oni";
    ShowOni(video);	//��ʾoni��ʽ��Ƶ
    OniToPcd(video);//����ȡ����oni��Ϣת��Ϊpcd��ʽ�ĵ��ƺ�txt��ʽ�ĵ��ƣ����Ƚϴ洢���������ʱ��
    return 0;
}
//point cloud data struct
struct SColorPoint3D
{    
    float  X;
    float  Y;
    float  Z;

    SColorPoint3D( XnPoint3D pos)
    {
        X = pos.X;
        Y = pos.Y;
        Z = pos.Z;

    }
};

vector<SColorPoint3D>vPointCloud;

void OniToPcd(string sFilename)
{

    //DepthGenerator& rDepthGen;

    XnStatus eResult = XN_STATUS_OK;
    int i = 0;
    xn::Context mContext;     //���������Ķ���
    eResult = mContext.Init();    //�����Ķ����ʼ��
    mContext.OpenFileRecording(sFilename.c_str());     //�����е�oni�ļ�

    DepthGenerator mDepthGenerator;
    eResult = mDepthGenerator.Create(mContext);
    ImageGenerator mImageGenerator;
    eResult = mImageGenerator.Create(mContext);
    xn::UserGenerator userGnrt;
    eResult  =       userGnrt.Create(mContext);
    // set output mode
    XnMapOutputMode mapMode;
    mapMode.nXRes = XN_VGA_X_RES;
    mapMode.nYRes = XN_VGA_Y_RES;
    mapMode.nFPS  = 30;
    eResult = mDepthGenerator.SetMapOutputMode(mapMode);
    eResult = mImageGenerator.SetMapOutputMode(mapMode);
    // start generating
    eResult = mContext.StartGeneratingAll();
    //eResult =mContext.WaitNoneUpdateAll();

    // read data
    int nFrame=0;
    while ( nFrame<20 &&!mContext.WaitAnyUpdateAll() )
    {
        //��ȡUserͼ��
        XnUInt16 nUser = userGnrt.GetNumberOfUsers();
        if(nUser == 0)
        {
            //���汳��
            continue;
        }
        // get the depth map
        const XnDepthPixel*  pDepthMap = mDepthGenerator.GetDepthMap();
        // generate point cloud
        vPointCloud.clear();
        const XnDepthPixel*  pDepth = mDepthGenerator.GetDepthMap();
        // number of point is the number of 2D image pixel
        DepthMetaData mDepthMD;
        mDepthGenerator.GetMetaData( mDepthMD );
        unsigned int uPointNum = mDepthMD.FullXRes() * mDepthMD.FullYRes();
        // build the data structure for convert
        XnPoint3D* pDepthPointSet = new XnPoint3D[ uPointNum ];
        unsigned int i, j, idxShift, idx;
        xn::SceneMetaData sceneMD;
        int m=0;
        for(XnUInt16 i = 1; i <=1;i++)
        {

            userGnrt.GetUserPixels(i,sceneMD);
            for( j = 0; j < mDepthMD.FullYRes(); ++j )

            {
                idxShift = j * mDepthMD.FullXRes();

                for( i = 0; i < mDepthMD.FullXRes(); ++i )
                {
                    idx = idxShift + i;

                    if (sceneMD(i,j) !=0)
                    {
                        pDepthPointSet[idx].X = i;
                        pDepthPointSet[idx].Y = j;
                        pDepthPointSet[idx].Z = pDepth[idx];
                        m++;
                    }
                    else
                    {
                        pDepthPointSet[idx].X = 0;
                        pDepthPointSet[idx].Y = 0;
                        pDepthPointSet[idx].Z = 0;
                        // m++;
                    }
                }
            }
        }

        // un-project points to real world
        XnPoint3D* p3DPointSet = new XnPoint3D[ uPointNum ];
        mDepthGenerator.ConvertProjectiveToRealWorld( uPointNum, pDepthPointSet, p3DPointSet );
        delete[] pDepthPointSet;

        for( i = 0; i <640*480;i++ )
        {
            if (p3DPointSet[i].X==0&&p3DPointSet[i].Y==0&&p3DPointSet[i].Z==0)
            {
                continue;
            }
            else
            {
                vPointCloud.push_back( SColorPoint3D( p3DPointSet[i]) );

            }

            //vPointCloud.push_back( SColorPoint3D( p3DPointSet[i]) );

            //	copy(vPointCloud.begin(),vPointCloud.begin(),cloud.begin());
        }

        // print point cloud
        cout.flags(ios::left);    //Left-aligned
        //cout << "Point number: " <<cloud.size() << endl;
        // ������д��data.pcd������
        cloud.width=vPointCloud.size();
        cloud.height=1;
        cloud.is_dense=false;
        cloud.points.resize(cloud.width*cloud.height);
        int k=0;

        for (vector<SColorPoint3D>::iterator iter = vPointCloud.begin();iter !=vPointCloud.end();iter++)
        {


            cloud.points[k].x=iter->X;
            cloud.points[k].y=iter->Y;
            cloud.points[k].z=iter->Z;
            k++;
        }
        /*for(pcl::PointCloud<pcl::PointXYZ>::iterator iter1 = cloud.begin();iter1 !=cloud.end();)
   {


          // cout<<iter1->z<<endl;
                if ((iter1->x==0)&&(iter1->y==0)&&(iter1->z==0))
                {
                        iter1=cloud.erase(iter1,cloud.end());
                }
                else
                {
                        ++iter1;
                }

   }*/

        static short  p=0;
        char namefile[50];
        TimeStart=GetTickCount();
        sprintf(namefile,"D:\\����\\%d.pcd",++p);
        pcl::io::savePCDFileASCII(namefile,cloud);
        TimeEnd=GetTickCount();
        TimeUsed=(TimeEnd-TimeStart)/1000;
        static short  p1=0;
        char namefile1[50];
        TimeStart1=GetTickCount();
        sprintf(namefile1,"D:\\����\\%d.txt",++p1);
        ofstream out(namefile1);
        if(out.is_open()){
            for (vector<SColorPoint3D>::iterator iter = vPointCloud.begin();iter !=vPointCloud.end();iter++)
            {out<<(iter->X)<<"  "<<(iter->Y)<<"  "<<(iter->Z)<<endl;}}
        TimeEnd1=GetTickCount();
        TimeUsed1=(TimeEnd1-TimeStart1)/1000;
        cout<<"�����"<<p <<"֡"<<"PCD��txt����ʱ��ֱ�Ϊ��"<<TimeUsed<<"��"<<TimeUsed1<<"�롣"<<endl;
        //cout<<"��������Ϊ��"<<cloud.size()<<endl;
        delete[] p3DPointSet;
        //cloud.clear();
        nFrame++;
    }

    //stop
    mContext.StopGeneratingAll();
    mContext.Shutdown();
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




