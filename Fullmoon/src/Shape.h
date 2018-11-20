#include "stdafx.h"
//#include <graphics.h>
//#include "../Includes/turbo/graphics.h"
#include "gdi.h"
//_T
#include <tchar.h>

// 里面有infinity
#include"../myLib/Graph/Graph.h"
#include"../myLib/Graph/TreeObject.h"

// GetWindow: winuser.h (include Windows.h)
//#include <winuser.h>

template<class T>
//2D矢量 指一个同时具有大小和方向，且满足平行四边形法则的几何对象。 也可表示点(但没用于表示点)
//基本上如果要用到单位向量的话都应该使用double泛型
struct Vector{
	using ValueType = T;
	T x, y;

	Vector(){
		x = 0, y = 0;
	}
	Vector(T x_, T y_){
		x = x_, y = y_;
	}

	~Vector(){}

	void clear(){
		x = 0, y = 0;
	}

	bool operator==(T rhs)const{
		return x == rhs && y == rhs;
	}

	bool operator!=(T rhs)const{
		return !(*this == rhs);
	}

	// 求向量长度(模长)
	double magnitude() const {
		return sqrt(pow(this->x, 2) + pow(this->y, 2));
	}
	//两个向量相加，得到另一个向量可以表示为源向量的起点重合后，以它们为邻边构成的平行四边形的一条对角线
	Vector add(Vector const &vector) {
		return Vector(this->x + vector.x, this->y + vector.y);
	}
	// 向量求差
	Vector subtract(Vector const &vector) const {
		return Vector(this->x - vector.x, this->y - vector.y);
	}
	// 求矢量点积(scalar product: 标量积) 矢量点投影运算: 第一个向量投影到第二个向量上（满足: 交换律,分配律,结合律 -> 顺序不重要)
	T dotProduct(Vector const &vector) const {
		return this->x * vector.x + this->y * vector.y;
	}
	//边的向量(两点向量的差)
	Vector edge(Vector const &vector) const {
		return this->subtract(vector);
	}
	// 求垂线向量(法向量)
	Vector prependicular() const {
		return Vector(this->y, -this->x);
	}
	// 求单位向量(单位元) 求模运算  unitization: 单元化
	Vector normalize() const {
		Vector v = Vector(0, 0);
		T m = this->magnitude();
		if (m != 0) {
			v.x = this->x / m;
			v.y = this->y / m;
		}
		return v;
	}
	// 获取法向量
	Vector normal() const {
		return this->prependicular().normalize();
	}
	//二维和三维空间里的两向量的夹角cos(a)
	T corner(Vector const &rhs){
		//https://zh.wikipedia.org/wiki/%E5%90%91%E9%87%8F
		return this->dotProduct(rhs)
			/ (this->magnitude()*rhs.magnitude());
	}
	//返回主要方向
	View mainView(){
		return abs(this->x) > abs(this->y) ?
			(this->x <= 0 ? LEFT : RIGHT) :/*withX*/
			(this->y <= 0 ? BACK : FRONT);/*withY*/
	}
};

//精灵碰撞矢量collision
using CollisionVector = Vector<double>;
using CollisionValue = CollisionVector::ValueType;
//精灵方向矢量
using Direction = Vector<double>;
//精灵偏移矢量(偏位移) skewingVector
using Offset = Vector<int>;

//精灵速度矢量
struct Velocity: public Vector<double>{
};


/******** 碰撞图形 *********/

//平移量
struct TranslationVector{
public:
	TranslationVector(){}
	TranslationVector(CollisionVector const &axis, CollisionValue const &overlap){
		this->axis = axis;
		this->overlap = overlap;
	}
	//投影轴: 平移方向就是该投影轴的方向
	CollisionVector axis;
	//交叠间隔
	CollisionValue overlap;
};

// 投影: 二维图形的投影(一维: 一条线段)
class Projection{
public:
	Projection(){
		minPoint = infinity;
		maxPoint = -infinity;
	}
	Projection(CollisionValue min, CollisionValue max){
		this->minPoint = min;
		this->maxPoint = max;
	}
	//两投影的交叠间隔 (若>0表示相交; ==0表示相切; <0表示相离)
	CollisionValue overlap(Projection const &projection){
		return min(this->maxPoint, projection.maxPoint) - max(this->minPoint, projection.minPoint);
	}
private:
	//一维投影轴上的两个点(一维点): 最小值 最大值 这两个点连接成的线段反映了该2d图形在该投影轴上的投影
	CollisionValue minPoint, maxPoint;
};

//线段
class LineSegment{
public:
	LineSegment(){}
	LineSegment(GP PointF const &startPoint, GP PointF const &endPoint){
		this->startPoint = startPoint;
		this->endPoint = endPoint;
	}
	GP PointF const &getStartPoint() const {
		return startPoint;
	}
	GP PointF const &getEndPoint() const {
		return endPoint;
	}
private:
	GP PointF startPoint, endPoint;
};

//二维图形: 同类型的碰撞检测(碰撞盒中实现形状的相互碰撞)
class Shape{
public:
	int typeId() const ;

	//获取需要检测的投影轴 (所有边的垂线)
	virtual LinkedList<CollisionVector> listAxes() const = 0;

	// 投射: 获取在某一轴线上的投影
	virtual Projection project(CollisionVector const &axis) const = 0;

	//void move() {throw "move(x, y) not implemented";}

	// 检查每个轴上投影的间隔，返回存在的最小偏移量mtv
	TranslationVector minimumTranslationVector
		(LinkedList<CollisionVector> const &axes, Shape const &anotherShape) const {
		CollisionValue minOverlap = infinity, overlap;
		CollisionVector axis, axisWithSmallOverlap;
		Projection projection1, projection2;

		FOR_ALL_OBJECT(axes) {
			axis = element;
			projection1 = this->project(axis);
			projection2 = anotherShape.project(axis);
			overlap = projection1.overlap(projection2);
			if (overlap < minOverlap) {
				axisWithSmallOverlap = axis;
				minOverlap = overlap;
			}
			if (overlap <= 0){
				break;
			}
		}

		return TranslationVector(axisWithSmallOverlap, minOverlap);
	}
};

//射线
class Ray : public Shape{
public:
	Ray(){
	}
	Ray(GP Point const &startPoint, Direction const &dir){
		this->dir = dir.normalize();
		this->startPoint = startPoint;
	}

	LinkedList<CollisionVector> listAxes() const override{
		LinkedList<CollisionVector> temp;
		temp.push_back(dir.normal());
		return temp;
	}
	Projection project(CollisionVector const &axis) const override {
		CollisionValue n = axis.dotProduct(CollisionVector(this->startPoint.X, this->startPoint.Y));
		return Projection(n, n + 1);
	}
	//几何中心, 形心, 重心(对质量分布平均的物体)
	CollisionVector centroid() {
		return CollisionVector(startPoint.X, startPoint.Y);
	}

	bool collide(Shape const &rhs) const {
		LinkedList<CollisionVector> axes = this->listAxes();
		TranslationVector mtv = rhs.minimumTranslationVector(axes, *this);
		return mtv.overlap > 0;
	}

	LineSegment toLineSegment(int length) const {
		static GP PointF endPointBuffer;
		//@TODO 强制转换
		endPointBuffer = { (REAL)(startPoint.X + length*dir.x), (REAL)(startPoint.Y + length*dir.y) };
		return LineSegment(GP PointF((REAL)startPoint.X, (REAL)startPoint.Y), endPointBuffer);
	}
	void setDir(Direction const &dir){
		this->dir = dir.normalize();
	}
	Direction const &getDir(){
		static Direction re = dir.normalize();
		return re;
	}
private:
	//顺时针旋转(magnitude: 幅度)
	/*
	void clockwise(Direction::ValueType const &magnitude) {
		if (rotatingFlag){
			if (dir.y < 1){
				dir.y += magnitude;
			}
			else{
				dir.x -= magnitude;
				if (dir.x - -1 <= 0){
					rotatingFlag = false;
				}
			}
		}
		else{
			if (dir.x < 1){
				dir.x += magnitude;
			}
			else{
				dir.y -= magnitude;
				if (dir.y - -1 <= 0){
					rotatingFlag = true;
				}
			}
		}
		//dir.unitization();
	}
	bool rotatingFlag;
	/**/
	GP Point startPoint;
	//方向矢量的分量<=1
	Direction dir;
};

//椭圆形(circular sector: 扇形)  空壳类 无实意
class EllipseShape : public Shape{
public:
private:
};

//圆形
class CircleShape : public EllipseShape{
public:
	CircleShape(){}
	CircleShape(GP Point centre, int radius){
		this->centre = centre;
		this->radius = radius;
	}
	Projection project(CollisionVector const &axis) const override {
		CollisionValue dotProduct = CollisionVector(centre.X, centre.Y).dotProduct(axis);
		return Projection(dotProduct - this->radius, dotProduct + this->radius);
	}
	LinkedList<CollisionVector> listAxes() const override{
		return LinkedList<CollisionVector>();
	}
	void setCentre(GP Point const &centre) {
		this->centre = centre;
	}
	GP Point const & getCentre() const {
		return centre;
	}
	void setRadius(int radius) {
		this->radius = radius;
	}
	int getRadius() const {
		return radius;
	}
	//返回外接矩形
	/**
	PolygonShape const &boundingRectangle() const {
		static PolygonShape boundingPolygon;
		static GP Rect boundingRect;
		boundingRect.Width = boundingRect.Height = getRadius() * 2;
		boundingRect.X = centre.X - boundingRect.Width / 2;
		boundingRect.Y = centre.Y - boundingRect.Height / 2;
		boundingPolygon = PolygonShape(boundingRect);
		return boundingPolygon;
	}/**/
	bool collide(CircleShape const &rhs) const {
		static GP Point centre1, centre2;
		centre1 = this->getCentre();
		centre2 = rhs.getCentre();
		CollisionValue distance = sqrt(pow(centre1.X - centre2.X, 2)
			+ pow(centre1.Y - centre2.Y, 2));
		TranslationVector mtv = TranslationVector(
			(CollisionVector(centre1.X - centre2.X, centre1.Y - centre2.Y)).normalize()
			, this->getRadius() + rhs.getRadius() - (int)distance
			);
		return mtv.overlap > 0;
	}

	bool collide(Ray const &rhs) const {
		return rhs.collide(*this);
	}
private:
	//半径
	int radius;
	//圆心
	GP Point centre;
};

//多边形
class PolygonShape : public Shape{
public:
	PolygonShape(){}
	PolygonShape(GP Rect const &rect){
		addPoint(rect.GetLeft(), rect.GetTop());
		addPoint(rect.GetRight(), rect.GetTop());
		addPoint(rect.GetRight(), rect.GetBottom());
		addPoint(rect.GetLeft(), rect.GetBottom());
	}
	PolygonShape(ArrayList<GP Point> points){
		this->points = points;
	}
	ArrayList<GP Point> const &getPoints() const {
		return points;
	}
	
	Projection project(CollisionVector const &axis) const override {
		//标量列表(所有点对应的矢量在投影轴上的标量积 即投影点)
		LinkedList<double> scalarsList;
		CollisionVector v;
		FOR_ALL_OBJECT(this->points){
			GP Point const &point = element;
			v.x = point.X;
			v.y = point.Y;
			scalarsList.push_back(v.dotProduct(axis));
		}
		//return Projection(Math.min.apply(Math, scalarsList), Math.max.apply(Math, scalarsList));
		return Projection(
			*min_element(scalarsList.begin(), scalarsList.end())
			, *max_element(scalarsList.begin(), scalarsList.end())
			);
	}
	
	LinkedList<CollisionVector> listAxes() const override {
		CollisionVector v1 = CollisionVector(), v2 = CollisionVector();
		LinkedList<CollisionVector> axes;

		//for (LinkedList<GP Point>::iterator it = this->points.begin(); it != this->points.end(); ++it){
		FOR (it, this->points.begin(), this->points.end()){
 			GP Point const &point = *it;
			v1.x = point.X;
			v1.y = point.Y;
			if ((it + 1) != points.end()) {
				v2.x = (it + 1)->X; // points[i + 1].x; ---
				v2.y = (it + 1)->Y; // points[i + 1].y;
			}
			else {
				v2.x = (this->points.begin())->X; //points[0].x;
				v2.y = (this->points.begin())->Y; //points[0].y;
			}

			axes.push_back(v1.edge(v2).normal());
		}

		return axes;
	}

	CollisionVector centroid() {
		CollisionVector result = CollisionVector(0, 0);
		int n = this->points.size();
		FOR_ALL_OBJECT(this->points){
			GP Point &point = element;
			result.x += point.X;
			result.y += point.Y;
		}
		result.x /= n;
		result.y /= n;
		return result;
	}

	void addPoint(int x, int y) {
		this->points.push_back(GP Point(x, y));
	}

	void move(int dX, int dY) {
		FOR_ALL_OBJECT(this->points){
			GP Point &point = element;
			point.X += dX;
			point.Y += dY;
		}
	}

	bool collide(PolygonShape const &rhs) const {
		LinkedList<CollisionVector> axes = this->listAxes();
		//axes = axes.concat(p2.listAxes());
		LinkedList<CollisionVector> temp = rhs.listAxes();
		axes.splice(axes.end(), temp);
		TranslationVector mtv = this->minimumTranslationVector(axes, rhs);
		if (mtv.overlap > 0) {
			puts("");
			//分离
			//separate(shape, mtv, shape.velocity);
			//弹跳
			//bounce(mtv, shape, shapes[j]);
		}
		return mtv.overlap > 0;
	}

	//TranslationVector polygonCollidesWithCircle(PolygonShape const &polygon, CircleShape const &circle);
	// 多边形和圆形之间的碰撞
	bool collide(CircleShape const &rhs) const {
		LinkedList<CollisionVector> axes = this->listAxes();
		GP Point closestPoint = getPolygonPointClosestToCircle(*this, rhs);
		static GP Point centre;
		centre = rhs.getCentre();
		CollisionVector v1 = CollisionVector(closestPoint.X, closestPoint.Y);
		CollisionVector v2 = CollisionVector(centre.X, centre.Y);

		axes.push_back(v1.edge(v2).normalize());

		return this->minimumTranslationVector(axes, rhs).overlap > 0;
	}

	bool collide(Ray const &rhs) const {
		return rhs.collide(*this);
	}

private:
	GP Point getPolygonPointClosestToCircle(PolygonShape const &polygon, CircleShape const &circle) const {
		int min = infinity;
		GP Point closestPoint;
		static GP Point centre;
		centre = circle.getCentre();
		ArrayList<GP Point> points = polygon.getPoints();
		FOR_ALL_OBJECT(points){
			GP Point const &point = element;
			int length2 = (int)(pow(centre.X - point.X, 2) + pow(centre.Y - point.Y, 2));
			if (length2 < min) {
				min = length2;
				closestPoint = point;
			}
		}

		return closestPoint;
	};
	//顶点vertices和边edges，所有点的顺序必须按顺时针或者逆时针
	ArrayList<GP Point> points;
};

/**/