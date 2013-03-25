#include "output.h"

    double loopTime;
    double beginTime = 0;
    int fps = 0;

    void outPut::setScene()
{
    static bool autoZfocus(true);
    static bool init = true;
    if (init)
    {
        glfwSetTime(0);
        TwAddVarRW(b_scene, "Auto Z focus", TW_TYPE_BOOLCPP, &autoZfocus, "");
        TwAddVarRO(b_scene, "FPS", TW_TYPE_INT8, &fps, "");
        init = false;
        glfwSetTime(0);
    }
    static float mat[4*4];
    static coords3d focus3d;

    beginTime = glfwGetTime();

    focus3d = _scene3d.focus*(coords3d::retournercoords3d(_reg.PIXEL_SIZE, _reg.PIXEL_SIZE, 1));
    if(autoZfocus)
    {
        focus3d.z = getVertex((int)_scene3d.focus.x, (int)_scene3d.focus.y).z;
    }

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(_scene3d.zoom,_reg.WIDTH/_reg.HEIGHT,1,1000);


    gluLookAt(focus3d.x, focus3d.y, _scene3d.distance, focus3d.x, focus3d.y, focus3d.z, 0, 1, 0);

    if(!_scene3d.lightPos[3])
    {
        _scene3d.lightPos[0] = -_scene3d.lightDir[0];
        _scene3d.lightPos[1] = -_scene3d.lightDir[1];
        _scene3d.lightPos[2] = -_scene3d.lightDir[2];
    }
    else
    {
        _scene3d.lightPos[0] = _scene3d.lightDir[0];
        _scene3d.lightPos[1] = _scene3d.lightDir[1];
        _scene3d.lightPos[2] = _scene3d.lightDir[2];
    }
    glLightfv(GL_LIGHT0,GL_POSITION,_scene3d.lightPos);


    glTranslatef(focus3d.x, focus3d.y, focus3d.z);
    ConvertQuaternionToMatrix(_scene3d.orientation, mat);
    glMultMatrixf(mat);
    glTranslatef(-focus3d.x, -focus3d.y, -focus3d.z);

}
void outPut::drawScene()
{
    drawLight();
   // genList();
    glCallList(_dispListMap);
    TwDraw();
}

void outPut::display()
{
    glfwSwapBuffers();

    loopTime = glfwGetTime() - beginTime;
    fps = 1/loopTime;
    if(fps >  _reg.MAX_FPS)
        glfwSleep(loopTime-1/_reg.MAX_FPS );
}

void outPut::drawResult(vector<Node>* list)
{
    static bool init(true);
    static unsigned int occurence(0);
    occurence++;
    static vector<Node>* lastList = NULL;
    if(init)
    {
        lastList = list;
        if(list == NULL)
            return;
    }
    else if(list == NULL)
    {
        list = lastList;
        init = true;
    }
#define decalZ 1
#define lineWidth 2
    if(init)
    {
        _results.verticesA = new float[list->size()*P_SIZE];
        _results.colorsA = new float[list->size()*C_SIZE];

        coords3d vertex(0,0,0), normal(0,0,0);

        for(unsigned int i = 0; i < list->size(); i++)
        {
            vertex = getVertex((*list)[i].x,(*list)[i].y);
            normal = _scene3d.normalMap[(*list)[i].x][(*list)[i].y];
            _results.verticesA[i*P_SIZE] = vertex.x;
            _results.verticesA[i*P_SIZE+1] = vertex.y;
            _results.verticesA[i*P_SIZE+2] = vertex.z;

            /*cout << "\n\n\n x :" << _results.verticesA[i*P_SIZE] << "\n y :" << _results.verticesA[i*P_SIZE+1] << "\n z :" << _results.verticesA[i*P_SIZE+2] ;
            cout << "\n r :" << _results.colorsA[i*C_SIZE] << "\n v :" << _results.colorsA[i*C_SIZE+1] << "\n b :" << _results.colorsA[i*C_SIZE+2] ;*/
        }
        init = false;
    }
#define ESPACE 50
    for(unsigned int i = 0; i < list->size(); i++)
        {
            _results.colorsA[i*C_SIZE] = 0;
            _results.colorsA[i*C_SIZE+1] = (occurence+i)%(ESPACE)*(255/ESPACE);
            _results.colorsA[i*C_SIZE+2] = 0;
        }


    if(!_reg.ZTEST_RESULT)
        glDisable(GL_DEPTH_TEST);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(P_SIZE, GL_FLOAT,(P_SIZE)*sizeof(_results.verticesA[0]), _results.verticesA);
    glColorPointer(C_SIZE, GL_FLOAT,(C_SIZE)*sizeof(_results.colorsA[0]), _results.colorsA);

    glDisable(GL_LIGHTING);
    glDrawArrays(GL_LINE_STRIP, 0, list->size());
    glEnable(GL_LIGHTING);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    if(!_reg.ZTEST_RESULT)
        glEnable(GL_DEPTH_TEST);
}

void outPut::drawLight()
{
#define DL_SCALE 10
    glDisable(GL_LIGHTING);
    glPointSize(5);
    glColor3ub(255,255,0);
    if(_scene3d.lightPos[3])
    {
        glBegin(GL_POINTS);
        glVertex3d(_scene3d.lightPos[0],_scene3d.lightPos[1],_scene3d.lightPos[2]);
        glEnd();
    }
    else
    {
        glBegin(GL_LINES);
        glVertex3d(_scene3d.lightPos[0]*DL_SCALE,_scene3d.lightPos[1]*DL_SCALE,_scene3d.lightPos[2]*DL_SCALE);
        glVertex3d(0,0,0);
        glEnd();
    }

    glEnable(GL_LIGHTING);
}
