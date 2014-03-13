#include "stdafx.h"
#include "ciratefiAlgorithm.h"

using namespace std;
using namespace cv;

namespace Ciratefi
{
	void CiratefiData::CountParameter(Mat& templateImage)
	{
		if (_scaleNum>1) _passoesc=exp(log(_finalScale/_initialScale)/_scaleNum); else _passoesc=1.0;
		_angleDegree=360.0/_angleNum;
		_angleRadian = _angleDegree * M_PI / 180.0;
		_finalRadius=scale(_scaleNum-1)*(templateImage.rows/2);
		_templateRadius=templateImage.rows/2;
		if (_circleNum>1) _circleDistance=(_finalRadius-_initialRadius)/(_circleNum-1); else _circleDistance=0.0;
	}

	double CiratefiData::CircularSample(Mat& image, int y, int x, int radius)
	{ 
		int y2=0; int x2=radius; int sum=0; int count=0;
		int r2=radius*radius;
		while (x2>0) 
		{
			sum+=*(image.data+image.step[0]*(y+y2)+image.step[1]*(x+x2));
			sum+=*(image.data+image.step[0]*(y-y2)+image.step[1]*(x-x2));
			sum+=*(image.data+image.step[0]*(y+x2)+image.step[1]*(x-y2));
			sum+=*(image.data+image.step[0]*(y-x2)+image.step[1]*(x+y2));

			count=count+4;

			int mh=abs((y2+1)*(y2+1)+x2*x2-r2);
			int md=abs((y2+1)*(y2+1)+(x2-1)*(x2-1)-r2);
			int mv=abs(y2*y2+(x2-1)*(x2-1)-r2);
			int m=min(min(mh, md), mv);
			if (m==mh) y2++;
			else if (m==md) { y2++; x2--; }
			else x2--;
		}
		if (count>0)
		{
			return clip(((double)sum+(double)count/2.0)/(double)count, 0.0, 255.0);
		}
		return *(image.data+y*image.step[0]+x*image.step[1]);
	}

	void CiratefiData::Cisssa(Mat& sourceImage)
	{
		_ca.resize(_circleNum*sourceImage.rows*sourceImage.cols,-1.0);
		int n=sourceImage.rows*sourceImage.cols;
		int smallestRadius=ceil(scale(0)*_templateRadius);
		int lastRow=sourceImage.rows-smallestRadius;
		int lastCol=sourceImage.cols-smallestRadius;

		for (int s=0; s<_circleNum; s++) 
		{
			int sn=s*n;
			int radius=round(_circleDistance*s+_initialRadius);
			for (int y=smallestRadius; y<lastRow; y++)
			{
				int rn=y*sourceImage.cols;
				for (int x=smallestRadius; x<lastCol; x++) 
				{
					if(y+radius<sourceImage.rows && y-radius>=0 && x+radius<sourceImage.cols && x-radius>=0)
					{
						_ca[sn+rn+x]=CircularSample(sourceImage, y, x, radius);
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
		for (int s=0; s<_scaleNum; s++) 
		{
			int sn=s*_circleNum;
			double scaleRatio=scale(s);
			int length=ceil(scale(s)*templateImage.rows);

			resize(templateImage, resizedTemplate, Size(length, length));
			int resizedCircleNum=min((int)floor(scaleRatio/scale(_scaleNum-1)*_circleNum),_circleNum);
			int templateRowCenter=(resizedTemplate.rows-1)/2;
			int templateColCenter=(resizedTemplate.cols-1)/2;
			for (int c=0; c<resizedCircleNum; c++) 
			{
				_cq[sn+c]=CircularSample(resizedTemplate,templateRowCenter,templateColCenter,round(c*_circleDistance+_initialRadius));
			}
		}
	}

	void CiratefiData::Cifi(Mat& sourceImage)
	{
		vector<vector<double> > cqi(_scaleNum);
		vector<double> cqi2(_scaleNum,0);
		for (int s=0; s<_scaleNum; s++) 
		{
			int resizedCircleNum=_circleNum-1;
			int sn=s*_circleNum;
			while (_cq[sn+resizedCircleNum]<0.0 && 0<=resizedCircleNum) resizedCircleNum--;
			resizedCircleNum++;
			if (resizedCircleNum<3) MessageBox(NULL, "There has a resizedTemplate with less than 3 circle", "Error", MB_ICONERROR | MB_OK);
			cqi[s].resize(resizedCircleNum);
			double meanCqi=0;
			for(int i=0; i< resizedCircleNum; i++)
			{
				cqi[s][i]=_cq[sn+i];
				meanCqi+=_cq[sn+i];

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
		int smallestRadius=ceil(scale(0)*_templateRadius);
		int lastRow=sourceImage.rows-smallestRadius;
		int lastCol=sourceImage.cols-smallestRadius;
		_cis.reserve(n);
		vector<double> Y;
		for (int y=smallestRadius; y<lastRow; y++) 
		{
			int rn=y*sourceImage.cols;
			for (int x=smallestRadius; x<lastCol; x++) 
			{
				double maxCoef=-2;
				int fitScale;
				for (int s=0; s<_scaleNum; s++) 
				{
					vector<double>& X=cqi[s];
					double X2=cqi2[s];
					Y.resize(cqi[s].size());
					double meanY=0;
					double Y2=0;
					for (int i=Y.size()-1; i>=0; i--)
					{
						Y[i]=_ca[i*n+rn+x];
						if(Y[i]<0.0)
						{
							meanY=-1;
							break;
						}
						meanY+=Y[i];
					}
					if(meanY<0) continue;
					meanY/=(double)Y.size();
					for(int i=0;i<Y.size();i++)
					{
						Y[i]-=meanY;
						Y2+=Y[i]*Y[i];
					}

					double coef=0;
					for(int i=0; i<X.size(); i++)
					{
						coef+=X[i]*Y[i];
					}
					coef/=sqrt(X2*Y2);
					if (_isMatchNegative==true) coef=abs(coef);
					if (coef>maxCoef) 
					{
						maxCoef=coef;
						fitScale=s;
					}
				}
				if (maxCoef>_scaleThreshold) 
				{
					_cis.push_back(CorrData(y, x, fitScale, -1, maxCoef));
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

	double CiratefiData::RadialSample(Mat& image, int centerY, int centerX, double angle, double radius)
	{
		//������8�����p��
		int sum=0; int count=0;
		int x,y,dx,dy,a,err,dx2,dy2,sobe;
		int x1=centerX; int x2=centerX+round(cos(angle)*radius);
		int y1=centerY; int y2=centerY-round(sin(angle)*radius);
		dx=x2-x1; dy=y2-y1;

		if (abs(dx)>=abs(dy))//���ߥN�����Ϊ�1, 4, 5, 8��
		{ 
			if (dx<0) 
			{ swap(x1,x2); swap(y1,y2); dx=-dx; dy=-dy; }
			a=abs(2*dy); err=0; dx2=dx*2;
			sobe=(dy==0)?0:((dy>0)?1:-1); y=y1;
			for (x=x1; x<=x2; x++) 
			{
				sum+=*(image.data+y*image.step[0]+x*image.step[1]); count++; err=err+a;
				if (err>=dx) { y=y+sobe; err=err-dx2; }
			}
		} 
		else//�N�����Ϊ�2, 3, 6, 7��
		{
			if (dy<0) { swap(x1,x2); swap(y1,y2); dx=-dx; dy=-dy; }
			a=abs(2*dx); err=0; dy2=dy*2;
			sobe=(dx==0)?0:((dx>0)?1:-1); x=x1;
			for (y=y1; y<=y2; y++) 
			{
				sum+=*(image.data+y*image.step[0]+x*image.step[1]); count++; err=err+a;
				if (err>=dy) { x=x+sobe; err=err-dy2; }
			}
		}
		if (count>0)
		{
			return clip(((double)sum+(double)count/2.0)/(double)count, 0.0, 255.0);
		}
		return *(image.data+centerY*image.step[0]+centerX*image.step[1]);
	}

	void CiratefiData::Rassq(Mat& templateImage)
	{
		_rq.resize(_angleNum);
		for (int a=0; a<_angleNum; a++)
		{
			_rq[a]=RadialSample(templateImage, _templateRadius, _templateRadius, _angleRadian*a,_templateRadius);
		}
	}

	void CiratefiData::Rafi(Mat& sourceImage)
	{
		vector<double> x(_angleNum);
		for (int k=0; k<_angleNum; k++) x[k]=_rq[k];
		double meanX=0;
		double x2=0;
		for(int i=0;i<_angleNum;i++)
		{
			meanX+=x[i];
		}
		meanX/=_angleNum;
		for(int i=0;i<_angleNum;i++)
		{
			x[i]-=meanX;
			x2+=x[i]*x[i];
		}

		_ras.clear();
		int n=sourceImage.rows*sourceImage.cols;
		_ras.reserve(sourceImage.rows*sourceImage.cols);
		vector<double> y(_angleNum);
		for (int i=0; i<_cis.size(); i++) 
		{
			CorrData& candidate=_cis[i];

			double scaleRatio=scale(candidate.GetScale());
			double angleRange=2.0*M_PI/(double)_angleNum;
			int row=candidate.GetRow();
			int col=candidate.GetCol();
			double maxCoef=-2; int angle=0;

			double meanY=0;
			for (int s=0; s<_angleNum; s++)
			{
				y[s]=RadialSample(sourceImage,col,row,s*angleRange,_templateRadius*scaleRatio);
				meanY+=y[s];

			}
			meanY/=(double)_angleNum;
			double y2=0;
			for(int i=0; i<_angleNum; i++)
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

			for (int i=0; i<_angleNum; i++) 
			{
				double coef=0;
				for(int i=0; i<_angleNum; i++)
				{
					coef+=x[i]*y[i];
				}
				coef/=sqrt(x2*y2);

				if (_isMatchNegative) coef=abs(coef);
				if (coef>maxCoef) 
				{
					maxCoef=coef; angle=i;
				}
				rotate(x.rbegin(),x.rbegin()+1,x.rend());
			}
			if (maxCoef>_angleThreshold) 
			{
				_ras.push_back(CorrData(row, col, scaleRatio, angle, maxCoef));
			}
		}
	}

	Mat CiratefiData::DrawRafiResult(Mat& sourceImage)
	{
		Mat rafiResult;
		cvtColor(sourceImage, rafiResult, CV_GRAY2BGR);
		for(int i=0; i<_ras.size(); i++)
		{
			int row=_ras[i].GetRow();
			int col=_ras[i].GetCol();
			if(row>=rafiResult.rows && row<0 && col>=rafiResult.cols && col<0)
			{
				MessageBox(NULL, "DrawRafiResult: out of range", "Error", MB_ICONERROR | MB_OK);
				return Mat();
			}
			rafiResult.at<Vec3b>(row, col)=Vec3b((uchar)(_ras[i].GetCoefficient()*255.0),_ras[i].GetAngle(), 255);
		}		
		return rafiResult;
	}
}

