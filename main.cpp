//500个点作为样本


#include<opencv2\opencv.hpp>
#include<iostream>
#include<vector>
#include<cmath>
#include<fstream>
using namespace std;
using namespace cv;
int cols, rows;
int maxPointNum = 15000;
struct GeneEle
{
	Point pos;//位置
	Scalar color;//颜色
};
struct Gene
{
	//基因组
	vector<GeneEle>g;
	//差值
	unsigned long distance = 0;//最大差值
};
//转换成图像
Mat getPicture(vector<GeneEle> ele)
{
	//一张白色图像
	Mat picture(1280,1241,CV_8UC3, Scalar(244,244,244));
	for (int i = 0; i < ele.size(); i++)
	{
		int thickness = -1;
		int lineType = 1;
		
		circle(picture,
			ele[i].pos,
			1,
			ele[i].color,
			thickness,
			lineType);
	}
	return picture;
}
//计算图像差值
unsigned long Distance(Mat a,Mat b)
{
	unsigned long sum = 0;
	for (int i = 0; i < a.rows; i++)
	{
		for (int j = 0; j < a.cols; j++)
		{
			for(int k=0;k<3;k++)
				sum += 255 - abs(a.at<cv::Vec3b>(i, j)[k] - b.at<cv::Vec3b>(i, j)[k]);
		}
	}
	return sum;
}
//产生后代
Gene next(Gene A, Gene B)
{
	Gene n;
	int int3 = 0;
	int i = 0;
	int k = 0;
	//断开分割点
	while (true)
	{
		int div = rand() % 120;
		if (int3+div >= maxPointNum)
		{
			div = maxPointNum-int3;
		}
		//一部分
		if (i % 2 == 0)
		{
			for (int j = int3; j < int3+div; j++)
			{
				n.g.push_back(A.g[k]);
				k++;
			}
			int3 += div;
		}
		else
		{
			for (int j = int3; j < int3+div; j++)
			{
				n.g.push_back(B.g[k]);
				k++;
			}
			int3 += div;
		}
		if (int3 >= maxPointNum)
			break;
		i++;
	}
	//随机突变
	int Tnum = rand() % 10;//随机选择8个基因点
	if (Tnum > 5)
	{
		for (int i = 0; i < 1; i++)
		{
			//随机选择点位
			int index = rand() % n.g.size();
			GeneEle ele;
			ele.pos.x = rand() % cols;
			ele.pos.y = rand() % rows;
			ele.color = n.g[index].color;
			n.g[index] = ele;
		}
	}
	else
	{
		for (int i = 0; i < 1; i++)
		{
			//随机选择点位
			int index = rand() % n.g.size();
			GeneEle ele;
			ele.pos.x = n.g[index].pos.x;
			ele.pos.y = n.g[index].pos.y;
			ele.color = Scalar(rand() % 255, rand() % 255, rand() % 255);
			n.g[index] = ele;
		}
	}
	return n;
}

Gene getParent(vector<Gene> gene,unsigned long sum)
{
	int getNum = rand() % sum;
	int temp = 0;
	for (int i = 0; i < gene.size(); i++)
	{
		temp += gene[i].distance;//加上之前的值
		//可以获得
		if (temp >= getNum)
		{
			return gene[i];
		}
	}
}

void suit(Gene & gene,Mat&src)
{
		//转换成图像
		Mat t = getPicture(gene.g);
		//得到图像相似程度
		gene.distance = Distance(src, t);
}
//计算最小个体值
unsigned long minIndiv(vector<Gene>& gene)
{
	int mins = gene[0].distance;
	for (int i = 0; i < gene.size(); i++)
	{
		if (gene[i].distance < mins)
			mins = gene[i].distance;
	}
	return mins;
}
//计算最大个体值
unsigned long maxIndiv(vector<Gene>& gene)
{
	int maxs = gene[0].distance;
	int index = 0;

	for (int i = 0; i < gene.size(); i++)
	{
		if (gene[i].distance >maxs)
		{
			maxs = gene[i].distance;
			index = i;
		}
	}
	imwrite("2.jpg",getPicture(gene[index].g));
	ofstream fout("a.txt",ios::out);
	for (int i = 0; i < gene[index].g.size(); i++)
	{
		fout << gene[index].g[i].pos.x << " " << gene[index].g[i].pos.y << endl;
		fout << gene[index].g[i].color.val[0] << " " << gene[index].g[i].color.val[1] << " "<<gene[index].g[i].color.val[2] << endl;
		
		
	}
	fout.close();
	//waitKey(3);
	return maxs;
}
//淘汰个体
void dieOut(vector<Gene>& gene,unsigned long mins)
{
	for (auto i = gene.begin(); i != gene.end();)
	{
		if (i->distance <= mins)
			i = gene.erase(i);
		else
			++i;
	}
}

unsigned long getSum(vector<Gene>& gene)
{
	unsigned long sum = 0;
	for (int i = 0; i < gene.size(); i++)
	{
		sum += gene[i].distance;
	}
	return sum;
}

void test()
{
	int pp = 10;
	Mat a(480,640,CV_8UC1);
	uchar* ptr = new uchar[640 * 480];

	memcpy(ptr,(void*)1,640*480);
}

int main()
{
	//test();
	srand((int)time(0));
	vector<Gene> libs;
	Mat src;
	src = imread("3.jpg");
	int mins=0, maxs=0;
	int sum = 0;
	cols = src.cols;
	rows = src.rows;
	//生成10个初始种群
	for (int i = 0; i < 10; i++)
	{
		Gene g;
		for (int j = 0; j < maxPointNum; j++)
		{
			//生成位置
			GeneEle ele;
			ele.pos.x = rand() % src.cols;
			ele.pos.y = rand() % src.rows;
			ele.color = Scalar(rand()%255,rand()%255,rand()%255);
			g.g.push_back(ele);
		}
		libs.push_back(g);
	}
	ifstream fin("a.txt", ios::in);
	Gene last;
	for (int i = 0; i < maxPointNum; i++)
	{
		GeneEle ele;
		fin >> ele.pos.x >> ele.pos.y;
		fin >> ele.color.val[0] >> ele.color.val[1] >> ele.color.val[2];
		last.g.push_back(ele);
	}
	fin.close();
	libs.push_back(last);
	for (int i = 0; i < libs.size(); i++)
	{
		suit(libs[i], src);
	}
	int delta = 5000;
	unsigned long times = 0;
	while (1)
	{
		//计算种群对环境适应度
		times++;
		cout << "times:" << times << endl;
		//计算个体最大小值
		maxs = maxIndiv(libs);
		cout << "max --- [" << maxs << "]" << endl;
		//mins = minIndiv(libs);
		//淘汰不合适的个体
		dieOut(libs,(maxs)-delta);
		if (libs.size() == 1000)
		{
			delta -= 10;
			if (delta <= 0)
				delta = 2;
			continue;
		}
		cout <<"["<< libs.size() <<"]"<< endl;
		//当种群数目少了时候
		sum = getSum(libs);
		while (libs.size() < 1000)
		{
			//选出父母
			Gene m = getParent(libs, sum);
			Gene n = getParent(libs, sum);
			//生成子代
			Gene e = next(m,n);
			//计算子代适应度
			suit(e,src);
			libs.push_back(e);
		}
	}
	return 0;
}