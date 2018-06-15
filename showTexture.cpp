//GL header files
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glut.h>

#include"SimpleObject.h"
#include <iostream>
#include<math.h>
#include<string.h>
#include"loadTexture.h"
using namespace SimpleOBJ;
using namespace std;

//GLUT variabls
static int GLUTwindow = 0;
static int GLUTwindow_height = 800;
static int GLUTwindow_width = 800;
static int GLUTmouse[2] = { 0, 0 };
static int GLUTbutton[3] = { 0, 0, 0 };
static int GLUTarrows[4] = { 0, 0, 0, 0 };
static int GLUTmodifiers = 0;




// Display variables
static int scaling = 0;
static int translating = 0;
static int rotating = 0;
static float scale = 1.0;
static float center[3] = { 0.0, 0.0, 0.0 };
static float rotation[3] = { 0.0, 0.0, 0.0 };
static float translation[3] = { 0.0, 0.0, -4.0 };

vector<GLuint> texture_ID;

CSimpleObject *obj = NULL;

Vec3f computeNormal(Vec3f v[3]){
    float normal[3];
    Vec3f vec;
    normal[0] = normal[1] = normal[2] = 0;
    Vec3f v1 = v[0] - v[1];
    Vec3f v2 = v[0] - v[2];
    normal[0] = v1.y * v2.z - v2.y * v1.z;
    normal[1] = v1.z * v2.x - v2.z * v1.x;
    normal[3] = v1.x * v2.y - v2.x * v1.y;
	/*Vec3f *v1 = &v[3];
	for (int i = 0; i < 3 ; i++) {
		Vec3f *v2 = &v[i];
		normal[0] += (v1->y - v2->y) * (v1->z + v2->z);
		normal[1] += (v1->z - v2->z) * (v1->x + v2->x);
		normal[2] += (v1->x - v2->x) * (v1->y + v2->y);
		v1 = v2;
	}*/

	// Normalize normal for face
	float squared_normal_length = 0.0;
	squared_normal_length += normal[0] * normal[0];
	squared_normal_length += normal[1] * normal[1];
	squared_normal_length += normal[2] * normal[2];
	float normal_length = sqrt(squared_normal_length);
	if (normal_length > 1.0E-6) {
		normal[0] /= normal_length;
		normal[1] /= normal_length;
		normal[2] /= normal_length;
	}
    vec.x = normal[0];
    vec.y = normal[1];
    vec.z = normal[2];
    return vec;
}

GLuint findMTL(char* s){
    for(int i=0;i<obj->material.size();i++){
        if(strcmp(obj->material[i]->name,s)==0)
            //return texture[i].texID;
            return texture_ID[i];
    }
    return 0;
}

void GLUTRedraw(void)
{
	// Setup viewing transformation
	glLoadIdentity();
	glScalef(scale, scale, scale);
	glTranslatef(translation[0], translation[1], 0.0);

	// Set projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)GLUTwindow_width / (GLfloat)GLUTwindow_height, 0.1, 100.0);

	// Set camera transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(translation[0], translation[1], translation[2]);
	glScalef(scale, scale, scale);
	glRotatef(rotation[0], 1.0, 0.0, 0.0);
	glRotatef(rotation[1], 0.0, 1.0, 0.0);
	glRotatef(rotation[2], 0.0, 0.0, 1.0);
	glTranslatef(-center[0], -center[1], -center[2]);

	// Clear window
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set lights
	static GLfloat light0_position[] = { 3.0, 4.0, 5.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	static GLfloat light1_position[] = { -3.0, -2.0, -3.0, 0.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

//    GLfloat position[]={30.0f,30.0f,30.0f,1.0f};
//    GLfloat ambient[]={0.0f,0.0f,0.0f,1.0f};
//    GLfloat diffuse[]={1.0f, 1.0f, 1.0f, 1.0f};
//    GLfloat specular[]={1.0f, 1.0f, 1.0f, 1.0f};
//
//    glLightfv(GL_LIGHT0,GL_POSITION,position);
//    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
//    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
//
    glEnable(GL_LIGHT0);
   // glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);

	// Set material
	//static GLfloat material[] = { 0.8, 0.8, 0.8, 1.0 };
	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, material);
    GLfloat wall_mat_ambient[] = { 0.5, 0.5, 0.5, 1.000000};
    GLfloat wall_mat_diffuse[] = { 0.9, 0.9, 0.9, 1.000000};
    GLfloat wall_mat_specular[] ={ 0.0, 0.0, 0.0, 1.000000};
    GLfloat wall_mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat wall_mat_shininess =  10;
    glMaterialfv(GL_FRONT, GL_AMBIENT, wall_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, wall_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, wall_mat_specular);
    //glMaterialfv(GL_FRONT, GL_EMISSION, wall_mat_emission);
    glMaterialf (GL_FRONT, GL_SHININESS, wall_mat_shininess);

//    cout<<texture_ID<<endl;
	// Draw faces
    if(obj->m_pTextureVertexList && obj->m_pNormalVertexList){
        int nCount = 0;
        char *name;
        string s = obj->mtl[nCount];
        name = (char*)s.data();
        GLuint t = findMTL(name);
        glBindTexture(GL_TEXTURE_2D,t);
        for (int i = 0; i < obj->m_nTriangles; i++) {
            if(i < obj->mtlb[nCount+1]){
            }
            else{
                nCount++;
                s = obj->mtl[nCount];
                name = (char*)s.data();
                t = findMTL(name);
                glBindTexture(GL_TEXTURE_2D,t);
            }
            glBegin(GL_POLYGON);
            for (int j = 0; j < 3; j++) {
                glTexCoord2f(obj->m_pTextureVertexList[obj->m_pTextureList[i][j]].x,obj->m_pTextureVertexList[obj->m_pTextureList[i][j]].y);
                glNormal3f(obj->m_pNormalVertexList[obj->m_pNormalList[i][j]].x,obj->m_pNormalVertexList[obj->m_pNormalList[i][j]].y,obj->m_pNormalVertexList[obj->m_pNormalList[i][j]].z);
                glVertex3f(obj->m_pVertexList[obj->m_pTriangleList[i][j]].x, obj->m_pVertexList[obj->m_pTriangleList[i][j]].y, obj->m_pVertexList[obj->m_pTriangleList[i][j]].z);
            }
            glEnd();

	    }
    }else if(obj->m_pTextureVertexList && !obj->m_pNormalVertexList){
        int nCount = 0;
        char *name;
        string s = obj->mtl[nCount];
        name = (char*)s.data();
        GLuint t = findMTL(name);
        glBindTexture(GL_TEXTURE_2D,t);
        for (int i = 0; i < obj->m_nTriangles; i++) {
            if(i < obj->mtlb[nCount+1]){
            }
            else{
                nCount++;
                s = obj->mtl[nCount];
                name = (char*)s.data();
                t = findMTL(name);
                glBindTexture(GL_TEXTURE_2D,t);
            }
            Vec3f v[3];
            for (int j = 0; j < 3; j++) {
                v[j] = obj->m_pVertexList[obj->m_pTriangleList[i][j]];
            }
            Vec3f nv = computeNormal(v);
            glNormal3f(nv.x,nv.y,nv.z);
            glBegin(GL_POLYGON);
            for (int j = 0; j < 3; j++) {
                glTexCoord2f(obj->m_pTextureVertexList[obj->m_pTextureList[i][j]].x,obj->m_pTextureVertexList[obj->m_pTextureList[i][j]].y);
                glVertex3f(obj->m_pVertexList[obj->m_pTriangleList[i][j]].x, obj->m_pVertexList[obj->m_pTriangleList[i][j]].y, obj->m_pVertexList[obj->m_pTriangleList[i][j]].z);
            }
            glEnd();
        }
    }
	// Swap buffers
	glutSwapBuffers();
}


void GLUTStop(void)
{
	// Destroy window
	glutDestroyWindow(GLUTwindow);

	// Exit
	exit(0);
}



void GLUTResize(int w, int h)
{
	// Resize window
	glViewport(0, 0, w, h);

	// Remember window size
	GLUTwindow_width = w;
	GLUTwindow_height = h;

	// Redraw
	glutPostRedisplay();
}



void GLUTMotion(int x, int y)
{
	// Invert y coordinate
	y = GLUTwindow_height - y;

	// Process mouse motion event
	if (rotating) {
		// Rotate model
		rotation[0] += -0.5 * (y - GLUTmouse[1]);
		rotation[2] += 0.5 * (x - GLUTmouse[0]);
	}
	else if (scaling) {
		// Scale window
		scale *= exp(2.0 * (float)(x - GLUTmouse[0]) / (float)GLUTwindow_width);
	}
	else if (translating) {
		// Translate window
		translation[0] += 2.0 * (float)(x - GLUTmouse[0]) / (float)GLUTwindow_width;
		translation[1] += 2.0 * (float)(y - GLUTmouse[1]) / (float)GLUTwindow_height;
	}

	// Remember mouse position
	GLUTmouse[0] = x;
	GLUTmouse[1] = y;
}



void GLUTMouse(int button, int state, int x, int y)
{
	// Invert y coordinate
	y = GLUTwindow_height - y;

	// Process mouse button event
	rotating = (button == GLUT_LEFT_BUTTON);
	scaling = (button == GLUT_MIDDLE_BUTTON);
	translating = (button == GLUT_RIGHT_BUTTON);
	if (rotating || scaling || translating) glutIdleFunc(GLUTRedraw);
	else glutIdleFunc(0);

	// Remember button state
	int b = (button == GLUT_LEFT_BUTTON) ? 0 : ((button == GLUT_MIDDLE_BUTTON) ? 1 : 2);
	GLUTbutton[b] = (state == GLUT_DOWN) ? 1 : 0;

	// Remember modifiers
	GLUTmodifiers = glutGetModifiers();

	// Remember mouse position
	GLUTmouse[0] = x;
	GLUTmouse[1] = y;
}



void GLUTSpecial(int key, int x, int y)
{
	// Invert y coordinate
	y = GLUTwindow_height - y;

	// Process keyboard button event

	// Remember mouse position
	GLUTmouse[0] = x;
	GLUTmouse[1] = y;

	// Remember modifiers
	GLUTmodifiers = glutGetModifiers();

	// Redraw
	glutPostRedisplay();
}



void GLUTKeyboard(unsigned char key, int x, int y)
{
	// Process keyboard button event
	switch (key) {
	case 'Q':
	case 'q':
		GLUTStop();
		break;

	case 27: // ESCAPE
		GLUTStop();
		break;
	}

	// Remember mouse position
	GLUTmouse[0] = x;
	GLUTmouse[1] = GLUTwindow_height - y;

	// Remember modifiers
	GLUTmodifiers = glutGetModifiers();
}



void GLUTInit(int *argc, char **argv)
{
	// Open window
	glutInit(argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(GLUTwindow_width, GLUTwindow_height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // | GLUT_STENCIL
	GLUTwindow = glutCreateWindow("3Dviewer");

	// Initialize GLUT callback functions
	glutReshapeFunc(GLUTResize);
    glutDisplayFunc(GLUTRedraw);
	glutKeyboardFunc(GLUTKeyboard);
	glutSpecialFunc(GLUTSpecial);
	glutMouseFunc(GLUTMouse);
	glutMotionFunc(GLUTMotion);
	glutIdleFunc(0);

	// Initialize lights
	static GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	static GLfloat light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glEnable(GL_LIGHT0);
	static GLfloat light1_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
    GLuint id = 0;
    for (int i=0;i < obj->material.size();i++){

        //if(!LoadTGATexture(&texture[i],obj->material[i]->map_Kd))
        if((id=loadTexture(obj->material[i]->map_Kd)) > 0)
            texture_ID.push_back(id);
        else
            texture_ID.push_back(0);

    }

	// Initialize graphics modes
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
}



void GLUTMainLoop(void)
{
	// Compute bounding box
	float bbox[2][3] = { { 1.0E30F, 1.0E30F, 1.0E30F },{ -1.0E30F, -1.0E30F, -1.0E30F } };
	for (int i = 0; i < obj->m_nVertices; i++) {
		if (obj->m_pVertexList[i].x < bbox[0][0]) bbox[0][0] = obj->m_pVertexList[i].x;
		else if (obj->m_pVertexList[i].x > bbox[1][0]) bbox[1][0] = obj->m_pVertexList[i].x;
		if (obj->m_pVertexList[i].y < bbox[0][1]) bbox[0][1] = obj->m_pVertexList[i].y;
		else if (obj->m_pVertexList[i].y > bbox[1][1]) bbox[1][1] = obj->m_pVertexList[i].y;
		if (obj->m_pVertexList[i].z < bbox[0][2]) bbox[0][2] = obj->m_pVertexList[i].z;
		else if (obj->m_pVertexList[i].z > bbox[1][2]) bbox[1][2] = obj->m_pVertexList[i].z;
	}

	// Setup initial viewing scale
	float dx = bbox[1][0] - bbox[0][0];
	float dy = bbox[1][1] - bbox[0][1];
	float dz = bbox[1][2] - bbox[0][2];
	scale = 2.0 / sqrt(dx*dx + dy * dy + dz * dz);

	// Setup initial viewing center
	center[0] = 0.5 * (bbox[1][0] + bbox[0][0]);
	center[1] = 0.5 * (bbox[1][1] + bbox[0][1]);
	center[2] = 0.5 * (bbox[1][2] + bbox[0][2]);

	// Run main loop -- never returns
	glutMainLoop();
}

int ParseArgs(int argc, char **argv,char * &filename)
{
	// Innocent until proven guilty
	int print_usage = 0;

	// Parse arguments
	argc--; argv++;
	while (argc > 0) {
		if ((*argv)[0] == '-') {
			if (!strcmp(*argv, "-help")) { print_usage = 1; }
            else if(!strcmp(*argv, "-m")){
                argv++;argc--;
			    if(!strcmp(*argv, "sim")){}
			    else if(!strcmp(*argv, "sub")){}
                else{ fprintf(stderr, "Invalid program argument: %s\n", *argv); exit(1); }
                argv++; argc--;
            }
            else{ fprintf(stderr, "Invalid program argument: %s\n", *argv); exit(1); }
		}
		else {
			filename = *argv;
			argv++; argc--;
		}
	}

	// Check filename
	if (!filename || print_usage) {
		printf("Usage: %s -t texture_file <filename>\n",argv[0]);
		return 0;
	}

	// Return OK status
	return 0;
}


int main(int argc,char **argv){

    char* filename;
    ParseArgs(argc,argv,filename);
    obj = new CSimpleObject;
    obj->LoadFromObj(filename);
    GLUTInit(&argc, argv);
//    texture_ID = load_texture(texture);
    GLUTMainLoop();

}
