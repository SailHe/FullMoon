#include "stdafx.h"
//#include <graphics.h>
//#include "../Includes/turbo/graphics.h"
#include "gdi.h"
//_T
#include <tchar.h>

// ������infinity
#include"../myLib/Graph/Graph.h"
#include"../myLib/Graph/TreeObject.h"

// GetWindow: winuser.h (include Windows.h)
//#include <winuser.h>

template<class T>
//2Dʸ�� ָһ��ͬʱ���д�С�ͷ���������ƽ���ı��η���ļ��ζ��� Ҳ�ɱ�ʾ��(��û���ڱ�ʾ��)
//���������Ҫ�õ���λ�����Ļ���Ӧ��ʹ��double����
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

	// ����������(ģ��)
	double magnitude() const {
		return sqrt(pow(this->x, 2) + pow(this->y, 2));
	}
	//����������ӣ��õ���һ���������Ա�ʾΪԴ����������غϺ�������Ϊ�ڱ߹��ɵ�ƽ���ı��ε�һ���Խ���
	Vector add(Vector const &vector) {
		return Vector(this->x + vector.x, this->y + vector.y);
	}
	// �������
	Vector subtract(Vector const &vector) const {
		return Vector(this->x - vector.x, this->y - vector.y);
	}
	// ��ʸ�����(scalar product: ������) ʸ����ͶӰ����: ��һ������ͶӰ���ڶ��������ϣ�����: ������,������,����� -> ˳����Ҫ)
	T dotProduct(Vector const &vector) const {
		return this->x * vector.x + this->y * vector.y;
	}
	//�ߵ�����(���������Ĳ�)
	Vector edge(Vector const &vector) const {
		return this->subtract(vector);
	}
	// ��������(������)
	Vector prependicular() const {
		return Vector(this->y, -this->x);
	}
	// ��λ����(��λԪ) ��ģ����  unitization: ��Ԫ��
	Vector normalize() const {
		Vector v = Vector(0, 0);
		T m = this->magnitude();
		if (m != 0) {
			v.x = this->x / m;
			v.y = this->y / m;
		}
		return v;
	}
	// ��ȡ������
	Vector normal() const {
		return this->prependicular().normalize();
	}
	//��ά����ά�ռ�����������ļн�cos(a)
	T corner(Vector const &rhs){
		//https://zh.wikipedia.org/wiki/%E5%90%91%E9%87%8F
		return this->dotProduct(rhs)
			/ (this->magnitude()*rhs.magnitude());
	}
	//������Ҫ����
	View mainView(){
		return abs(this->x) > abs(this->y) ?
			(this->x <= 0 ? LEFT : RIGHT) :/*withX*/
			(this->y <= 0 ? BACK : FRONT);/*withY*/
	}
};

//������ײʸ��collision
using CollisionVector = Vector<double>;
using CollisionValue = CollisionVector::ValueType;
//���鷽��ʸ��
using Direction = Vector<double>;
//����ƫ��ʸ��(ƫλ��) skewingVector
using Offset = Vector<int>;

//�����ٶ�ʸ��
struct Velocity: public Vector<double>{
};


/******** ��ײͼ�� *********/

//ƽ����
struct TranslationVector{
public:
	TranslationVector(){}
	TranslationVector(CollisionVector const &axis, CollisionValue const &overlap){
		this->axis = axis;
		this->overlap = overlap;
	}
	//ͶӰ��: ƽ�Ʒ�����Ǹ�ͶӰ��ķ���
	CollisionVector axis;
	//�������
	CollisionValue overlap;
};

// ͶӰ: ��άͼ�ε�ͶӰ(һά: һ���߶�)
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
	//��ͶӰ�Ľ������ (��>0��ʾ�ཻ; ==0��ʾ����; <0��ʾ����)
	CollisionValue overlap(Projection const &projection){
		return min(this->maxPoint, projection.maxPoint) - max(this->minPoint, projection.minPoint);
	}
private:
	//һάͶӰ���ϵ�������(һά��): ��Сֵ ���ֵ �����������ӳɵ��߶η�ӳ�˸�2dͼ���ڸ�ͶӰ���ϵ�ͶӰ
	CollisionValue minPoint, maxPoint;
};

//�߶�
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

//��άͼ��: ͬ���͵���ײ���(��ײ����ʵ����״���໥��ײ)
class Shape{
public:
	int typeId() const ;

	//��ȡ��Ҫ����ͶӰ�� (���бߵĴ���)
	virtual LinkedList<CollisionVector> listAxes() const = 0;

	// Ͷ��: ��ȡ��ĳһ�����ϵ�ͶӰ
	virtual Projection project(CollisionVector const &axis) const = 0;

	//void move() {throw "move(x, y) not implemented";}

	// ���ÿ������ͶӰ�ļ�������ش��ڵ���Сƫ����mtv
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

//����
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
	//��������, ����, ����(�������ֲ�ƽ��������)
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
		//@TODO ǿ��ת��
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
	//˳ʱ����ת(magnitude: ����)
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
	//����ʸ���ķ���<=1
	Direction dir;
};

//��Բ��(circular sector: ����)  �տ��� ��ʵ��
class EllipseShape : public Shape{
public:
private:
};

//Բ��
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
	//������Ӿ���
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
	//�뾶
	int radius;
	//Բ��
	GP Point centre;
};

//�����
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
		//�����б�(���е��Ӧ��ʸ����ͶӰ���ϵı����� ��ͶӰ��)
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
			//����
			//separate(shape, mtv, shape.velocity);
			//����
			//bounce(mtv, shape, shapes[j]);
		}
		return mtv.overlap > 0;
	}

	//TranslationVector polygonCollidesWithCircle(PolygonShape const &polygon, CircleShape const &circle);
	// ����κ�Բ��֮�����ײ
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
	//����vertices�ͱ�edges�����е��˳����밴˳ʱ�������ʱ��
	ArrayList<GP Point> points;
};

/**/