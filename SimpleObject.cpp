#include "SimpleObject.h"
#include <vector>
#include <string.h>
using namespace std;

namespace SimpleOBJ
{
    CSimpleObject::CSimpleObject(void)
    {
        m_nVertices = -1;
        m_nTriangles = -1;
        m_pTriangleList = NULL;
        m_pVertexList = NULL;
        m_pTextureList = NULL;
        m_pNormalList = NULL;
    }

    CSimpleObject::~CSimpleObject(void)
    {
        Destroy();
    }

    void CSimpleObject::Destroy()
    {
        if(m_pTriangleList)
            delete []m_pTriangleList;
        if(m_pVertexList)
            delete []m_pVertexList;
        if(m_pTextureList)
            delete []m_pTextureList;
        if(m_pNormalList)
            delete []m_pNormalList;
        if(m_pNormalVertexList)
            delete []m_pNormalVertexList;
        if(m_pTextureVertexList)
            delete []m_pTextureVertexList;
        m_nVertices = -1;
        m_nTriangles = -1;
        m_pVertexList = NULL;
        m_pNormalVertexList = NULL;
        m_pTextureVertexList = NULL;
        m_pTriangleList = NULL;
        m_pTextureList = NULL;
        m_pNormalList = NULL;
    }

    bool CSimpleObject::LoadFromObj(const char* fn)
    {
        FILE* fp = fopen(fn,"r");
        if(fp==NULL)
        {
            printf("Error: Loading %s failed.\n",fn);
            return false;
        }
        else
        {
            if(Parse(fp))
            {
                printf("Loading from %s successfully.\n",fn);
                printf("Vertex Number = %d\n",m_nVertices);
                printf("Triangle Number = %d\n",m_nTriangles);
                fclose(fp);
                return true;
            }
            else
            {
                fclose(fp);
                return false;
            }
        }
    }

    bool CSimpleObject::Parse(FILE* fp)
    {

        char buf[256];
        int nVertices,nTriangles;
        std::vector<Vec3f>          vecVertices;
        std::vector<Vec3f>          norVertices;
        std::vector<Vec3f>          texVertices;
        std::vector<Array<int,3> >  vecTriangles;
        std::vector<Array<int,3> >  texTriangles;
        std::vector<Array<int,3> >  norTriangles;

        nVertices = 0;
        nTriangles = 0;
        vecVertices.clear();
        norVertices.clear();
        texVertices.clear();
        vecTriangles.clear();
        texTriangles.clear();
        norTriangles.clear();
        int lineNumber = 0;

        while(fscanf(fp, "%s", buf) != EOF)
        {
            lineNumber ++;
            switch(buf[0])
            {
            case '#':				/* comment */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), fp);
                break;
            case 'v':				/* v, vn, vt */
                switch(buf[1])
                {
                case '\0':			    /* vertex */
                    {
                        Vec3f vP;
                        if(fscanf(fp, "%f %f %f",
                            &vP.x,
                            &vP.y,
                            &vP.z)==3)
                        {
                            nVertices++;
                            vecVertices.push_back(vP);
                        }
                        else
                        {
                            printf("Error: Wrong Number of Values(Should be 3). at Line %d\n",lineNumber);
                            return false;
                        }
                    }
                    break;
                case 'n':			    /* vertex */
                    {
                        Vec3f vP;
                        if(fscanf(fp, "%f %f %f",
                            &vP.x,
                            &vP.y,
                            &vP.z)==3)
                        {
                            norVertices.push_back(vP);
                        }
                        else
                        {
                            printf("Error: Wrong Number of Values(Should be 3). at Line %d\n",lineNumber);
                            return false;
                        }
                    }
                    break;
                case 't':			    /* vertex */
                    {
                        Vec3f vP;
                        if(fscanf(fp, "%f %f",
                            &vP.x,
                            &vP.y)==2)
                        {
                            vP.z = 0;
                            texVertices.push_back(vP);
                        }
                        else
                        {
                            printf("Error: Wrong Number of Values(Should be 3). at Line %d\n",lineNumber);
                            return false;
                        }
                    }
                    break;
                default:
                    /* eat up rest of line */
	                fgets(buf, sizeof(buf), fp);
                    break;
                }
                break;

            case 'f':				/* face */
                {
                    int v,n,t;
                    Array<int,3> vIndices;
                    Array<int,3> nIndices;
                    Array<int,3> tIndices;
                    if(fscanf(fp, "%s", buf)!=1)
                    {
                        printf("Error: Wrong Face at Line %d\n",lineNumber);
                        return false;
                    }

                    /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                    if (strstr(buf, "//"))
                    {
                        /* v//n */
                        if( sscanf(buf, "%d//%d", &vIndices[0],&nIndices[0]) ==2  &&
                            fscanf(fp, "%d//%d", &vIndices[1], &nIndices[1]) ==2  &&
                            fscanf(fp, "%d//%d", &vIndices[2], &nIndices[2]) ==2)
                        {
                            nTriangles++;
                            vecTriangles.push_back(vIndices);
                            norTriangles.push_back(nIndices);
                        }
                        else
                        {
                            printf("Error: Wrong Face at Line %d\n",lineNumber);
                            return false;
                        }

                    }
                    else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3)
                    {
                        /* v/t/n */
                        vIndices[0] = v;
                        tIndices[0] = t;
                        nIndices[0] = n;
                        if( fscanf(fp, "%d/%d/%d", &vIndices[1], &tIndices[1], &nIndices[1]) ==3 &&
                            fscanf(fp, "%d/%d/%d", &vIndices[2], &tIndices[1], &nIndices[2]) ==3 )
                        {
                            nTriangles++;
                            vecTriangles.push_back(vIndices);
                            texTriangles.push_back(tIndices);
                            norTriangles.push_back(nIndices);
                        }
                        else
                        {
                            printf("Error: Wrong Face at Line %d\n",lineNumber);
                            return false;
                        }
                    }
                    else if (sscanf(buf, "%d/%d", &v, &t) == 2)
                    {
                        /* v/t */
                        vIndices[0] = v;
                        tIndices[0] = t;
                        if( fscanf(fp, "%d/%d", &vIndices[1], &tIndices[1]) ==2 &&
                            fscanf(fp, "%d/%d", &vIndices[2], &tIndices[2]) ==2 )
                        {
                            nTriangles++;
                            vecTriangles.push_back(vIndices);
                            texTriangles.push_back(tIndices);
                        }
                        else
                        {
                            printf("Error: Wrong Face at Line %d\n",lineNumber);
                            return false;
                        }
                    }
                    else
                    {
                        /* v */
                        if( sscanf(buf, "%d", &vIndices[0]) ==1 &&
                            fscanf(fp, "%d", &vIndices[1])  ==1 &&
                            fscanf(fp, "%d", &vIndices[2])  ==1 )
                        {
                            nTriangles++;
                            vecTriangles.push_back(vIndices);
                        }
                        else
                        {
                            printf("Error: Wrong Face at Line %d\n",lineNumber);
                            return false;
                        }
                    }

                }

                break;

            default:
                fgets(buf, sizeof(buf), fp);
                break;
            }
        }

        if(CheckParse(vecVertices.size(),vecTriangles))
        {
            Destroy();

            m_nVertices = vecVertices.size();
            m_nTriangles = vecTriangles.size();
            m_pVertexList = new Vec3f[m_nVertices];

            for(int i=0;i<m_nVertices;i++)
                m_pVertexList[i] = vecVertices[i];
            if(norVertices.size() > 0){
                m_pNormalVertexList = new Vec3f[norVertices.size()];
                for(int i=0;i<norVertices.size();i++)
                    m_pNormalVertexList[i] = norVertices[i];
            }
            if(texVertices.size() > 0){
                m_pTextureVertexList = new Vec3f[texVertices.size()];
                for(int i=0;i<texVertices.size();i++)
                    m_pTextureVertexList[i] = texVertices[i];
            }
            if(texTriangles.size() > 0 && norTriangles.size() > 0){
                m_pTriangleList = new Array<int,3> [m_nTriangles];
                m_pTextureList = new Array<int,3> [m_nTriangles];
                m_pNormalList = new Array<int,3> [m_nTriangles];

                for(int i=0;i<m_nTriangles;i++)
                {
                    for(int j=0;j<3;j++){
                        m_pTriangleList[i][j] = vecTriangles[i][j] - 1;
                        m_pTextureList[i][j] = texTriangles[i][j] - 1;
                        m_pNormalList[i][j] = norTriangles[i][j] - 1;
                    }
                }
            }
            else if(texTriangles.size() > 0 && norTriangles.size() == 0){
                m_pTriangleList = new Array<int,3> [m_nTriangles];
                m_pTextureList = new Array<int,3> [m_nTriangles];

                for(int i=0;i<m_nTriangles;i++)
                {
                    for(int j=0;j<3;j++){
                        m_pTriangleList[i][j] = vecTriangles[i][j] - 1;
                        m_pTextureList[i][j] = texTriangles[i][j] - 1;
                    }
                }
            }
            else if(texTriangles.size() == 0 && norTriangles.size() > 0){
                m_pTriangleList = new Array<int,3> [m_nTriangles];
                m_pNormalList = new Array<int,3> [m_nTriangles];

                for(int i=0;i<m_nTriangles;i++)
                {
                    for(int j=0;j<3;j++){
                        m_pTriangleList[i][j] = vecTriangles[i][j] - 1;
                        m_pNormalList[i][j]   = norTriangles[i][j] -1;
                    }
                }
            }
            else if(texTriangles.size() == 0 && norTriangles.size() == 0){
                m_pTriangleList = new Array<int,3> [m_nTriangles];

                for(int i=0;i<m_nTriangles;i++)
                {
                    for(int j=0;j<3;j++)
                        m_pTriangleList[i][j] = vecTriangles[i][j] - 1;
                }
            }

            return true;
        }
        else
            return false;
    }

    bool CSimpleObject::CheckParse(int nVertices,std::vector<Array<int,3> > & vecTriangles)
    {
        for(int i=0;i<vecTriangles.size();i++)
        {
            Array<int,3> & vIndices = vecTriangles[i];
            for(int j=0;j<vIndices._len;j++)
                if(vIndices[j]<=0||vIndices[j]>nVertices)
                {
                    printf("Error: The vertex index of Face %d has exceeded vertex number %d\n",i,nVertices);
                    return false;
                }
        }

        return true;
    }

    bool CSimpleObject::SaveToObj(const char* fn)
    {
        if(!IsLoaded())
        {
            printf("Error: Object is not initialized.\n",fn);
            return false;
        }

        FILE* fp = fopen(fn,"w");
        if(fp==NULL)
        {
            printf("Error: Failed opening %s to write\n",fn);
            return false;
        }

        fprintf(fp,"# %d vertices\n",m_nVertices);
        for(int i=0;i<m_nVertices;i++)
            fprintf(fp,"v %f %f %f\n",  m_pVertexList[i].x,
                                        m_pVertexList[i].y,
                                        m_pVertexList[i].z);

        fprintf(fp,"# %d triangles\n",m_nTriangles);
        if(m_pTextureList && m_pNormalList){
            for(int i=0;i<m_nTriangles;i++)
                fprintf(fp,"f %d/%d/%d %d/%d/%d %d/%d/%d\n",  m_pTriangleList[i][0] + 1,m_pTextureList[i][0] + 1,m_pNormalList[i][0] + 1,
                                            m_pTriangleList[i][1] + 1,m_pTextureList[i][1] + 1,m_pNormalList[i][1] + 1,
                                            m_pTriangleList[i][2] + 1,m_pTextureList[i][2] + 1,m_pNormalList[i][2]) + 1;
        }
        else if(m_pTextureList && !m_pNormalList){
            for(int i=0;i<m_nTriangles;i++)
                fprintf(fp,"f %d/%d %d/%d %d/%d\n",  m_pTriangleList[i][0] + 1,m_pTextureList[i][0] + 1,
                                            m_pTriangleList[i][1] + 1,m_pTextureList[i][1] + 1,
                                            m_pTriangleList[i][2] + 1,m_pTextureList[i][2] + 1);
        }
        else if(!m_pTextureList && m_pNormalList){
            for(int i=0;i<m_nTriangles;i++)
                fprintf(fp,"f %d//%d %d//%d %d//%d\n",  m_pTriangleList[i][0] + 1,m_pNormalList[i][0] + 1,
                                            m_pTriangleList[i][1] + 1,m_pNormalList[i][1] + 1,
                                            m_pTriangleList[i][2] + 1,m_pNormalList[i][2] + 1);
        }
        else if(!m_pTextureList && !m_pNormalList){
            for(int i=0;i<m_nTriangles;i++)
                fprintf(fp,"f %d %d %d\n",  m_pTriangleList[i][0] + 1,
                                            m_pTriangleList[i][1] + 1,
                                            m_pTriangleList[i][2] + 1);
        }

        fclose(fp);

        printf("Writing to %s successfully\n",fn);
        return true;

    }
}


