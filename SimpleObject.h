#pragma once
#include "Vec3f.h"
#include <vector>
#include <stdio.h>
#include <vector>
#include <string>
#include "loadMTL.h"

namespace SimpleOBJ
{
    template <typename T, int N> class Array
    {
    public:
        enum {_len = N};
        typedef T t_Val;
    public:
        T& operator[] (int i)
        {
//            ASSERT(i>=0&&i<N);
            return _p[i];
        }
        const T& operator[] (int i) const
        {
            //ASSERT(i>=0&&i<N);
            return _p[i];
        }

        T _p[N];
    };

    class CSimpleObject
    {
    public:
        CSimpleObject(void);
        ~CSimpleObject(void);

    public:
        bool IsLoaded() { return m_pVertexList!=NULL;}

        void Destroy();
        bool LoadFromObj(const char* fn);
        bool SaveToObj(const char* fn);

        bool Parse(FILE* fp);
        bool CheckParse(int nVertices,std::vector<Array<int,3> > & vecTriangles);

        std::vector<std::string> mtl;
        std::vector<int> mtlb;

        int             m_nVertices;
        int             m_nTriangles;
        Vec3f*          m_pVertexList;
        Vec3f*          m_pNormalVertexList;
        Vec3f*          m_pTextureVertexList;
        Array<int,3>*   m_pTriangleList;
        Array<int,3>*   m_pTextureList;
        Array<int,3>*   m_pNormalList;
        char MTLFileName[16];
        std::vector<Material*> material;
    };

}
