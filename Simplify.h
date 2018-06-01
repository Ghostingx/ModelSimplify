#pragma once
#include "SimpleObject.h"
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <iostream>
using namespace SimpleOBJ;
using namespace std;
const int maxn=500000;
struct Face
{
private:
	void _set(int a,int b,int c)
	{
		x=a;
		y=b;
		z=c;
		exist=true;
	}
	bool check(int& x,int k,int newID)
	{
		if(x==k)
		{
			x=newID;
			return true;
		}
		return false;
	}
public:
	int x,y,z;
	bool exist;
	void set(Array<int,3> face)
	{
		_set(face[0],face[1],face[2]);
	}
	bool has(int k,int newID)
	{
		bool f1=check(x,k,newID);
		bool f2=check(y,k,newID);
		bool f3=check(z,k,newID);
		return ((f1||f2)||f3);
	}
};
struct Matrix
{
	float a[4][4];

	Matrix(){}
	Matrix(const Matrix& A)
	{
		for(int i=0;i<4;++i)
			for(int j=0;j<4;++j)
				a[i][j]=A.a[i][j];
	}
	void clear()
	{
		for(int i=0;i<4;++i)
			for(int j=0;j<4;++j) a[i][j]=0;
	}
	void set(float* v)
	{
		for(int i=0;i<4;++i)
			for(int j=0;j<4;++j)
				a[i][j]=v[i]*v[j];
	}
};
Matrix operator+( const Matrix& a, const Matrix& b )
{
	Matrix ans;
	ans.clear();
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			ans.a[i][j] = a.a[i][j] + b.a[i][j];
	return ans;
}
float product(const Vec3f &a,const Vec3f &b )
{
	return a.x*b.x+a.y*b.y+a.z*b.z;
}
Vec3f VtProduct(const Vec3f &b,const Vec3f &a )
{
	Vec3f v(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
	return v;
}
struct Faces
{
public:
	vector<int> TrianglesOfVertices[maxn];
	Face face[maxn];
	int FaceNum,Fnum;
	float v[4];
	void setv(float a,float b,float c,float d)
	{
		v[0]=a;
		v[1]=b;
		v[2]=c;
		v[3]=d;
	}
	Vec3f direction(Face face)
	{
		Vec3f a=points[face.z]-points[face.x];
		Vec3f b=points[face.y]-points[face.x];
		Vec3f c=VtProduct(a,b);
		c.Normalize();
		return c;
	}
	Vec3f* points;
public:
	Faces(int num,Array<int,3>* list,Vec3f* p)
	{
		for(int i=0;i<maxn;++i) TrianglesOfVertices[i].clear();
		FaceNum=num;
		Fnum=0;
		for ( int i=0; i<num; ++i )
		{
			Face t;
			t.set(list[i]);
			face[Fnum++]=t;
		}
		for(int i=0;i<FaceNum;++i) addface(face[i],i);
		points=p;
	}
	void DelFaces(int x,int y)
	{
		for (int i=0; i<TrianglesOfVertices[x].size(); ++i)
			DelF(x,y,face[TrianglesOfVertices[x][i]]);
		for (int i=0; i<TrianglesOfVertices[y].size(); ++i)
			DelF(x,y,face[TrianglesOfVertices[y][i]]);
	}
	void addface(Face t,int i)
	{
		TrianglesOfVertices[t.x].push_back(i);
		TrianglesOfVertices[t.y].push_back(i);
		TrianglesOfVertices[t.z].push_back(i);
	}
	void DelF(int x,int y,Face &triangle)
	{
		if (!triangle.exist) return;
		bool hasx=triangle.has(x,x);
		bool hasy=triangle.has(y,x);
		if (hasx&&hasy)
		{
			triangle.exist = false;
			FaceNum--;
		}
	}
	void GetQ(int x,Matrix& ans)
	{
		ans.clear();
		Matrix A;
		for (int i=0; i<TrianglesOfVertices[x].size(); ++i )
		{
			int k=TrianglesOfVertices[x][i];
			if (face[k].exist)
			{
				A.clear();
				Vec3f N=direction(face[k]);
				setv(N[0],N[1],N[2],-product(N,points[face[k].x]));
				A.set(v);
				ans = ans+A;
			}
		}
	}
	void changeform(int& n,Array<int,3>*&list,int*w)
	{
		n=FaceNum;
		list = new Array<int,3>[FaceNum];
		int num=0;
		for (int i=0; i<Fnum; ++i)
			if (face[i].exist)
			{
				list[num][0]=w[face[i].x];
				list[num][1]=w[face[i].y];
				list[num][2]=w[face[i].z];
				++num;
			}
	}
	int size()
	{
		return FaceNum;
	}
};
struct Points
{
public:
	Vec3f points[maxn];
	Matrix Q[maxn];
	bool has[maxn];
	int Pnum;
	vector<int> links[maxn];
public:
	Points(int num,Vec3f* p)
	{
		Pnum=num;
		for(int i=0;i<num;++i) points[i]=p[i];
		for(int i=0;i<num;++i) Q[i].clear();
		for(int i=0;i<num;++i) has[i]=true;
	}
	bool find(vector<int>& X,int x)
	{
		for(int i=0;i<X.size();++i) if(X[i]==x) return true;
		return false;
	}
	void MergeVec(vector<int>& X,vector<int>& Y)
	{
		for(int i=0;i<Y.size();++i)
			if(!find(X,Y[i])) X.push_back(Y[i]);
	}
	void changelink(int v,int y,int x)
	{
		for(int i=0;i<links[v].size();++i)
			if(links[v][i]==y) links[v][i]=x;
	}
	void Copy(int x,int y,Faces* faces)
	{
		for (int i=0; i<links[y].size(); ++i)
			changelink(links[y][i],y,x);
		MergeVec(links[x],links[y]);
		MergeVec(faces->TrianglesOfVertices[x],faces->TrianglesOfVertices[y]);
	}
	void link(int x,int y)
	{
		links[x].push_back(y);
		links[y].push_back(x);
	}
	void changeform(int& n,Vec3f*& vecs,int*& w)
	{
		w=new int[Pnum];
		int num=0;
		for ( int i=0; i<Pnum; ++i )
			if (has[i]) w[i]=num++;
			else w[i]=-1;
		n=num;
		vecs = new Vec3f[num];
		num=0;
		for ( int i=0; i<Pnum; ++i )
			if (has[i]) vecs[num++] = points[i];
	}
	void updateQ(int x,Faces* faces)
	{
		faces->GetQ(x,Q[x]);
		for (int i=0; i<links[x].size(); ++i)
			if (has[links[x][i]]) faces->GetQ(links[x][i],Q[links[x][i]]);
	}
	void del(int y)
	{
		has[y]=false;
	}
	int size()
	{
		return Pnum;
	}
};
struct Method
{
public:
	virtual void GetNewPoint(const Matrix& qx,const Matrix& qy,const Vec3f& x,const Vec3f& y,Vec3f& point)
	{
		point=(x+y)*0.5;
	}
};
struct _Pair
{
	int birthday;
	float delta;
	int x,y;
	void set(int x0,int y0,int now)
	{
		x=x0;
		y=y0;
		birthday=now;
	}
	bool isclose() const
	{
		return(x==y);
	}
	friend bool operator<(const _Pair&a,const _Pair&b)
	{
		return(a.delta>b.delta);
	}
};
struct BirthDay
{
private:
	map<pair<int,int>,int> Date;
public:
	BirthDay()
	{
		Date.clear();
	}
	void setdate(int x,int y,int nowtime)
	{
		Date[make_pair(x,y)]=nowtime;
		Date[make_pair(y,x)]=nowtime;
	}
	int getdate(int x,int y)
	{
		return Date[make_pair(x,y)];
	}
};
struct _Pairs
{
private:
	priority_queue<_Pair> heap;
	BirthDay* birthday;
	void clearheap()
	{
		while(!heap.empty()) heap.pop();
	}
	bool* has;
	float v[4];
	bool legal(const _Pair& edge)
	{
		if ((!has[edge.x]||!has[edge.y])||edge.isclose()) return false;
		if (edge.birthday < birthday->getdate(edge.x,edge.y)) return false;
		return true;
	}
	void insert(_Pair pair,int nowtime)
	{
		birthday->setdate(pair.x,pair.y,nowtime);
		heap.push(pair);
	}
	void setv(float a,float b,float c,float d)
	{
		v[0]=a;
		v[1]=b;
		v[2]=c;
		v[3]=d;
	}
public:
	_Pairs(bool* h)
	{
		has=h;
		clearheap();
		birthday=new BirthDay;
	}
	_Pair getmin()
	{
		_Pair p;
		while (true)
		{
			p = heap.top();
			heap.pop();
			if(legal(p)) return p;
			if(heap.empty())return p;
		}
	}
	void _link(int x,int y,Points* vertices,Method* method,int now)
	{
		_Pair edge;
		Vec3f point;
		method->GetNewPoint(vertices->Q[x],vertices->Q[y],vertices->points[x],vertices->points[y],point);
		edge.delta=0;
		setv(point.x,point.y,point.z,1);
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
			{
				float Qij=vertices->Q[x].a[i][j]+vertices->Q[y].a[i][j];
				edge.delta=edge.delta+v[i]*Qij*v[j];
			}
		edge.set(x,y,now);
		insert(edge,now);
	}
	bool empty()
	{
		return heap.empty();
	}
};
struct Simplify
{
private:
	_Pairs* Heap_Pair;
	int now;
	Faces* faces;
	Points* vertices;
	Vec3f point;
	Method* method;
	Vec3f* m_pVertexList;
	int m_nVertices,m_nTriangles;
	Array<int,3>* m_pTriangleList;
	void updateLink(int x)
	{
		for(int i=0;i<vertices->links[x].size();++i)
			Heap_Pair->_link(x,vertices->links[x][i],vertices,method,now);
	}
	void Merge(int x,int y)
	{
		++now;
		method->GetNewPoint(vertices->Q[x],vertices->Q[y],vertices->points[x],vertices->points[y],point);
		vertices->points[x]=point;
		vertices->points[y]=point;
		faces->DelFaces(x,y);
		vertices->Copy(x,y,faces);
		vertices->updateQ(x,faces);
		updateLink(x);
		for(int i=0;i<vertices->links[x].size();++i)
			updateLink(vertices->links[x][i]);
		vertices->del(y);
	}
	void clear()
	{
		now=0;
	}
	void simplify_()
	{
		_Pair edge;
		edge=Heap_Pair->getmin();
		if (Heap_Pair->empty()) return;
		Merge(edge.x,edge.y);
	}
	void changeform()
	{
		int num=0;
		int* w;
		vertices->changeform(m_nVertices,m_pVertexList,w);
		faces->changeform(m_nTriangles,m_pTriangleList,w);
		delete [] w;
	}
	void connectface()
	{
		for(int i=0;i<faces->FaceNum;++i)
		{
			Face t=faces->face[i];
			vertices->link(t.x,t.y);
			vertices->link(t.x,t.z);
			vertices->link(t.y,t.z);
		}
	}
public:
	int times;
	Simplify(CSimpleObject* OBJ,Method* mth)
	{
		clear();
		vertices=new Points(OBJ->m_nVertices,OBJ->m_pVertexList);
		faces=new Faces(OBJ->m_nTriangles,OBJ->m_pTriangleList,vertices->points);
		method=mth;
		Heap_Pair=new _Pairs(vertices->has);
		connectface();
		for (int i=0; i<vertices->size(); ++i)
			faces->GetQ(i,vertices->Q[i]);
		for(int i=0;i<vertices->size();++i) updateLink(i);
	}
	void GetObj(CSimpleObject* obj)
	{
		obj->m_nTriangles=m_nTriangles;
		obj->m_nVertices=m_nVertices;
		obj->m_pTriangleList=m_pTriangleList;
		obj->m_pVertexList=m_pVertexList;
	}
	void simplify(int goal)
	{
		while(faces->size()>goal) simplify_();
		changeform();
	}
};
