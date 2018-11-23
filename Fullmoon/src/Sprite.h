#ifndef __Sprite_H
#define __Sprite_H

#include "Shape.h"
#include "TimeClock.h"
#include "constant.h"

namespace EcologicEngine {

	double dis(GP  Point const &, GP  Point const &);
	double dis2(GP  Point const &, GP  Point const &);
	//���˶�����
	class Movable {};

	/*�ӿ�: �ɵ���Ķ���*/
	interface Clickable {
		virtual bool isClick(int, int) = 0;
	};

	/*�ӻ�����(����) = ����չʾ����(Rect) + ����ӻ��涯��(view) + ÿ���ӻ����֡����(�ڶ�����ʵ��)*/
	class Sprite : implements Clickable {
	private:
		static GP Point lhsP, rhsP;//���ܷ��� ֻ����ʱ�Ƚ�ʹ��
		//�ڴ�����(����ʱ����)
		void alloc() {
			offsetValue_ = new Offset();
			dir_ = new Direction();
			rect_ = new GP Rect();
		}
		bool dynamic = false;//true��ʾ���ڶ����Ƕ�̬��
		StaticFrameEnum staticFrame = ORIGIN_FRAME;
		Offset *offsetValue_;//ƫλ�� (d->x, d->y)
		//����ָ���� �ÿ�
		void null() {
			offsetValue_ = nullptr;
			dir_ = nullptr;
			rect_ = nullptr;
		}
		GP Rect *rect_;//��������� ����λ��(�������ط��ظ����)
		/*���Ͻ�*/
		int getLeft()const {
			return rect_->X;
		}
		int getTop()const {
			return rect_->Y;
		}
		/*���½�*/
		int getRight()const {
			return rect_->X + rect_->Width;
		}
		int getBottom()const {
			return rect_->Y + rect_->Height;
		}
	protected:/*�麯���õ��ĳ�Ա��Ӧ��Ϊ�̳пɷ��ʵ�(��ζ���Ǹ���Ա������Ҫ�������) ����ʹ������ҲӦ��ʹ����Ӧ�ķ��ʷ���*/
		RowsSub originView = 0;/*λͼ��Դ���������*/
		View lastView = defaultView;
		Direction *dir_;//��һ�ε��ƶ�����
		std::shared_ptr<Sprite const> parent;//������
		//��������Χ�ᱻ����
		static bool overRebound;
	public:
		//��������� (left, top)->(left+w, top+h) ���滻���ε�˫��ѭ�� ���������ڿ������õ��������ٶ�
		class Iterator {
			friend class Sprite;
			int limit = 0, i = 0;//��������, ��ǰ����
			void null() {
				iterD_ = nullptr;
				rect_ = nullptr;
			}
			Vector<int> *iterD_;//������ֵ Ĭ����1
			GP Rect *rect_;
			//��������ֵ
			void setD(int dx, int dy) {
				iterD_->x = dx, iterD_->y = dy;
			}
			//�����С
			void reSize(int w, int h) {
				rect_->Width = w;
				rect_->Height = h;
			}
			/*���� size��D��ȷ���� update����Ч*/
			void updateSize() {
				rect_->Width /= iterD_->x;
				rect_->Height /= iterD_->y;//���ܻ��Ϊ0
				limit = rect_->Width*rect_->Height;
			}

		public:
			//�����ṩһ��size(����û����)
			Iterator(GP Size const& size, int left = 0, int top = 0
				, unsigned dx = 1, unsigned dy = 1) {
				iterD_ = new Vector<int>(dx, dy);
				rect_ = new GP Rect(left, top, size.Width, size.Height);
				updateSize();
			}

			//��ֵ��ֵ ����ͨ���� ָ������ú�ֵ(�����ҵ����С����Ӧ��delete��ߺ󿽱������ұ� eg�������� �����ø�ֵ�ı����ǿ���������һ���ڴ�)
			Iterator& operator=(Iterator const &rhs) {
				limit = rhs.limit, i = rhs.i;
				*iterD_ = *rhs.iterD_;
				*rect_ = *rhs.rect_;
				return *this;
			}
			//��ֵ��ֵ �Ƹ�������(��<=�� ��������ֻ�и�) ��ָ������(��<==>��3��) (Ҳ����delete��ߺ󿽱������ұ� std::swap�ı���������ֵ��ϵͳ�Զ�����)
			Iterator& operator=(Iterator &&rvalue) {
				limit = rvalue.limit, i = rvalue.i;
				std::swap(iterD_, rvalue.iterD_);
				std::swap(rect_, rvalue.rect_);
				return *this;
			}
			//��ֵ�������� �����ڴ� ��ֵ��ֵ
			Iterator(Iterator const &rhs) {
				limit = rhs.limit, i = rhs.i;
				iterD_ = new Vector<int>(*rhs.iterD_);
				rect_ = new GP Rect(*rhs.rect_);
				/*Ч�����*/
				//alloc();
				//*this = rhs;
			}
			//��ֵ�������� ȫ�� �� ���ƶ���ֵ��������
			Iterator(Iterator &&rvalue) {
				//*this = std::move(rvalue);
				//rvalue.null();//ָ����󶼻��ÿ�û��Ҫ��һ��
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

			//������һ���� �Ѿ��յ㷵��false
			bool iterate(int &x, int &y) {
				i %= limit + 1;/*[0,limit]*/
				y /*rows*/ = i % rect_->Height * iterD_->y + rect_->Y;//y�����
				x /*cols*/ = i / rect_->Height * iterD_->x + rect_->X;//x����� �������ҵ�cols��
				return i++ < limit;
			}

			//������ʼλ��(left,top)
			void setLocation(int left, int top) {
				rect_->X = left, rect_->Y = top;
			}
		};
		//�������������ĵ����� ����
		Iterator iterator(int left, int top, int dx = 1, int dy = 1)const {
			return Iterator({ rect_->Width, rect_->Height }, left, top, dx, dy);
		}
		//��������������(��ԭλ��Ϊ�����������ڽ��е���) ���������->�ҽǵ� ������x y����Ĳ���ֵ
		Iterator pointIterator(int dx = 1, int dy = 1)const {
			return Iterator({ rect_->Width, rect_->Height }, rect_->X, rect_->Y, dx, dy);
		}
		//������ֵD���ֱ�ź�� ����������(��������ı��num���±�sub)
		Iterator indexIterator(int dr = 1, int dc = 1)const {
			return Iterator({ rect_->Width, rect_->Height }, 0, 0, dr, dc);
		}

		//�ܶ�����¶�����Ԥ��֪������
		Sprite() {
			alloc();
			setSize(Constant::GRID_CELL);
		}

		/*��֪����*/
		Sprite(GP Rect const &rect) {
			alloc();
			setRect(rect);
		}
		Sprite(int left, int top, GP Size const  &s) {
			alloc();
			setLocation(left, top);
			setSize(s);
		}

		/*��֪size*/
		Sprite(GP Size const &s) {
			alloc();
			setSize(s);
		}
		/*��֪size��λ��*/
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

		/*��ֵ��ֵ (1��ֵ)*/
		Sprite &operator=(Sprite const &rhs) {
			//*this = Sprite(rhs);//	����д����ֵrhs����->��ֵ��ֵ
			//return *this = std::move(*new Sprite(rhs));//���� һ����ֵ���� һ����ֵ��ֵ(��ԯ����)
			//return Sprite(rhs);//		������ʱ���� ��1������ û�и�ֵЧ��
			//return *new Sprite(rhs);//û�и�ֵЧ��
			//return *this = *new Sprite(rhs);//���Ǹ��ݹ�
			//�������ù��� �Ǹ�ֵ ���� ����...����ȫ��ƨ��
			dynamic = rhs.dynamic;
			lastView = rhs.lastView;
			*offsetValue_ = *rhs.offsetValue_;
			*dir_ = *rhs.dir_;
			*rect_ = *rhs.rect_;
			//@TODO ÿ�������Ƶ�Sprite��parent��ָ��ͬһ�� ����������
			parent = rhs.parent;
			originView = rhs.originView;
			return *this;
		}
		/*��ֵ�������� (1����+1��ֵ)*/
		Sprite(Sprite const &rhs) {
			alloc();
			*this = rhs;
		}

		//��ֵ��ֵ (1��ֵ) (һ�㳡��: ĳ���ط�����rvalue->��ֵ->������������)
		Sprite& operator=(Sprite &&rvalue) {
			/*ԭ���ڴ�Ҫdelete ��˲���ֱ�Ӹ�ֵ Ӧ�ý������� (��ֵ�ᱻϵͳ�Զ�����)*/
			dynamic = rvalue.dynamic;
			lastView = rvalue.lastView;
			std::swap(offsetValue_, rvalue.offsetValue_);//ǳ����
			std::swap(dir_, rvalue.dir_);
			std::swap(rect_, rvalue.rect_);
			parent = rvalue.parent;
			originView = rvalue.originView;
			return *this;
		}
		//��ֵ�������� (1��ֵ)
		Sprite(Sprite &&rvalue) {
			*this = std::move(rvalue);
			rvalue.null();
		}

		//����һ����¡����(new)
		//Sprite *clone(){return new Sprite(*this);}

		virtual ~Sprite() {
			//delete offsetValue_, dir_, rect_, nextLocation_;//����ֻ�����˵�һ����Ա...
			delete offsetValue_;
			delete dir_;
			delete rect_;
			null();
		}

		void setParent(std::shared_ptr<Sprite const> const &parent) {
			this->parent = parent;
		}

		//��ȫ���
		bool equals(IN Sprite const &rhs)const {
			return rect_->Equals(*rhs.rect_) == 0 ? false : true;
		}
		//��ײ���
		bool operator==(IN Sprite const &rhs)const {
			return collide(rhs);
		}
		// ������operator<���ǰ���������  ������Ȼ�޷����� ������ĵ����������� && fun();
		//��ײС�� ���������0���λ������ ���Լ�����ķ�����ײ��body
		bool operator<(IN Sprite const &rhs) const {
			rect_->GetLocation(&lhsP);
			rhs.rect_->GetLocation(&rhsP);
			return collide(rhs) ? false//(û������ײ)����ȵ�����°���λ������(λ�þ��Բ��� ��˲����ٱȽϿ��)
				: lhsP < rhsP;
		}
		/*�����ķ�������*/
		bool operator!=(IN Sprite const &rhs) const {
			return !(*this == rhs);
		}
		bool operator>=(IN Sprite const &rhs) const {
			return !(*this < rhs);//��С�ڵȼ���>=
		}
		bool operator<=(IN Sprite const &rhs) const {
			return (*this < rhs) || (*this == rhs);//С�ڻ���� �ȼ��� <=
		}
		bool operator>(IN Sprite const &rhs) const {
			//return !(*this <= rhs);//�� <= �ȼ��� >
			return rhs < *this;//this > rhs �ȼ��� rhs < this
		}
		// �����(ֻ�����и�Ϊ��������)
		Sprite operator-=(IN Sprite const &rhs) {
			_ASSERT_EXPR(this->getWidth() == rhs.getWidth() || this->getHeight() == rhs.getHeight(), "lhs��rhs��size֮һ������� �����и�Ϊ��������");
			_ASSERT_EXPR(this->contains(rhs), "lhs�ӻ���������rhs�ӻ���");
			// �������Ӿ���λ�����Ҳ�
			if (this->getTop() == rhs.getTop()) {
				this->setSize(getWidth() - rhs.getWidth(), getHeight());
				// lhsλ��rhs���
				if (this->getLeft() < rhs.getLeft()) {
					// ����ı�λ��
				}
				else {
					this->rect_->X = rhs.rect_->GetRight();
				}
			}
			else {
				this->setSize(getWidth(), getHeight() - rhs.getHeight());
				// lhsλ��rhs�²�
				if (this->getTop() < rhs.getTop()) {
					// ����ı�λ��
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

		/*ʹvalue������[zero,border]��ƫ�� ƫ����Ϊmodify ��������Χ������overRebound�ж��Ƿ񱻵���*/
		template<class T, class T2> void skewing(T &value, T2 &modify, T border, T zero = 0)const {
			value += modify;
			//modify = -modify;
			modify *= overRebound ? -1 : 1;
			value > border ? value = border
				: value < zero ? value = zero
				//: modify = -modify;
				: modify *= overRebound ? -1 : 1;
		}
		/*����value��target ƫ����Ϊd  ��������ֵ ��Сλ��d��Ϊһ����ͼ���ӵĴ�С*/
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
		//����value����target�ı仯ֵ
		int correction(int value, int d = 8, int target = 0)const {
			int dd = 0;
			if (value - d >= target)
				dd = -d;
			else if (value + d <= target)
				dd = d;
			else//���ϻ��ȥһ������ֵ����ʹ��"Խ��"Ŀ�� ȡ����Ŀ���Ĳ�ֵ
				dd = target - value;
			//value += dd;
			return dd;
		}
		//���߾����ƽ��
		double distance2(Sprite const &rhs)const {
			rect_->GetLocation(&lhsP);
			rhs.rect_->GetLocation(&rhsP);
			return dis2(lhsP, rhsP);
		}
		/*��ײ(impact)���*/
		bool collide(Sprite const &rhs)const {
			//return rect_->IntersectsWith(rhs.getRect()) == 0 ? false : true;
			return (getLeft() < rhs.getRight() && getTop() < rhs.getBottom() &&
				getRight() > rhs.getLeft() && getBottom() > rhs.getTop());
		}
		bool isClick(int x, int y) override {
			return getRect().Contains(x, y) == 0 ? false : true;
		}
		//����������rhs����true
		bool contains(Sprite const &rhs)const {
			//Gdiplus::Rect; ���ε�������Ҳ�� �²����ϲ��
			return (getLeft() <= rhs.getLeft()) && (getTop() <= rhs.getTop()) &&
				(getRight() >= rhs.getRight()) && (getBottom() >= rhs.getBottom());
		}

		/*���������ڱ������ڵ�һ�����λ�� ���û����޷����ڱ������� ����false*/
		bool randomLocation(Sprite &child)const {
			//Ϊ�˱��������Ҳ���Դ���(���ô˷���������ֻ��Sprite����) ���Բ�ʵ��Ϊ���� �µ�����ӻ���
			if (rect_->Contains(*child.rect_)) {
				child.setLocation(rand() % getWidth(), rand() % getHeight());
				return true;
			}
			else
				return false;
		}
		/*�����������ڸ������ڵ�һ�����λ�� ���û����޷����ڱ�������*/
		void randomLocation() {
			int x = rand() % (parent->getWidth() - getWidth() - Constant::GRID_CELL.Width),
				y = rand() % (parent->getHeight() - getHeight() - Constant::GRID_CELL.Height);
			setLocation(x, y);
		}

		//������������dis (�ƶ�location)
		void downward(int dis) {
			rect_->Y += dis;
			dir_->y = dis;
		}
		//������������dis
		void rightward(int dis) {
			rect_->X += dis;
			dir_->x = dis;
		}
		//�����׼��normΪ(16*16)��ͼ���λ��(����洢ʱ�ǰ��±�ı�������� û�а��±괢��)
		Sprite& toSite(int modix = 0, int modiy = 0) {
			rect_->X = (rect_->X + modix) / rect_->Width * rect_->Width;
			rect_->Y = (rect_->Y + modiy) / rect_->Height * rect_->Height;
			//if (rect_->X == 0)rect_->X = rect_->Width;
			//if (rect_->Y == 0)rect_->Y = rect_->Height;
			return *this;
		}
		/*��ԭ�������λ��Ϊ��׼ �������仯 ���طŴ����Լ�*/
		//�ؿ�
		Sprite& broaden(double ratio) {
			rect_->Width = (int)(rect_->Width* ratio);
			return*this;
		}
		//����
		Sprite& heighten(double ratio) {
			rect_->Height = (int)(rect_->Height* ratio);
			return*this;
		}
		//�Ŵ�
		Sprite& enlargement(double ratio) {
			broaden(ratio);
			heighten(ratio);
			return *this;
		}

		//�����Զ����ŵĶ�����ͼλ����Դ��������
		void setOriginView(RowsSub originView_) {
			originView = originView_;
		}

		//���ƶ����򷵻�true
		bool haveTravellingMotion()const {
			//�����˵��ڱȽϷ� û��ƫλ��˵��û���ƶ�����
			return *offsetValue_ != 0;
		}

		//�ڸ������ڽ���ƫλ������  n. �������ƶ������ƣ����������鰸         vi.�˶���������
		void motion() {
			if (*offsetValue_ != 0) {
				skewing(rect_->X, offsetValue_->x, parent->getWidth() - rect_->Width, 0);//����
				skewing(rect_->Y, offsetValue_->y, parent->getHeight() - rect_->Height - Constant::GRID_CELL.Height, 0);
			}
		}

		void motionX() {
			if (*offsetValue_ != 0) {
				skewing(rect_->X, offsetValue_->x, parent->getWidth() - rect_->Width, 0);//����
			}
		}

		void motionY() {
			if (*offsetValue_ != 0) {
				skewing(rect_->Y, offsetValue_->y, parent->getHeight() - rect_->Height - Constant::GRID_CELL.Height, 0);
			}
		}

		//�ƶ��ع�(ֻ�ܻع�һ��)
		void revertMotion() {
			offsetValue_->x = -offsetValue_->x;
			offsetValue_->y = -offsetValue_->y;
			motion();
			offsetValue_->x = -offsetValue_->x;
			offsetValue_->y = -offsetValue_->y;
		}

		//�ƶ��ع�(ֻ�ܻع�һ��)
		void revertMotionX() {
			offsetValue_->x = -offsetValue_->x;
			motionX();
			offsetValue_->x = -offsetValue_->x;
		}

		//�ƶ��ع�(ֻ�ܻع�һ��)
		void revertMotionY() {
			offsetValue_->y = -offsetValue_->y;
			motionY();
			offsetValue_->y = -offsetValue_->y;
		}

		//�ڸ������ڽ����ƶ�  (������)
		virtual void move() {
			//�ƶ��ı���: ��ͣ����λ�� ʹ�䷢��λ��displacement
			//��Ҫ�ﵽ�ƶ�Ŀ�ı�������ķ���;
			motion();
			updateDir();//���·���
		}

		//�����Ƿ�չʾ�ƶ�Ч��
		void setDynamic(bool dynamic_) {
			dynamic = dynamic_;
		}
		//�������Ƕ�̬�ķ���true
		bool isDynamic()const {
			return dynamic;
		}
		void setStaticFrame(StaticFrameEnum staticFrame) {
			this->staticFrame = staticFrame;
		}
		StaticFrameEnum getStaticFrame() {
			return this->staticFrame;
		}
		//������ǰ����ͼ
		void setPastView(View lView) {
			lastView = lView;
		}
		//����λͼ��Դ����±�
		void setOriginView(View lView) {
			originView = lView;
		}
		//���ص�ǰ����ͼ�±�(ע�����Ǹ��麯��)
		virtual Sub currentView() {
			return originView + lastView;//Ĭ������� ֻ��һ����ͼ lastView = 0;
		}
		//���·���ʸ�� ��ֻ֤�ڷ���䶯ʱ���·���(��:d->x d->y��Ϊ0ʱ���򲻱�)
		void updateDir() {
			if (offsetValue_->x != 0 || offsetValue_->y != 0) {
				dir_->x = offsetValue_->x, dir_->y = offsetValue_->y;
			}
			offsetValue_->clear();
		}
		//���ط���ʸ��
		Direction const &getDir() {
			return *dir_;
		}

		//ʹ�������һ����Ŀ�������(��������ײ����Ϊ�ﵽĿ�ĵ�)
		void updateD(Sprite const &destination) {
			offsetValue_->x = correction(rect_->X, 2, destination.getLeft());
			offsetValue_->y = correction(rect_->Y, 2, destination.getTop());
		}
		void updateD(GP Point const &target) {
			/*�����ٶ�Ϊ>1ʱ ��ͼ�赲�ж��������*/
			offsetValue_->x = correction(rect_->X, 1, target.X);
			offsetValue_->y = correction(rect_->Y, 1, target.Y);
		}
		void setD(int dx, int dy) {
			offsetValue_->x = dx, offsetValue_->y = dy;
		}
		//����ƫ��ʸ��
		Offset const &getD()const {
			return *offsetValue_;
		}
		int getDx()const {
			return offsetValue_->x;
		}
		int getDy()const {
			return offsetValue_->y;
		}

		//void setRect(GP Rect &&rect);//���Ǵ���� ���������ʱ���� �ᱻ����(û�������ַ���ֽ�)

		void setRect(GP Rect const &rect) {
			*rect_ = rect;
		}
		//ָ�����ֵ���� �������ⲿnewһ��Rect���� �ⲿ���� �ڲ�����(��ԭ��rect_Ϊnullptr�ܿ���ֻ��һ�ι���һ������)
		//(���Ǵ����� �ⲿ����->�ڲ���ֵ->�ⲿ����) ��һ�θ�ֵ(�൱�ڹ���)
		//�������ֹgetһ��ָ������ֱֵ�Ӵ��������� ��������� ����취1.Լ��getָ���򷵻�����(������ȡ��ַ�õ���Ҳ����ֵ) 2.ֻ��û�ж�Ӧ��get���������������ʹ�� (���߶��ú���...)
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
		//�������þ����λ��position(��Ҫְָλ...)
		void setLocation(int left, int top) {
			rect_->X = left, rect_->Y = top;
		}
		void setLocation(GP Point const &p) {
			setLocation(p.X, p.Y);
		}
		void setLocation(Sprite const &rhs) {
			setLocation(rhs.getLeft(), rhs.getTop());
		}
		//��������ļ�������
		void setCentre(GP Point const &centre) {
			rect_->X = centre.X - rect_->Width / 2;
			rect_->Y = centre.Y - rect_->Height / 2;
		}
		//����ǰλ������Ϊ����ļ�������
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
		//����
		void inflate(int w, int h) {
			rect_->Inflate(w, h);
		}
		int getWidth()const {
			return rect_->Width;
		}
		//���ش��ӻ��水��Constant::GRID_CELL�ָ�����������
		int colsLimit()const {
			return Constant::colSub(getWidth());
			//return getWidth() / Constant::GRID_CELL.Width;
		}
		int getHeight()const {
			return rect_->Height;
		}
		//���ش��ӻ��水��Constant::GRID_CELL�ָ�����������
		int rowsLimit()const {
			return Constant::rowSub(getHeight());
			//return getHeight() / Constant::GRID_CELL.Height;
		}
		//��ȡ����Բ�İ뾶
		int getRadius() const {
			return (int)((sqrt(pow(getWidth(), 2) + pow(getHeight(), 2))) / 2);
		}

		//�ɹ�����true (ֻ����λ��: Point)
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
