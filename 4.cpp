#include<stdio.h>
#include <vector>
using namespace std;
#include <gl/glut.h>
#include "graphicType.h"
// 全局变量
vector<point> points;     // 存储折线的端点
//vector <point> pointVertex;   //控制多边形顶点序列
bool isEnd=false;			  //标识控制多边形是否确定

const GLint ControlN=4;       //由4个控制点定义三次bezier曲线段


color redColor(1,0,0),blackColor(0,0,0);//红色、黑色


bool start=false;//用于开始菜单的进入条件判断，如果start为true，则可以直接进行功能操作
bool line=false;
bool rectangle=false;

int x1=0, x2=0, y1=0, y2=0; // 鼠标操作中记录的裁剪窗口位置
int iMousePointNum= 0;// 鼠标单击确定裁剪窗口点的数目
int xKey1=0,xKey2=0,yKey1=0,yKey2=0;// 键盘操作中记录的鼠标位置
int iKeyPointNum = 0;// 键盘'p'确定直线段端点的数目
int winWidth = 400, winHeight = 300;
int count=0;

int selection;

//绘制折线
void drawPolygonalLine(vector<point> &points,color &c ){//根据点序列向量数组points，绘制折线
	glColor3f(c.r, c.g, c.b);
	glBegin (GL_LINE_STRIP);
		for(int i=0;i<points.size();i++){
			glVertex2i(points[i].x, points[i].y);
		}
	glEnd();
}
//绘制折线
void drawPolygonalLine(vector<point> &points ,point &p,color &c){//根据点序列向量数组points，绘制折线
	glColor3f(c.r, c.g, c.b);
	glBegin (GL_LINE_STRIP);
		for(int i=0;i<points.size();i++){
			glVertex2i(points[i].x, points[i].y);
		}
		glVertex2i(p.x, p.y);
	glEnd();
}
//如果控制点数n<4，绘制一段n-1次Bezier曲线段，否则绘制一段三次Bezier曲线
void drawBezierCurve(vector<Point> &points,color &c){
	GLfloat ControlP[4][3];
	int iPointNum=points.size();
	int i;
	if(iPointNum<4){//控制点数<4，绘制一段n-1次Bezier曲线段
		for(i=0;i<iPointNum;i++){
			ControlP[i][0]=points[i].x;
			ControlP[i][1]=points[i].y;
			ControlP[i][2]=0.0;
		}
	}
	else{
		for(i=0;i<4;i++){//控制点数>=4，绘制一段三次Bezier曲线段
			ControlP[i][0]=points[i].x;
			ControlP[i][1]=points[i].y;
			ControlP[i][2]=0.0;
		}
	}
	glColor3f(c.r, c.g, c.b);
	glPointSize(2);
	if(iPointNum>4)
		glMap1f(GL_MAP1_VERTEX_3,0.0,1.0,3,4,*ControlP);//定义一维取值器
	else
		glMap1f(GL_MAP1_VERTEX_3,0.0,1.0,3,iPointNum,*ControlP);//定义一维取值器
	glEnable(GL_MAP1_VERTEX_3);
	glMapGrid1f(100,0.0,1.0);//生成均匀分布的一维网格参数值
	glEvalMesh1(GL_LINE, 0, 100);//绘制Bezier曲线
}
void drawBezierCurve_deCasteljau(vector<Point> &points,color &c){
    int iPointNum = points.size();
    glColor3f(c.r, c.g, c.b);
    glPointSize(2);

    // 使用 de Casteljau 算法计算贝塞尔曲线上的点
    vector<Point> tempPoints = points;
    vector<Point> curvePoints;

    // 递归计算并插值生成曲线点
    for (int i = 0; i <= 100; ++i) {
        float t = i / 100.0f;

        // 深拷贝控制点
        tempPoints = points;

        // 逐步计算贝塞尔曲线点
        while (tempPoints.size() > 1) {
            vector<Point> nextPoints;
            for (int j = 0; j < tempPoints.size() - 1; ++j) {
                Point p;
                p.x = (1 - t) * tempPoints[j].x + t * tempPoints[j + 1].x;
                p.y = (1 - t) * tempPoints[j].y + t * tempPoints[j + 1].y;
                nextPoints.push_back(p);
            }
            tempPoints = nextPoints; // 继续迭代
        }

        // 将计算得到的曲线点加入到曲线点集合中
        curvePoints.push_back(tempPoints[0]);
    }

    // 绘制曲线
    glBegin(GL_LINE_STRIP);
    for (const auto &pt : curvePoints) {
        glVertex3f(pt.x, pt.y, 0.0f);
    }
    glEnd();
}
//通过（m×3+1）控制点，绘制m段n-1次ezier曲线段
void drawBezierCurve(vector<Point> &points, int n, color &c) {
    int m = (points.size() - 1) / 3; // 计算曲线段数
    vector<Point> adjustedPoints = points;

    // 调整控制点以实现 G1 连续性
    for (int i = 1; i < m; i++) {
        Point &P3 = adjustedPoints[i * 3];     // 当前段的 P3（连接点）
        Point &P2 = adjustedPoints[i * 3 - 1]; // 当前段的 P2
        Point &Q1 = adjustedPoints[i * 3 + 1]; // 下一段的 Q1

        // 根据 G1 连续性条件调整 Q1
        Q1.x = P3.x + (P3.x - P2.x);
        Q1.y = P3.y + (P3.y - P2.y);
    }

    // 绘制每段三次贝塞尔曲线
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


// 计算B样条基函数值
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

    int n = points.size() - 1; // 点数
    int k = 4;                // 阶数（固定为3次B样条）
    vector<float> knots(n + k + 1); // 均匀节点向量

    // 生成均匀节点向量
    for (int i = 0; i < knots.size(); i++) {
        knots[i] = i;
    }

    // 绘制B样条曲线
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

// de Boor 算法
Point deBoor(int n, int k, float u, const vector<Point>& points, const vector<float>& knots) {
    vector<Point> d(points); // 工作数组
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

    int n = points.size() - 1; // 点数
    int k = 4;                // 阶数（固定为3次B样条）
    vector<float> knots(n + k + 1); // 均匀节点向量

    // 生成均匀节点向量
    for (int i = 0; i < knots.size(); i++) {
        knots[i] = i;
    }

    // 绘制B样条曲线
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
// 设置窗口背景颜色
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
    //鼠标响应函数
    if(start==true && button == GLUT_LEFT_BUTTON && action == GLUT_DOWN) //鼠标左键单击确定裁剪窗口主对角线位置
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
            glutPostRedisplay();//刷新页面
            }
    }
}
void PassiveMouseMove (GLint xMouse, GLint yMouse){
    if(iKeyPointNum == 1)//若只有1个点，则另外一个点需要跟随鼠标移动位置实时变化
    {
        xKey2 = xMouse;
        yKey2 = winHeight-yMouse;//y2在窗口的实际位置为窗口高度减去ymouse（为距离页面上方的高度）
        glutPostRedisplay();
    }
}
void key(unsigned char key, GLint xKey, GLint yKey) {
    switch (key) {
        case 'p': // 键盘'p'确定折线端点
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
        case 'e': // 键盘'e'结束折线
            iKeyPointNum = 0;
            isEnd=true;
            break;
        case 'c': // 键盘'c'执行裁剪
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
        // 激活虚线模式，设置线型模式为 0x00FF (每个短线段和空白段的长度一样)
        glLineStipple(1, 0x00FF); // 1是因子，0x00FF是虚线的图案
        glEnable(GL_LINE_STIPPLE);
        glColor3f(3.0, 0.0, 0.0);
        glBegin(GL_LINE_STRIP);
        for (const auto& pt : points) {
            glVertex2f(pt.x, pt.y);
        }
        glVertex2f(xKey2, yKey2);
        glEnd();
        glDisable(GL_LINE_STIPPLE); // 关闭虚线模式，恢复为实线
    }
    //键盘‘c'实现基于窗口的直线裁剪
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
                break;//重要
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
            //结束之后再绘制就不需要再绘制xKey了
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
            //结束之后再绘制就不需要再绘制xKey了
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
    iMousePointNum= 0;// 鼠标单击确定裁剪窗口点的数目
    x1=0;
    x2=0;
    y1=0;
    y2=0;
    iKeyPointNum = 0;// 键盘'p'确定直线段端点的数目
    xKey1=0;
    xKey2=0;
    yKey1=0;
    yKey2=0;
    count=0;
    glutPostRedisplay();//刷新页面，进入功能
    points.clear();
}
int main(int argc, char*argv[]){
    glutInit (&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);// 使用双缓存及 RGB 模型
    glutInitWindowSize (400, 300);//指定窗口的尺寸
    glutInitWindowPosition(100,100);// 指定窗口在屏幕上的位置
    glutCreateWindow("Bezier曲线绘制");




    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("Bezier",1);
    glutAddMenuEntry("Bezier by de Casteljau",2);
    glutAddMenuEntry("B Spline by Definition",3);
    glutAddMenuEntry("B Spline by de Boor",4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);





    glutDisplayFunc(Display);
    glutReshapeFunc(ChangeSize) ;//指定窗口再整形回调函数



    glutMouseFunc (MousePlot) ;//指定鼠标响应函数
    glutPassiveMotionFunc(PassiveMouseMove);//指定鼠标移动响应函数
    glutKeyboardFunc(key);//键盘响应函数



    Initial();
    glutMainLoop();//启动主GLUT事件处理循环
    return 0;
}
