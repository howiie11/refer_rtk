
#include "math.h"
#include "Probability.h"
#include "..\rtklib.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//  ����  n/2 �Ħ�����ֵ: ��(n/2)
extern double gam(int n)
{
	int i;
	double k;
	
	if(n%2) // nΪ����
	{
		k=1.772453850905516;
		i=1;
	}
	else
	{
		k=1.0;
		i=2;
	}
	
	while(i<=n-2)
	{
		k*=i/2.0;
		i+=2;
	}
	
	return k;
}

//////////////////////////////////////////////////////////////////////////
//	  ��̬(��˹)�ֲ�����ֵ:  p(-��,u)
extern double gaos(double u)
{
	double y,p,er;
	
	if(u<-5.0) return 0.0;
	if(u>5.0) return 1.0;
	
	y=fabs(u)/sqrt(2.0);
	
    p=1.0+y*(0.0705230784+y*(0.0422820123+y*(0.0092705272+
		y*(0.0001520143+y*(0.0002765672+y*0.0000430638)))));
	
	er =1- pow( p, -16.0 );
    p=(u<0.0)? 0.5-0.5*er: 0.5+0.5*er;
	
    return p;
}



//////////////////////////////////////////////////////////////////////////
// ��̬�ֲ��ķ�����, p(-��,u)=p ; ��֪p, ����u
extern double re_gaos(double p)
{
	double y;

	if(p==0.5) return 0.0;
	if(p>0.9999997) return 5.0;
	if(p<0.0000003) return -5.0;
	if(p<0.5) return -re_gaos(1.0-p);
	
	y=-log(4.0*p*(1.0-p));

	y=y*(1.570796288+y*(0.3706987906e-1
		+y*(-0.8364353589e-3+y*(-0.2250947176e-3
		+y*(0.6841218299e-5+y*(0.5824238515e-5
		+y*(-0.1045274970e-5+y*(0.8360937017e-7
		+y*(-0.3231081277e-8+y*(0.3657763036e-10
		+y*0.6936233982e-12))))))))));
	
	return sqrt(y);
	
}



//////////////////////////////////////////////////////////////////////////
// chi2�ֲ�����ֵ���ܶ�ֵ:
// ����ֵ: ����(0,x)�ϵĸ���p, f-�ܶ�ֵ
extern double chi2(int n,double x,double *f)
{
	double iai;
	double p,Ux;	
	double pi=3.14159265358979312;
	
	double y=x/2.0;

	if(n%2)
	{
		Ux=sqrt(y/pi)*exp(-y);
		p=2.0*gaos(sqrt(x))-1.0;
		iai=0.5;
	}
	else
	{
		Ux=y*exp(-y);
		p=1.0-exp(-y);
		iai=1.0;
	}
	
	while(iai!=0.5*n)
	{
		p = p-Ux/iai;
		Ux = Ux*y/iai;
		iai+=1.0;
	} 
	*f = Ux/x;
	return p;	
}



//////////////////////////////////////////////////////////////////////////
//	chi���ֲ��ķ�����:  p=F(0,x)
//  ��֪����ֵp,���ɶ�n������x 
extern double re_chi2(int n,double p)
{
	double x,u,w,x0;
	double f,pp,xx;

	if(p>0.9999999)p=0.9999999;
	if(n==1)
	{
		x=re_gaos((1.0-p)/2.0);
		return x*x;
	}
	
	if(n==2)return -2.0*log(1.0-p);
	
	u = re_gaos(p);
	w = 2.0/(9.0*n);
    x0 = 1.0-w+u*sqrt(w);
	x0 = n*x0*x0*x0;
	
	while(1)
	{
		f=0.0001;
		pp=chi2(n,x0,&f);
		if(f+1.0==1.0)return x0;
		xx=x0-(pp-p)/f;
		if(fabs(x0-xx)<0.001) return xx;
		
		x0=xx;
	}
	
}


//////////////////////////////////////////////////////////////////////////
//  ����F�ֲ�����ֵ: ����(0,x)�ϵĸ���p
//  ��֪x,n1,n2, ��p,f
//  f: x���ĸ����ܶ�ֵ
extern double F(int n1,int n2, double x,double *f)
{
	double y=n1*x/(n2+n1*x);
	double Ux;
	double p=B(n1,n2,y,&Ux);
	
	*f= Ux/x;
	return p;
}

//////////////////////////////////////////////////////////////////////////
//    F�ֲ��ķ�������p=F(0,x), ��֪p,����x
extern double re_F(int n1,int n2,double p)
{
	double d,f0;
	
	double a=2.0/(9.0*n1);
	double b=2.0/(9.0*n2);
	double a1=1.0-a;
	double b1=1.0-b;
	double u=re_gaos(p);
	double e=b1*b1-b*u*u;

	double a2,B,f,pp,df;
	
	if(e<=0.8)
	{
		a1=2.0*pow(n2+0.0,0.5*n2-1.0);
		a2=pow(n1+0.0,0.5*n2);
	    B=gam(n1)*gam(n2)/gam(n1+n2);
		f=a1/a2/B/(1-p);
		f0=pow(f,2.0/n2);		
	}
	else
	{
		f0=(a1*b1+u*sqrt(a1*a1*b+a*e))/e;
		f0=f0*f0*f0;
	}
	
	if(f0<0.0)f0=0.01;
	
	while(1)
	{
		pp=F(n1,n2,f0,&d);
	    df=(p-pp)/d;
		
		while(fabs(f0)<fabs(df))df/=2.0;
		f0=f0+df;
		
		if( fabs(df)/f0<0.0001 || fabs(df)<0.0001) //����Чλ�������Ƿ��˳�����
		{ 
			return f0;
		}
	}
	
}


//////////////////////////////////////////////////////////////////////////
//      B�ֲ�����ֵ: ����(0,x)�ϵĸ���p
//      ��֪x,n1,n2, ��q,Ux
//      Ux: x���ĸ����ܶ�ֵ
//   2008-3-5,  ������B�ֲ�����,
extern double B(int n1,int n2, double x,double  *Ux)
{
	int m1,m2;
	double Ix;
	
	double pi=3.14159265358979312;
	
	if(n1%2 && n2%2)   /*  n1,n2��������  */
	{
		*Ux=sqrt(x*(1.0-x))/pi;
		Ix=1.0-2.0*atan(sqrt((1.0-x)/x))/pi;
		m1=m2=1;
	}
	
	if(n1%2 && !(n2%2) ) /*  n1������,n2��ż��  */
	{
	       *Ux=sqrt(x)*(1.0-x)/2.0;
		   Ix=sqrt(x);
		   m1=1;
		   m2=2;
	}
	
	if(!(n1%2) && n2%2)  /*  n1��ż��,n2������  */
	{
		*Ux=x*sqrt(1.0-x)/2.0;
		Ix=1.0-sqrt(1.0-x);
		m1=2;
		m2=1;
	}
	
	if(!(n1%2) && !(n2%2))  /*  n1,n2����ż��  */
	{
		*Ux=x*(1.0-x);
		Ix=x;
		m1=m2=2;
	}
	
	while(m2!=n2)
	{
		Ix=Ix+2.0*(*Ux)/m2;
		(*Ux)=(*Ux)*(1.0+m1/(m2+0.0))*(1.0-x);
		m2=m2+2;
	}  
	
	while(m1!=n1)
	{    
		Ix=Ix-2.0*(*Ux)/m1;
		*Ux=(*Ux)*(1.0+m2/(0.0+m1))*x;
		m1=m1+2;
	}
	
	return Ix;
}




//////////////////////////////////////////////////////////////////////////
//        t(ѧ��)�ֲ��ķֲ�����ֵ�������t�Ļ���ֵ��  //nn ���ɶ�
extern double student(int nn, double t,double *f) 
{
	double x,P;

	if(t+1.0==1.0) return 0.5;
	
	x=t*t/(nn+t*t);
	
	P=0.5*B(1,nn,x,f);
	
	if(x<0.0) P=0.5-P;
	else      P=0.5+P;
	
	*f= *f/fabs(t);
	
	return P;
}

//////////////////////////////////////////////////////////////////////////
//   t�ֲ��ķ�������p=F(x),��֪p������x
extern double re_student(int n,double p)
{
	double x,pi,u;
	double f,F,dx;

	if(p<0.5)
	{
		return -re_student(n,1.0-p);
	}

    pi=3.14159265358979312;
	
	if(n==1)
	{
		x=tan(pi*(p-0.5));
	}
	else if(n==2)
	{
	    u=2.0*p-1.0;
		u=u*u;
		x=sqrt(2.0*u/(1.0-u));
	}
	else
	{
		x=re_gaos(p)*sqrt(n/(n-2.0));
		
		while(1)
		{
			f=0.00001; //�ܶ�ֵ
			F=student(n,x,&f);
			dx=(F-p)/f;
			x=x-dx;
			if(fabs(dx)<0.001) break;
		}
	}
	return x;
}



