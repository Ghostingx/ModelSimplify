#include"loadMTL.h"
int loadMTL(std::vector<Material*> &m,const char *file){
    FILE *fp = fopen(file,"r");
    if(NULL == fp){
        printf("ERROR:cannot open file %s\n",file);
        return -1;
    }
    int lineNumber =0;
    int nCount = 0;
    char buf[256];
    while(fscanf(fp,"%s",buf) != EOF){
        lineNumber++;
        if(buf[0] == '#')
            fgets(buf,sizeof(buf),fp);
        if(memcmp(buf,"newmtl",6) == 0){
            Material * temp = new Material;
            m.push_back(temp);
            nCount++;
            fscanf(fp,"%s",m[nCount-1]->name);
        }else if(0==strcmp(buf,"Ns")){
            if(fscanf(fp,"%d",&(m[nCount-1]->Ns))!=1)
                printf("line %d:file format error\n",lineNumber);
        }else if(0==strcmp(buf,"Ni")){
            if(fscanf(fp,"%f",&(m[nCount-1]->Ni))!=1)
                printf("line %d:file format error\n",lineNumber);
        }else if(0==strcmp(buf,"map_Kd")){
            fscanf(fp,"%s",m[nCount-1]->map_Kd);
        }else if(0==strcmp(buf,"Kd")){
            if(fscanf(fp,"%f %f %f",&(m[nCount-1]->Kd[0]),&(m[nCount-1]->Kd[1]),&(m[nCount-1]->Kd[2]))!=3)
                printf("line %d:file format error [Kd]\n",lineNumber);
        }else if(0==strcmp(buf,"d")){
            if(fscanf(fp,"%f",&(m[nCount-1]->d))!=1)
                printf("line %d:file format error [d]\n",lineNumber);
        }else if(0==strcmp(buf,"Tf")){
            if(fscanf(fp,"%d %d %d",&(m[nCount-1]->Tf[0]),&(m[nCount-1]->Tf[1]),&(m[nCount-1]->Tf[2]))!=3)
                printf("line %d:file format error [Tf]\n",lineNumber);

        }else if(0==strcmp(buf,"illum")){
            if(fscanf(fp,"%d",&(m[nCount-1]->illum))!=1)
                printf("line %d:file format error [illum]\n",lineNumber);

        }else if(0==strcmp(buf,"Ka")){
            if(fscanf(fp,"%f %f %f",&(m[nCount-1]->Ka[0]),&(m[nCount-1]->Ka[1]),&(m[nCount-1]->Ka[2]))!=3)
                printf("line %d:file format error [Ka]\n",lineNumber);

        }else if(0==strcmp(buf,"Ks")){
            if(fscanf(fp,"%f %f %f",&(m[nCount-1]->Ks[0]),&(m[nCount-1]->Ks[1]),&(m[nCount-1]->Ks[2]))!=3)
                printf("line %d:file format error [Ks]\n",lineNumber);

        }

    }
    return nCount;
};

