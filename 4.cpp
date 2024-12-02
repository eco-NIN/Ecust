#include<stdio.h>
#include <vector>
using namespace std;
#include <gl/glut.h>
#include "graphicType.h"
// ȫ�ֱ���
vector<point> points;     // �洢���ߵĶ˵�
//vector <point> pointVertex;   //���ƶ���ζ�������
bool isEnd=false;			  //��ʶ���ƶ�����Ƿ�ȷ��

const GLint ControlN=4;       //��4�����Ƶ㶨������bezier���߶�


color redColor(1,0,0),blackColor(0,0,0);//��ɫ����ɫ


bool start=false;//���ڿ�ʼ�˵��Ľ��������жϣ����startΪtrue�������ֱ�ӽ��й��ܲ���
bool line=false;
bool rectangle=false;

int x1=0, x2=0, y1=0, y2=0; // �������м�¼�Ĳü�����λ��
int iMousePointNum= 0;// ��굥��ȷ���ü����ڵ����Ŀ
int xKey1=0,xKey2=0,yKey1=0,yKey2=0;// ���̲����м�¼�����λ��
int iKeyPointNum = 0;// ����'p'ȷ��ֱ�߶ζ˵����Ŀ
int winWidth = 400, winHeight = 300;
int count=0;

int selection;

//��������
void drawPolygonalLine(vector<point> &points,color &c ){//���ݵ�������������points����������
	glColor3f(c.r, c.g, c.b);
	glBegin (GL_LINE_STRIP);
		for(int i=0;i<points.size();i++){
			glVertex2i(points[i].x, points[i].y);
		}
	glEnd();
}
//��������
void drawPolygonalLine(vector<point> &points ,point &p,color &c){//���ݵ�������������points����������
	glColor3f(c.r, c.g, c.b);
	glBegin (GL_LINE_STRIP);
		for(int i=0;i<points.size();i++){
			glVertex2i(points[i].x, points[i].y);
		}
		glVertex2i(p.x, p.y);
	glEnd();
}
//������Ƶ���n<4������һ��n-1��Bezier���߶Σ��������һ������Bezier����
void drawBezierCurve(vector<Point> &points,color &c){
	GLfloat ControlP[4][3];
	int iPointNum=points.size();
	int i;
	if(iPointNum<4){//���Ƶ���<4������һ��n-1��Bezier���߶�
		for(i=0;i<iPointNum;i++){
			ControlP[i][0]=points[i].x;
			ControlP[i][1]=points[i].y;
			ControlP[i][2]=0.0;
		}
	}
	else{
		for(i=0;i<4;i++){//���Ƶ���>=4������һ������Bezier���߶�
			ControlP[i][0]=points[i].x;
			ControlP[i][1]=points[i].y;
			ControlP[i][2]=0.0;
		}
	}
	glColor3f(c.r, c.g, c.b);
	glPointSize(2);
	if(iPointNum>4)
		glMap1f(GL_MAP1_VERTEX_3,0.0,1.0,3,4,*ControlP);//����һάȡֵ��
	else
		glMap1f(GL_MAP1_VERTEX_3,0.0,1.0,3,iPointNum,*ControlP);//����һάȡֵ��
	glEnable(GL_MAP1_VERTEX_3);
	glMapGrid1f(100,0.0,1.0);//���ɾ��ȷֲ���һά�������ֵ
	glEvalMesh1(GL_LINE, 0, 100);//����Bezier����
}
void drawBezierCurve_deCasteljau(vector<Point> &points,color &c){
    int iPointNum = points.size();
    glColor3f(c.r, c.g, c.b);
    glPointSize(2);

    // ʹ�� de Casteljau �㷨���㱴���������ϵĵ�
    vector<Point> tempPoints = points;
    vector<Point> curvePoints;

    // �ݹ���㲢��ֵ�������ߵ�
    for (int i = 0; i <= 100; ++i) {
        float t = i / 100.0f;

        // ������Ƶ�
        tempPoints = points;

        // �𲽼��㱴�������ߵ�
        while (tempPoints.size() > 1) {
            vector<Point> nextPoints;
            for (int j = 0; j < tempPoints.size() - 1; ++j) {
                Point p;
                p.x = (1 - t) * tempPoints[j].x + t * tempPoints[j + 1].x;
                p.y = (1 - t) * tempPoints[j].y + t * tempPoints[j + 1].y;
                nextPoints.push_back(p);
            }
            tempPoints = nextPoints; // ��������
        }

        // ������õ������ߵ���뵽���ߵ㼯����
        curvePoints.push_back(tempPoints[0]);
    }

    // ��������
    glBegin(GL_LINE_STRIP);
    for (const auto &pt : curvePoints) {
        glVertex3f(pt.x, pt.y, 0.0f);
    }
    glEnd();
}
//ͨ����m��3+1�����Ƶ㣬����m��n-1��ezier���߶�
void drawBezierCurve(vector<Point> &points, int n, color &c) {
    int m = (points.size() - 1) / 3; // �������߶���
    vector<Point> adjustedPoints = points;

    // �������Ƶ���ʵ�� G1 ������
    for (int i = 1; i < m; i++) {
        Point &P3 = adjustedPoints[i * 3];     // ��ǰ�ε� P3�����ӵ㣩
        Point &P2 = adjustedPoints[i * 3 - 1]; // ��ǰ�ε� P2
        Point &Q1 = adjustedPoints[i * 3 + 1]; // ��һ�ε� Q1

        // ���� G1 �������������� Q1
        Q1.x = P3.x + (P3.x - P2.x);
        Q1.y = P3.y + (P3.y - P2.y);
    }

    // ����ÿ�����α���������
    for (int i = 0; i < m; i++) {
        vector<Point> segmentPoints = {
            adjustedPoints[i * 3],
            adjustedPoints[i * 3 + 1],
            adjustedPoints[i * 3 + 2],
            adjustedPoints[i * 3 + 3]
        };

        drawBezierCurve(segmentPoints, c);
    }
}


// ����B����������ֵ
float calculateBSplineBasis(int i, int k, float u, const vector<float>& knots) {
    if (k == 1) {
        return (u >= knots[i] && u < knots[i + 1]) ? 1.0f : 0.0f;
    }
    float left = 0.0f, right = 0.0f;
    if (knots[i + k - 1] != knots[i]) {
        left = (u - knots[i]) / (knots[i + k - 1] - knots[i]) * calculateBSplineBasis(i, k - 1, u, knots);
    }
    if (knots[i + k] != knots[i + 1]) {
        right = (knots[i + k] - u) / (knots[i + k] - knots[i + 1]) * calculateBSplineBasis(i + 1, k - 1, u, knots);
    }
    return left + right;
}
void drawBSplineCurve_Definition(vector<Point>& points, color& c) {
    if (points.size() < 4) {
        printf("Error: At least 4 points are required for B-Spline.\n");
        return;
    }

    int n = points.size() - 1; // ����
    int k = 4;                // �������̶�Ϊ3��B������
    vector<float> knots(n + k + 1); // ���Ƚڵ�����

    // ���ɾ��Ƚڵ�����
    for (int i = 0; i < knots.size(); i++) {
        knots[i] = i;
    }

    // ����B��������
    glColor3f(c.r, c.g, c.b);
    glBegin(GL_LINE_STRIP);
    for (float u = knots[k - 1]; u <= knots[n + 1]; u += 0.01) {
        float x = 0, y = 0;
        for (int i = 0; i <= n; i++) {
            float B = calculateBSplineBasis(i, k, u, knots);
            x += B * points[i].x;
            y += B * points[i].y;
        }
        glVertex2f(x, y);
    }
    glEnd();
}

// de Boor �㷨
Point deBoor(int n, int k, float u, const vector<Point>& points, const vector<float>& knots) {
    vector<Point> d(points); // ��������
    for (int r = 1; r < k; r++) {
        for (int i = n; i >= r; i--) {
            float alpha = (u - knots[i]) / (knots[i + k - r] - knots[i]);
            d[i].x = (1 - alpha) * d[i - 1].x + alpha * d[i].x;
            d[i].y = (1 - alpha) * d[i - 1].y + alpha * d[i].y;
        }
    }
    return d[n];
}
void drawBSplineCurve_deBoor(vector<Point>& points, color& c) {
    if (points.size() < 4) {
        printf("Error: At least 4 points are required for B-Spline.\n");
        return;
    }

    int n = points.size() - 1; // ����
    int k = 4;                // �������̶�Ϊ3��B������
    vector<float> knots(n + k + 1); // ���Ƚڵ�����

    // ���ɾ��Ƚڵ�����
    for (int i = 0; i < knots.size(); i++) {
        knots[i] = i;
    }

    // ����B��������
    glColor3f(c.r, c.g, c.b);
    glBegin(GL_LINE_STRIP);
    for (float u = knots[k - 1]; u <= knots[n + 1]; u += 0.01) {
        Point p = deBoor(n, k, u, points, knots);
        glVertex2f(p.x, p.y);
    }
    glEnd();
}


void Initial(void) {
    glClearColor(1.0f, 1.0f,1.0f,1.0f);
// ���ô��ڱ�����ɫ
}
void ChangeSize(int w, int h){
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION) ;
    glLoadIdentity();
    gluOrtho2D(0.0, winWidth,0.0, winHeight);
}
void drawLine(int x1, int y1, int x2, int y2) {
    glBegin(GL_LINES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glEnd();
}
void MousePlot(GLint button, GLint action, GLint xMouse, GLint yMouse){
    //�����Ӧ����
    if(start==true && button == GLUT_LEFT_BUTTON && action == GLUT_DOWN) //����������ȷ���ü��������Խ���λ��
    {
        if(iMousePointNum==0)
        {
            iMousePointNum = 1;
            x1 = xMouse;
            y1 = winHeight-yMouse;
            rectangle=true;
        }
        else {
            iMousePointNum = 0;
            rectangle=false;
            glutPostRedisplay();//ˢ��ҳ��
            }
    }
}
void PassiveMouseMove (GLint xMouse, GLint yMouse){
    if(iKeyPointNum == 1)//��ֻ��1���㣬������һ������Ҫ��������ƶ�λ��ʵʱ�仯
    {
        xKey2 = xMouse;
        yKey2 = winHeight-yMouse;//y2�ڴ��ڵ�ʵ��λ��Ϊ���ڸ߶ȼ�ȥymouse��Ϊ����ҳ���Ϸ��ĸ߶ȣ�
        glutPostRedisplay();
    }
}
void key(unsigned char key, GLint xKey, GLint yKey) {
    switch (key) {
        case 'p': // ����'p'ȷ�����߶˵�
            if (iKeyPointNum == 0) {
                points.push_back({xKey, winHeight-yKey});
                count++;
                iKeyPointNum = 1;
            } else if (iKeyPointNum == 1) {
                points.push_back({xKey, winHeight-yKey});
                count++;
                //iKeyPointNum = 2;
            }
            break;
        case 'e': // ����'e'��������
            iKeyPointNum = 0;
            isEnd=true;
            break;
        case 'c': // ����'c'ִ�вü�
            line=true;
            //Line_Clipping(points, winObj);
            glutPostRedisplay();
            break;
        default:
            break;
    }
    glutPostRedisplay();
}
void Display(void){
    glClear (GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);
    if(isEnd==false)
    {
        // ��������ģʽ����������ģʽΪ 0x00FF (ÿ�����߶κͿհ׶εĳ���һ��)
        glLineStipple(1, 0x00FF); // 1�����ӣ�0x00FF�����ߵ�ͼ��
        glEnable(GL_LINE_STIPPLE);
        glColor3f(3.0, 0.0, 0.0);
        glBegin(GL_LINE_STRIP);
        for (const auto& pt : points) {
            glVertex2f(pt.x, pt.y);
        }
        glVertex2f(xKey2, yKey2);
        glEnd();
        glDisable(GL_LINE_STIPPLE); // �ر�����ģʽ���ָ�Ϊʵ��
    }
    //���̡�c'ʵ�ֻ��ڴ��ڵ�ֱ�߲ü�
    else if(isEnd==true)
    {
        if(line==true)
        {
            switch(selection){
            case(1):
                if((points.size())%3==1 && points.size()>4)
                {
                    drawBezierCurve(points,points.size(),blackColor);
                }
                else
                {
                    drawBezierCurve(points,blackColor);
                }
                break;
            case(2):
                drawBezierCurve_deCasteljau(points,blackColor);
                break;//��Ҫ
            case(3):
                drawBSplineCurve_Definition(points,blackColor);
                break;
            case(4):
                drawBSplineCurve_deBoor(points,blackColor);
                break;
            default:
                break;
            }

            glLineStipple(1, 0x00FF);
            glEnable(GL_LINE_STIPPLE);
            glColor3f(3.0, 0.0, 0.0);
            glBegin(GL_LINE_STRIP);
            for (const auto& pt : points) {
                glVertex2f(pt.x, pt.y);
            }
            //����֮���ٻ��ƾͲ���Ҫ�ٻ���xKey��
            glEnd();
            glDisable(GL_LINE_STIPPLE);
        }
        else
        {
            glLineStipple(1, 0x00FF);
            glEnable(GL_LINE_STIPPLE);
            glColor3f(3.0, 0.0, 0.0);
            glBegin(GL_LINE_STRIP);
            for (const auto& pt : points) {
                glVertex2f(pt.x, pt.y);
            }
            //glVertex2f(xKey2, yKey2);
            //����֮���ٻ��ƾͲ���Ҫ�ٻ���xKey��
            glEnd();
            glDisable(GL_LINE_STIPPLE);
        }
    }
    glutSwapBuffers();
}
void ProcessMenu(int value){
    selection=value;
    start=true;
    line=false;
    rectangle=false;
    iMousePointNum= 0;// ��굥��ȷ���ü����ڵ����Ŀ
    x1=0;
    x2=0;
    y1=0;
    y2=0;
    iKeyPointNum = 0;// ����'p'ȷ��ֱ�߶ζ˵����Ŀ
    xKey1=0;
    xKey2=0;
    yKey1=0;
    yKey2=0;
    count=0;
    glutPostRedisplay();//ˢ��ҳ�棬���빦��
    points.clear();
}
int main(int argc, char*argv[]){
    glutInit (&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);// ʹ��˫���漰 RGB ģ��
    glutInitWindowSize (400, 300);//ָ�����ڵĳߴ�
    glutInitWindowPosition(100,100);// ָ����������Ļ�ϵ�λ��
    glutCreateWindow("Bezier���߻���");




    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("Bezier",1);
    glutAddMenuEntry("Bezier by de Casteljau",2);
    glutAddMenuEntry("B Spline by Definition",3);
    glutAddMenuEntry("B Spline by de Boor",4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);





    glutDisplayFunc(Display);
    glutReshapeFunc(ChangeSize) ;//ָ�����������λص�����



    glutMouseFunc (MousePlot) ;//ָ�������Ӧ����
    glutPassiveMotionFunc(PassiveMouseMove);//ָ������ƶ���Ӧ����
    glutKeyboardFunc(key);//������Ӧ����



    Initial();
    glutMainLoop();//������GLUT�¼�����ѭ��
    return 0;
}
