#include <pcl/visualization/cloud_viewer.h>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <iostream>
#include "imagehlp.h"
#pragma comment(lib,"imagehlp.lib")
using namespace std;

void view(string namefile);
void viewerPsycho (pcl::visualization::PCLVisualizer& viewer);
void viewerOneOff (pcl::visualization::PCLVisualizer& viewer);;
void TxtView(string namefile);
int user_data;

int
	main(int argc,char** argv)
{

   string txt= "1.txt";//����txt��ʾ
   TxtView(txt);
   char namefile[55];
   for (int i=1;i<39;i++)
   {
	   sprintf(namefile,"E:/aa/%d.txt",i);
	   TxtView(namefile);	 
   }
	return 0;
}
void TxtView(string namefile)
{
	//pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ> cloud1;
	ifstream file(namefile);//��ȡ
	if (!file)//����ļ������ڷ���-1
	{
		cout<<"�ļ�������2"<<endl;;
	}
	    int i=1;
		while(1)
		{
			
			float tempX,tempY,tempZ;
			if(!(file>>tempX))
				break;
			if(!(file>>tempY))
				break;
			if(!(file>>tempZ))
				break;
			else
				//vPointCloud.push_back(tempX,tempY,tempZ);
			i++;	  		
		 }
		cloud1.width    = i-1;
		cloud1.height   =1;
		cloud1.is_dense = false;
		cloud1.points.resize (cloud1.width * cloud1.height);
		ifstream file1(namefile);//��ȡ
		if (!file1)//����ļ������ڷ���-1
		{
			cout<<"�ļ�������2"<<endl;
		}
		int ii=0;
		while(1)
		{

			float tempX,tempY,tempZ;
			if(!(file1>>tempX))
				break;
			if(!(file1>>tempY))
				break;
			if(!(file1>>tempZ))
				break;
			else
			cloud1.points[ii].x=tempX;
			cloud1.points[ii].y=tempY;
	         cloud1.points[ii++].z=tempZ; 	  		
		}
       file.close();
	   char *path="D:\\����\\��ʱ�ļ���\\";
	   if (_access(path,0)!=0)
	   {
		   MakeSureDirectoryPathExists(path);
	   } 
	   char name[50];
	   static int p=0;
	   sprintf(name,"D:\\����\\��ʱ�ļ���\\%d.pcd",++p);
	   pcl::io::savePCDFileASCII(name,cloud1);
	view(name);
	
}
void view(string namefile)
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
	pcl::io::loadPCDFile<pcl::PointXYZ>(namefile,*cloud);
	if(pcl::io::loadPCDFile<pcl::PointXYZ>(namefile,*cloud)==-1)//*�򿪵����ļ�
	{
		PCL_ERROR("Couldn't read file 1.pcd\n");
	}
	static int p=0;
	char namefile2[55];
sprintf(namefile2,"��%d֡Cloud Viewer",p++);
	pcl::visualization::CloudViewer viewer(namefile2);
	//pcl::visualization::CloudViewer viewer("Cloud Viewer");
	//showCloud������ͬ���ģ��ڴ˴��ȴ�ֱ����Ⱦ��ʾΪֹ
	viewer.showCloud(cloud);
	//��ע�ắ���ڿ��ӻ�ʱֻ����һ��
	viewer.runOnVisualizationThreadOnce (viewerOneOff);
	//��ע�ắ������Ⱦ���ʱÿ�ζ�����
	viewer.runOnVisualizationThread (viewerPsycho);
	
	while (!viewer.wasStopped ())
	{
		//�ڴ˴����������������
        user_data++;
	}
	/*
	Sleep(1);
	viewer.wasStopped ();*/
}
void 
	viewerOneOff (pcl::visualization::PCLVisualizer& viewer)
{
     //viewer.setBackgroundColor (1.0, 0.5, 1.0);
	viewer.setBackgroundColor (0.0, 0.0, 0.0);

	pcl::PointXYZ o;
	o.x = 1.0;
	o.y = 0;
	o.z = 0;
	viewer.addSphere (o, 0, "sphere", 0);
	std::cout << "i only run once" << std::endl;

}

void 
	viewerPsycho (pcl::visualization::PCLVisualizer& viewer)
{
	static unsigned count = 0;
	std::stringstream ss;
	ss <<"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"<< count++;
	// ss << "Once per viewer loop: " << count++;
	viewer.removeShape ("text", 0);
	viewer.addText (ss.str(), 200, 300, "text", 0);
	//FIXME: possible race condition here:
	user_data++;
}