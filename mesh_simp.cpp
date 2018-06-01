#include "Simplify.h"
#include "SimpleObject.h"
#include <iostream>
using namespace SimpleOBJ;
using namespace std;
int times;
int main(int argc, char* argv[])
{
	times=0;
	char* input=argv[1];
	char* output=argv[2];
	double rate=atof(argv[3]);
	CSimpleObject* OBJ=new CSimpleObject;
	OBJ->LoadFromObj(input);
	Method* method=new Method;
	Simplify* simplify=new Simplify(OBJ,method);
	int now=OBJ->m_nTriangles;
	int goal=now*rate;
	int num=0;
	simplify->times=0;
	simplify->simplify(goal);
	CSimpleObject* obj=new CSimpleObject;
	simplify->GetObj(obj);
	obj->SaveToObj(output);
	return 0;
}
