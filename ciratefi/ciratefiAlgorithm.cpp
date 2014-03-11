#include "stdafx.h"
#include "ciratefiAlgorithm.h"

using namespace std;
using namespace cv;

namespace Ciratefi
{
	void CiratefiData::CountParameter(Mat& templateImage)
	{
		if (_scaleNum>1) _passoesc=exp(log(_finalScale/_initialScale)/_scaleNum); else _passoesc=1.0;
		_AngleDegree=360.0/_angleNum;
		_AngleRadian = _AngleDegree * M_PI / 180.0;
		if (_circleNum>1) _circleDistance=(_finalRadius-_initialRadius)/(_circleNum-1); else _circleDistance=0.0;
		if (_finalRadius<_initialRadius) _finalRadius=scale(_scaleNum-1)*((double)templateImage.rows/2);
		_templateRadius=(double)templateImage.rows/2;
		if (_circleNum>1) _circleDistance=(_finalRadius-_initialRadius)/((double)_circleNum-1); else _circleDistance=0.0;
	}

	Point CiratefiData::ValidImageRange(Point& position, cv::Mat& image)
	{
		position.x=(position.x>=image.cols)?image.cols-1:((position.x<0)?0:position.x);
		position.y=(position.y>=image.rows)?image.rows-1:((position.y<0)?0:position.y);
		return position;
	}

	double CiratefiData::CircularSample(Mat& image, int row, int col, int radius)
	{ 
		int row2=0; int col2=radius; int sum=0; int count=0;
		int r2=radius*radius;
		while (col2>0) 
		{
			sum+=*(image.data+image.step[0]*(row+row2)+image.step[1]*(col+col2));
			sum+=*(image.data+image.step[0]*(row-row2)+image.step[1]*(col-col2));
			sum+=*(image.data+image.step[0]*(row+col2)+image.step[1]*(col-row2));
			sum+=*(image.data+image.step[0]*(row-col2)+image.step[1]*(col+row2));

			count=count+4;

			int mh=abs((row2+1)*(row2+1)+col2*col2-r2);
			int md=abs((row2+1)*(row2+1)+(col2-1)*(col2-1)-r2);
			int mv=abs(row2*row2+(col2-1)*(col2-1)-r2);
			int m=min(min(mh, md), mv);
			if (m==mh) row2++;
			else if (m==md) { row2++; col2--; }
			else col2--;
		}
		if (count>0)
		{
			return clip(((double)sum+(double)count/2.0)/(double)count, 0.0, 255.0);
		}
		return image.at<uchar>(row,col);
	}

	void CiratefiData::Cisssa(Mat& sourceImage)
	{
		_ca.resize(_circleNum*sourceImage.rows*sourceImage.cols,-1.0);
		int n=sourceImage.rows*sourceImage.cols;
		double scaleRatio=scale(0);
		int smallRadius=ceil(scale(0)*_templateRadius);
		int lastRow=sourceImage.rows-smallRadius;
		int lastCol=sourceImage.cols-smallRadius;

		for (int circleNO=0; circleNO<_circleNum; circleNO++) 
		{
			int cirn=circleNO*n;
			int radius=round(_circleDistance*circleNO+_initialRadius);
			for (int row=smallRadius; row<lastRow; row++)
			{
				int rn=row*sourceImage.cols;
				for (int col=smallRadius; col<lastCol; col++) 
				{
					if(row+radius<sourceImage.rows && row-radius>=0 && col+radius<sourceImage.cols && col-radius>=0)
					{
						_ca[cirn+rn+col]=CircularSample(sourceImage, row, col, radius);
					}					
				}
			}
		}
	}

	Mat CiratefiData::quadradaimpar(Mat& image)
	{
		int length=min(image.rows,image.cols);
		if (length%2==0) length--;
		Mat tempRoi = image(Rect((image.cols-1)/2-length/2, (image.rows-1)/2-length/2, length, length));
		Mat roi(tempRoi.clone());
		return roi;
	}

	void CiratefiData::Cissq(Mat& templateImage)
	{
		_cq.resize(_scaleNum*_circleNum, -1.0);
		Mat resizedTemplate;
		for (int f=0; f<_scaleNum; f++) 
		{
			int sn=f*_circleNum;
			double scaleRatio=scale(f);
			int length=ceil(scaleRatio*templateImage.rows);

			resize(templateImage, resizedTemplate, Size(length, length));
			int resizedCircleNum=min((int)floor(scaleRatio/scale(_scaleNum-1)*(double)_circleNum),_circleNum);
			int templateRowCenter=(resizedTemplate.rows-1)/2;
			int templateColCenter=(resizedTemplate.cols-1)/2;
			for (int c=0; c<resizedCircleNum; c++) 
			{
				_cq[sn+c]=CircularSample(resizedTemplate,templateRowCenter,templateColCenter,round((double)c*_circleDistance+_initialRadius));
			}
		}
	}

	void CiratefiData::Cifi(cv::Mat& sourceImage, cv::Mat& templateImage)
	{
		vector<vector<double> > cqi(_scaleNum);
		vector<double> cqi2(_scaleNum,0);
		for (int s=0; s<_scaleNum; s++) 
		{
			int resizedCircleNum=_circleNum-1;
			int sn=s*_circleNum;
			while (_cq[sn+resizedCircleNum]<0.0 && 0<=resizedCircleNum) resizedCircleNum--;
			resizedCircleNum++;
			if (resizedCircleNum<3) MessageBox(NULL, "Query.mat has a row with less than 3 columns", "Error", MB_ICONERROR | MB_OK);
			cqi[s].resize(resizedCircleNum);
			double meanCqi=0;
			for(int i=0; i< resizedCircleNum; i++)
			{
				cqi[s][i]=_cq[sn+i];
				meanCqi+=cqi[s][i];

			}
			meanCqi/=(double)resizedCircleNum;
			for(int i=0;i<resizedCircleNum;i++)
			{
				cqi[s][i]-=meanCqi;
				cqi2[s]+=cqi[s][i]*cqi[s][i];
			}
		}

		_cis.clear();
		int n=sourceImage.rows*sourceImage.cols;
		double scaleRatio=scale(0);
		int smallRadius=ceil(scale(0)*_templateRadius);
		int lastRow=sourceImage.rows-smallRadius;
		int lastCol=sourceImage.cols-smallRadius;
		_cis.reserve(n);
		vector<double> y;
		for (int row=smallRadius; row<lastRow; row++) 
		{
			int rn=row*sourceImage.cols;
			for (int col=smallRadius; col<lastCol; col++) 
			{
				double maxCoef=-2;
				int maxScale=0;
				for (int s=0; s<_scaleNum; s++) 
				{
					vector<double>& x=cqi[s];
					double x2=cqi2[s];
					y.resize(cqi[s].size());
					double meanY=0;
					double y2=0;
					for (int k=y.size()-1; k>=0; k--)
					{
						y[k]=_ca[k*n+rn+col];
						if(y[k]<0.0)
						{
							meanY=-1;
							break;
						}
						meanY+=y[k];
					}
					if(meanY<0) continue;
					meanY/=(double)y.size();
					for(int i=0;i<y.size();i++)
					{
						y[i]-=meanY;
						y2+=y[i]*y[i];
					}

					double coef=0;
					for(int i=0; i<x.size(); i++)
					{
						coef+=x[i]*y[i];
					}
					coef/=sqrt(x2*y2);
					if (_isMatchNegative==true) coef=abs(coef);
					if (coef>maxCoef) 
					{
						maxCoef=coef;
						maxScale=s;
					}
				}
				if (maxCoef>_scaleThreshold) 
				{
					_cis.push_back(CorrData(row, col, maxScale, -1, maxCoef));
				}
			}
		}
	}

	Mat CiratefiData::DrawCifiResult(Mat& sourceImage)
	{
		Mat cifiResult;
		cvtColor(sourceImage, cifiResult, CV_GRAY2BGR);
		for(int i=0; i<_cis.size(); i++)
		{
			int row=_cis[i].GetRow();
			int col=_cis[i].GetCol();
			if(row>=cifiResult.rows && row<0 && col>=cifiResult.cols && col<0)
			{
				MessageBox(NULL, "DrawCifiResult: out of range", "Error", MB_ICONERROR | MB_OK);
				return Mat();
			}
			cifiResult.at<Vec3b>(row, col)=Vec3b((uchar)(_cis[i].GetCoefficient()*255.0),_cis[i].GetScale(), 255);
		}		
		return cifiResult;
	}
}

