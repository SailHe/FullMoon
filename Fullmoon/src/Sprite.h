#ifndef __Sprite_H
#define __Sprite_H

#include "Shape.h"
#include "TimeClock.h"
#include "constant.h"

namespace EcologicEngine {

	double dis(GP  Point const &, GP  Point const &);
	double dis2(GP  Point const &, GP  Point const &);
	//可运动区域
	class Movable {};

	/*接口: 可点击的对象*/
	interface Clickable {
		virtual bool isClick(int, int) = 0;
	};

	/*子画面类(精灵) = 画面展示区域(Rect) + 多个子画面动画(view) + 每个子画面的帧动画(在动画类实现)*/
	class Sprite : implements Clickable {
	private:
		static GP Point lhsP, rhsP;//不能返回 只能临时比较使用
		//内存申请(构造时调用)
		void alloc() {
			offsetValue_ = new Offset();
			dir_ = new Direction();
			rect_ = new GP Rect();
		}
		bool dynamic = false;//true表示现在动画是动态的
		StaticFrameEnum staticFrame = ORIGIN_FRAME;
		Offset *offsetValue_;//偏位移 (d->x, d->y)
		//所有指针域 置空
		void null() {
			offsetValue_ = nullptr;
			dir_ = nullptr;
			rect_ = nullptr;
		}
		GP Rect *rect_;//区域决定了 精灵位置(根本不必返回给外界)
		/*左上角*/
		int getLeft()const {
			return rect_->X;
		}
		int getTop()const {
			return rect_->Y;
		}
		/*右下角*/
		int getRight()const {
			return rect_->X + rect_->Width;
		}
		int getBottom()const {
			return rect_->Y + rect_->Height;
		}
	protected:/*虚函数用到的成员才应设为继承可访问的(意味着那个成员可能需要子类管理) 否则即使在子类也应该使用相应的访问方法*/
		RowsSub originView = 0;/*位图资源起点行索引*/
		View lastView = defaultView;
		Direction *dir_;//上一次的移动方向
		std::shared_ptr<Sprite const> parent;//父画面
		//若超出范围会被弹回
		static bool overRebound;
	public:
		//区域迭代器 (left, top)->(left+w, top+h) 可替换矩形的双重循环 其意义在于可以设置迭代增长速度
		class Iterator {
			friend class Sprite;
			int limit = 0, i = 0;//迭代上限, 当前次数
			void null() {
				iterD_ = nullptr;
				rect_ = nullptr;
			}
			Vector<int> *iterD_;//步增长值 默认是1
			GP Rect *rect_;
			//设置增长值
			void setD(int dx, int dy) {
				iterD_->x = dx, iterD_->y = dy;
			}
			//重设大小
			void reSize(int w, int h) {
				rect_->Width = w;
				rect_->Height = h;
			}
			/*慎用 size与D都确定后 update后生效*/
			void updateSize() {
				rect_->Width /= iterD_->x;
				rect_->Height /= iterD_->y;//可能会变为0
				limit = rect_->Width*rect_->Height;
			}

		public:
			//至少提供一个size(否则没意义)
			Iterator(GP Size const& size, int left = 0, int top = 0
				, unsigned dx = 1, unsigned dy = 1) {
				iterD_ = new Vector<int>(dx, dy);
				rect_ = new GP Rect(left, top, size.Width, size.Height);
				updateSize();
			}

			//左值赋值 赋普通类型 指针解引用后赋值(若左右的域大小不等应先delete左边后拷贝构造右边 eg见动画类 解引用赋值的本意是可以少申请一次内存)
			Iterator& operator=(Iterator const &rhs) {
				limit = rhs.limit, i = rhs.i;
				*iterD_ = *rhs.iterD_;
				*rect_ = *rhs.rect_;
				return *this;
			}
			//右值赋值 移复杂类型(移<=赋 基本类型只有赋) 换指针类型(换<==>赋3次) (也可先delete左边后拷贝构造右边 std::swap的本意是让右值被系统自动析构)
			Iterator& operator=(Iterator &&rvalue) {
				limit = rvalue.limit, i = rvalue.i;
				std::swap(iterD_, rvalue.iterD_);
				std::swap(rect_, rvalue.rect_);
				return *this;
			}
			//左值拷贝构造 申请内存 左值赋值
			Iterator(Iterator const &rhs) {
				limit = rhs.limit, i = rhs.i;
				iterD_ = new Vector<int>(*rhs.iterD_);
				rect_ = new GP Rect(*rhs.rect_);
				/*效果差不多*/
				//alloc();
				//*this = rhs;
			}
			//右值拷贝构造 全移 或 用移动赋值函数定义
			Iterator(Iterator &&rvalue) {
				//*this = std::move(rvalue);
				//rvalue.null();//指针最后都会置空没必要换一遍
				limit = rvalue.limit, i = rvalue.i;
				iterD_ = rvalue.iterD_;
				rect_ = rvalue.rect_;
				rvalue.null();
			}

			virtual ~Iterator() {
				delete iterD_;
				delete rect_;
				null();
			}

			//迭代下一个点 已经终点返回false
			bool iterate(int &x, int &y) {
				i %= limit + 1;/*[0,limit]*/
				y /*rows*/ = i % rect_->Height * iterD_->y + rect_->Y;//y轴距离
				x /*cols*/ = i / rect_->Height * iterD_->x + rect_->X;//x轴距离 由左向右第cols列
				return i++ < limit;
			}

			//设置起始位置(left,top)
			void setLocation(int left, int top) {
				rect_->X = left, rect_->Y = top;
			}
		};
		//区域内任意起点的迭代器 慎用
		Iterator iterator(int left, int top, int dx = 1, int dy = 1)const {
			return Iterator({ rect_->Width, rect_->Height }, left, top, dx, dy);
		}
		//绝对坐标点迭代器(以原位置为基础在区域内进行迭代) 从区域左角->右角的 可设置x y方向的步进值
		Iterator pointIterator(int dx = 1, int dy = 1)const {
			return Iterator({ rect_->Width, rect_->Height }, rect_->X, rect_->Y, dx, dy);
		}
		//用增长值D划分编号后的 索引迭代器(索引区块的编号num即下标sub)
		Iterator indexIterator(int dr = 1, int dc = 1)const {
			return Iterator({ rect_->Width, rect_->Height }, 0, 0, dr, dc);
		}

		//很多情况下都不能预先知晓区域
		Sprite() {
			alloc();
			setSize(Constant::GRID_CELL);
		}

		/*已知区域*/
		Sprite(GP Rect const &rect) {
			alloc();
			setRect(rect);
		}
		Sprite(int left, int top, GP Size const  &s) {
			alloc();
			setLocation(left, top);
			setSize(s);
		}

		/*已知size*/
		Sprite(GP Size const &s) {
			alloc();
			setSize(s);
		}
		/*已知size及位置*/
		Sprite(GP Size const &s, GP Point const &p) {
			alloc();
			setSize(s);
			setLocation(p);
		}
		Sprite(GP Size const &s, int x, int y) {
			alloc();
			setSize(s);
			setLocation(x, y);
		}

		/*左值赋值 (1赋值)*/
		Sprite &operator=(Sprite const &rhs) {
			//*this = Sprite(rhs);//	这样写是左值rhs拷贝->右值赋值
			//return *this = std::move(*new Sprite(rhs));//可行 一次左值拷贝 一次右值赋值(南辕北辙)
			//return Sprite(rhs);//		返回临时变量 多1次析构 没有赋值效果
			//return *new Sprite(rhs);//没有赋值效果
			//return *this = *new Sprite(rhs);//这是个递归
			//根本不用构造 是赋值 不是 构造...上面全是屁话
			dynamic = rhs.dynamic;
			lastView = rhs.lastView;
			*offsetValue_ = *rhs.offsetValue_;
			*dir_ = *rhs.dir_;
			*rect_ = *rhs.rect_;
			//@TODO 每个被复制的Sprite的parent都指向同一个 可能有隐患
			parent = rhs.parent;
			originView = rhs.originView;
			return *this;
		}
		/*左值拷贝构造 (1构造+1赋值)*/
		Sprite(Sprite const &rhs) {
			alloc();
			*this = rhs;
		}

		//右值赋值 (1赋值) (一般场景: 某个地方构造rvalue->赋值->析构匿名对象)
		Sprite& operator=(Sprite &&rvalue) {
			/*原先内存要delete 因此不能直接赋值 应该交换交换 (右值会被系统自动析构)*/
			dynamic = rvalue.dynamic;
			lastView = rvalue.lastView;
			std::swap(offsetValue_, rvalue.offsetValue_);//浅拷贝
			std::swap(dir_, rvalue.dir_);
			std::swap(rect_, rvalue.rect_);
			parent = rvalue.parent;
			originView = rvalue.originView;
			return *this;
		}
		//右值拷贝构造 (1赋值)
		Sprite(Sprite &&rvalue) {
			*this = std::move(rvalue);
			rvalue.null();
		}

		//返回一个克隆对象(new)
		//Sprite *clone(){return new Sprite(*this);}

		virtual ~Sprite() {
			//delete offsetValue_, dir_, rect_, nextLocation_;//这样只析构了第一个成员...
			delete offsetValue_;
			delete dir_;
			delete rect_;
			null();
		}

		void setParent(std::shared_ptr<Sprite const> const &parent) {
			this->parent = parent;
		}

		//完全相等
		bool equals(IN Sprite const &rhs)const {
			return rect_->Equals(*rhs.rect_) == 0 ? false : true;
		}
		//碰撞相等
		bool operator==(IN Sprite const &rhs)const {
			return collide(rhs);
		}
		// 不重载operator<就是按父类排序  符号虽然无法抽象 但里面的调动方法可以 && fun();
		//碰撞小于 按照相对于0点的位置排序 离自己最近的发生碰撞的body
		bool operator<(IN Sprite const &rhs) const {
			rect_->GetLocation(&lhsP);
			rhs.rect_->GetLocation(&rhsP);
			return collide(rhs) ? false//(没发生碰撞)不相等的情况下按照位置排序(位置绝对不等 因此不用再比较宽高)
				: lhsP < rhsP;
		}
		/*派生的符号重载*/
		bool operator!=(IN Sprite const &rhs) const {
			return !(*this == rhs);
		}
		bool operator>=(IN Sprite const &rhs) const {
			return !(*this < rhs);//不小于等价于>=
		}
		bool operator<=(IN Sprite const &rhs) const {
			return (*this < rhs) || (*this == rhs);//小于或等于 等价于 <=
		}
		bool operator>(IN Sprite const &rhs) const {
			//return !(*this <= rhs);//不 <= 等价于 >
			return rhs < *this;//this > rhs 等价于 rhs < this
		}
		// 区域差(只允许切割为两个矩形)
		Sprite operator-=(IN Sprite const &rhs) {
			_ASSERT_EXPR(this->getWidth() == rhs.getWidth() || this->getHeight() == rhs.getHeight(), "lhs与rhs的size之一必须相等 才能切割为两个矩形");
			_ASSERT_EXPR(this->contains(rhs), "lhs子画面必须包涵rhs子画面");
			// 若两个子矩形位于左右侧
			if (this->getTop() == rhs.getTop()) {
				this->setSize(getWidth() - rhs.getWidth(), getHeight());
				// lhs位于rhs左侧
				if (this->getLeft() < rhs.getLeft()) {
					// 无需改变位置
				}
				else {
					this->rect_->X = rhs.rect_->GetRight();
				}
			}
			else {
				this->setSize(getWidth(), getHeight() - rhs.getHeight());
				// lhs位于rhs下侧
				if (this->getTop() < rhs.getTop()) {
					// 无需改变位置
				}
				else {
					this->rect_->Y = rhs.rect_->GetTop() + rhs.rect_->Height;
				}
			}
			return *this;
		}
		Sprite operator-(IN Sprite const &rhs) const {
			Sprite result = *this;
			result -= rhs;
			return result;
		}

		/*使value在区间[zero,border]内偏移 偏移量为modify 若超出范围会依据overRebound判断是否被弹回*/
		template<class T, class T2> void skewing(T &value, T2 &modify, T border, T zero = 0)const {
			value += modify;
			//modify = -modify;
			modify *= overRebound ? -1 : 1;
			value > border ? value = border
				: value < zero ? value = zero
				//: modify = -modify;
				: modify *= overRebound ? -1 : 1;
		}
		/*修正value至target 偏移量为d  返回修正值 最小位移d改为一个地图格子的大小*/
		template<class T> int correction(T &value, T d = 1, T target = 0)const {
			int dd = 0;
			if (value - d >= target)
				dd = -d;
			else if (value + d <= target)
				dd = d;
			else
				value = target;
			value += dd;
			return dd;
		}
		//返回value趋向target的变化值
		int correction(int value, int d = 8, int target = 0)const {
			int dd = 0;
			if (value - d >= target)
				dd = -d;
			else if (value + d <= target)
				dd = d;
			else//加上或减去一个增长值都会使其"越过"目标 取其与目标间的差值
				dd = target - value;
			//value += dd;
			return dd;
		}
		//两者距离的平方
		double distance2(Sprite const &rhs)const {
			rect_->GetLocation(&lhsP);
			rhs.rect_->GetLocation(&rhsP);
			return dis2(lhsP, rhsP);
		}
		/*碰撞(impact)检测*/
		bool collide(Sprite const &rhs)const {
			//return rect_->IntersectsWith(rhs.getRect()) == 0 ? false : true;
			return (getLeft() < rhs.getRight() && getTop() < rhs.getBottom() &&
				getRight() > rhs.getLeft() && getBottom() > rhs.getTop());
		}
		bool isClick(int x, int y) override {
			return getRect().Contains(x, y) == 0 ? false : true;
		}
		//包涵检测包涵rhs返回true
		bool contains(Sprite const &rhs)const {
			//Gdiplus::Rect; 矩形的左侧与右侧等 下侧与上侧等
			return (getLeft() <= rhs.getLeft()) && (getTop() <= rhs.getTop()) &&
				(getRight() >= rhs.getRight()) && (getBottom() >= rhs.getBottom());
		}

		/*将画面置于本画面内的一个随机位置 若该画面无法属于本画面内 返回false*/
		bool randomLocation(Sprite &child)const {
			//为了本类的子类也可以传入(调用此方法的往往只是Sprite对象) 所以不实现为返回 新的随机子画面
			if (rect_->Contains(*child.rect_)) {
				child.setLocation(rand() % getWidth(), rand() % getHeight());
				return true;
			}
			else
				return false;
		}
		/*将本画面置于父画面内的一个随机位置 若该画面无法属于本画面内*/
		void randomLocation() {
			int x = rand() % (parent->getWidth() - getWidth() - Constant::GRID_CELL.Width),
				y = rand() % (parent->getHeight() - getHeight() - Constant::GRID_CELL.Height);
			setLocation(x, y);
		}

		//将区域向下移dis (移动location)
		void downward(int dis) {
			rect_->Y += dis;
			dir_->y = dis;
		}
		//将区域向右移dis
		void rightward(int dis) {
			rect_->X += dis;
			dir_->x = dis;
		}
		//坐标标准化norm为(16*16)地图块的位置(坐标存储时是按下标的倍数储存的 没有按下标储存)
		Sprite& toSite(int modix = 0, int modiy = 0) {
			rect_->X = (rect_->X + modix) / rect_->Width * rect_->Width;
			rect_->Y = (rect_->Y + modiy) / rect_->Height * rect_->Height;
			//if (rect_->X == 0)rect_->X = rect_->Width;
			//if (rect_->Y == 0)rect_->Y = rect_->Height;
			return *this;
		}
		/*以原来的左角位置为基准 按比例变化 返回放大后的自己*/
		//拓宽
		Sprite& broaden(double ratio) {
			rect_->Width = (int)(rect_->Width* ratio);
			return*this;
		}
		//增高
		Sprite& heighten(double ratio) {
			rect_->Height = (int)(rect_->Height* ratio);
			return*this;
		}
		//放大
		Sprite& enlargement(double ratio) {
			broaden(ratio);
			heighten(ratio);
			return *this;
		}

		//决定自动播放的动画视图位于资源的行索引
		void setOriginView(RowsSub originView_) {
			originView = originView_;
		}

		//有移动意向返回true
		bool haveTravellingMotion()const {
			//重载了等于比较符 没有偏位移说明没有移动意向
			return *offsetValue_ != 0;
		}

		//在父画面内进行偏位移清算  n. 动作；移动；手势；请求；意向；议案         vi.运动；打手势
		void motion() {
			if (*offsetValue_ != 0) {
				skewing(rect_->X, offsetValue_->x, parent->getWidth() - rect_->Width, 0);//计算
				skewing(rect_->Y, offsetValue_->y, parent->getHeight() - rect_->Height - Constant::GRID_CELL.Height, 0);
			}
		}

		void motionX() {
			if (*offsetValue_ != 0) {
				skewing(rect_->X, offsetValue_->x, parent->getWidth() - rect_->Width, 0);//计算
			}
		}

		void motionY() {
			if (*offsetValue_ != 0) {
				skewing(rect_->Y, offsetValue_->y, parent->getHeight() - rect_->Height - Constant::GRID_CELL.Height, 0);
			}
		}

		//移动回滚(只能回滚一次)
		void revertMotion() {
			offsetValue_->x = -offsetValue_->x;
			offsetValue_->y = -offsetValue_->y;
			motion();
			offsetValue_->x = -offsetValue_->x;
			offsetValue_->y = -offsetValue_->y;
		}

		//移动回滚(只能回滚一次)
		void revertMotionX() {
			offsetValue_->x = -offsetValue_->x;
			motionX();
			offsetValue_->x = -offsetValue_->x;
		}

		//移动回滚(只能回滚一次)
		void revertMotionY() {
			offsetValue_->y = -offsetValue_->y;
			motionY();
			offsetValue_->y = -offsetValue_->y;
		}

		//在父画面内进行移动  (父画面)
		virtual void move() {
			//移动的本质: 不停修正位置 使其发生位移displacement
			//想要达到移动目的必须调动的方法;
			motion();
			updateDir();//更新方向
		}

		//设置是否展示移动效果
		void setDynamic(bool dynamic_) {
			dynamic = dynamic_;
		}
		//若动画是动态的返回true
		bool isDynamic()const {
			return dynamic;
		}
		void setStaticFrame(StaticFrameEnum staticFrame) {
			this->staticFrame = staticFrame;
		}
		StaticFrameEnum getStaticFrame() {
			return this->staticFrame;
		}
		//设置先前的视图
		void setPastView(View lView) {
			lastView = lView;
		}
		//设置位图资源起点下标
		void setOriginView(View lView) {
			originView = lView;
		}
		//返回当前的视图下标(注意这是个虚函数)
		virtual Sub currentView() {
			return originView + lastView;//默认情况下 只有一个视图 lastView = 0;
		}
		//更新方向矢量 保证只在方向变动时更新方向(即:d->x d->y都为0时方向不变)
		void updateDir() {
			if (offsetValue_->x != 0 || offsetValue_->y != 0) {
				dir_->x = offsetValue_->x, dir_->y = offsetValue_->y;
			}
			offsetValue_->clear();
		}
		//返回方向矢量
		Direction const &getDir() {
			return *dir_;
		}

		//使精灵具有一个向目标的趋势(若两者碰撞则视为达到目的地)
		void updateD(Sprite const &destination) {
			offsetValue_->x = correction(rect_->X, 2, destination.getLeft());
			offsetValue_->y = correction(rect_->Y, 2, destination.getTop());
		}
		void updateD(GP Point const &target) {
			/*增长速度为>1时 地图阻挡判定会出问题*/
			offsetValue_->x = correction(rect_->X, 1, target.X);
			offsetValue_->y = correction(rect_->Y, 1, target.Y);
		}
		void setD(int dx, int dy) {
			offsetValue_->x = dx, offsetValue_->y = dy;
		}
		//返回偏移矢量
		Offset const &getD()const {
			return *offsetValue_;
		}
		int getDx()const {
			return offsetValue_->x;
		}
		int getDy()const {
			return offsetValue_->y;
		}

		//void setRect(GP Rect &&rect);//这是错误的 传入的是临时变量 会被销毁(没法对其地址做手脚)

		void setRect(GP Rect const &rect) {
			*rect_ = rect;
		}
		//指针的右值引用 可以在外部new一个Rect传入 外部构造 内部销毁(若原先rect_为nullptr总开销只少一次构造一次析构)
		//(若是传引用 外部构造->内部赋值->外部销毁) 多一次赋值(相当于构造)
		//但必须防止get一个指针域右值直接传入再设置 会引起错误 解决办法1.约定get指针域返回引用(就算再取地址得到的也是左值) 2.只在没有对应的get方法的需管理域上使用 (两者都用好了...)
		void setRect(GP Rect *&&rvalue) {
			delete rect_;
			rect_ = rvalue;
			rvalue = nullptr;
		}
		GP Rect const &getRect()const {
			return *rect_;
		}
		GP RectF &tmpRectF()const {
			static GP RectF rf;
			rf.X = (GP REAL)rect_->X, rf.Y = (GP REAL)rect_->Y;
			rf.Width = (GP REAL)rect_->Width, rf.Height = (GP REAL)rect_->Height;
			return rf;
		}
		GP Point const &getLocation()const {
			static GP Point locationBuffer;
			locationBuffer.X = getLeft();
			locationBuffer.Y = getTop();
			return locationBuffer;
		}
		//设置设置精灵的位置position(主要指职位...)
		void setLocation(int left, int top) {
			rect_->X = left, rect_->Y = top;
		}
		void setLocation(GP Point const &p) {
			setLocation(p.X, p.Y);
		}
		void setLocation(Sprite const &rhs) {
			setLocation(rhs.getLeft(), rhs.getTop());
		}
		//设置区域的几何中心
		void setCentre(GP Point const &centre) {
			rect_->X = centre.X - rect_->Width / 2;
			rect_->Y = centre.Y - rect_->Height / 2;
		}
		//将当前位置设置为区域的几何中心
		void setCentre() {
			rect_->X -= rect_->Width / 2;
			rect_->Y -= rect_->Height / 2;
		}
		GP Point &getCentre(GP Point &centre)const {
			centre.X = rect_->X + rect_->Width / 2;
			centre.Y = rect_->Y + rect_->Height / 2;
			return centre;
		}
		GP Point const &getCentre()const {
			static GP Point centreBuffer;
			return getCentre(centreBuffer);
		}
		GP Size const &getSize() {
			static GP Size sizeBuffer;
			sizeBuffer.Width = getWidth();
			sizeBuffer.Height = getHeight();
			return sizeBuffer;
		}
		void setSize(int w, int h) {
			rect_->Width = w, rect_->Height = h;
		}
		void setSize(const GP Size &s) {
			setSize(s.Width, s.Height);
		}
		//膨胀
		void inflate(int w, int h) {
			rect_->Inflate(w, h);
		}
		int getWidth()const {
			return rect_->Width;
		}
		//返回此子画面按照Constant::GRID_CELL分割后的列数限制
		int colsLimit()const {
			return Constant::colSub(getWidth());
			//return getWidth() / Constant::GRID_CELL.Width;
		}
		int getHeight()const {
			return rect_->Height;
		}
		//返回此子画面按照Constant::GRID_CELL分割后的行数限制
		int rowsLimit()const {
			return Constant::rowSub(getHeight());
			//return getHeight() / Constant::GRID_CELL.Height;
		}
		//获取外切圆的半径
		int getRadius() const {
			return (int)((sqrt(pow(getWidth(), 2) + pow(getHeight(), 2))) / 2);
		}

		//成功返回true (只保存位置: Point)
		bool save(FILE *fp) {
			rect_->GetLocation(&lhsP);
			return fwrite(&lhsP, sizeof(GP Point), 1, fp) == 1;
		}
		bool load(FILE *fp) {
			bool temp = fread(&lhsP, sizeof(GP Point), 1, fp) == 1;
			setLocation(lhsP);
			return temp;
		}
	};

}

#endif
