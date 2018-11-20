#include "Animation.h"

double dis2(GP  Point const &a, GP  Point const &b){
	return (a.X - b.X)*(a.X - b.X) + (a.Y - b.Y)*(a.Y - b.Y);
}
double dis(GP  Point const &a, GP  Point const &b){
	return sqrt(dis2(a, b));
}

int close_123(char const s[]){
	//closegraph();
	for (int i = 0; i < 10; ++i)
		puts("");
	puts(s);
	puts("");
	do{
		puts("esc���س����");
	} while (_getch() != 27);
	return -1;
}
int close_123(char const s[], double value){
	//closegraph();
	for (int i = 0; i < 10;++i)
		puts("");
	puts(s);
	puts("");
	printf("�Ƿ�ֵ:%.2f\n", value);
	do{
		puts("esc���س����");
	} while (_getch() != 27);
	return -1;
}

GP Point Sprite::lhsP;//������ֻ���������ڲ�����ʱ�Ƚ� ���ܷ���
GP Point Sprite::rhsP;
bool Sprite::overRebound = false;
WCHAR AnimationManager::buf[Constant::BUFFER_MAX_BIT] = { L"res\\" };//����·��
GP Graphics *AnimationManager::deviceGraphics_ = nullptr;
GP Graphics *AnimationManager::deviceBufferGraphics_ = nullptr;
GP Status AnimationManager::pastStatus = GP Status::GdiplusNotInitialized;
GP SizeF AnimationManager::enlargeProportion = GP SizeF(1.0, 1.0);
Pen *AnimationManager::myPen = nullptr;
Sprite WindowSprite::mouse;

AnimationManager WindowSprite::messenger;

int Timestamp::ID = -1;

size_t AnimationManager::getWidthA(size_t i)const{
	if (animation.size() > i && animation[i].size() > 0)
		return animation[i][0]->GetWidth();
	_DEBUG_ERROR("��������Խ��");
	return 0;
}
size_t AnimationManager::getHeightA(size_t i)const{
	if (animation.size() > i && animation[i].size() > 0)
		return animation[i][0]->GetHeight();
	_DEBUG_ERROR("��������Խ��");
	return 0;
}

void WindowSprite::speValBar(int value, int fullValue, GP Color color){
	static GP Rect area;
	area = getRect();//״̬��Χ
	area.Height = (int)(area.Height*0.2), area.Y -= (int)(area.Height*1.5);
	messenger.getGraphics()->DrawRectangle(&GP Pen(WHITE_), area);

	area.X++, area.Y++, area.Width = (value*area.Width / fullValue) - 1, area.Height--;
	LinearGradientBrush liGrBrush(/*���Խ��仭ˢ*/
		area, GP Color(color.GetA() / 2, color.GetR(), color.GetG(), color.GetB()), color,
		LinearGradientModeHorizontal);//ˮƽ����
	messenger.getGraphics()->FillRectangle(&liGrBrush, area);
	setLocation(area.X - 1, area.Y - 1);/*ʵʱ����λ��:����Χ����ƽ*/
}
void WindowSprite::specificValueToBuffer(int value, int fullValue, wstring const &str){
	wcscpy_s(messageBuffer, str.c_str());
	wsprintf(messageBuffer + str.length(), _T(" %d / %d"), value, fullValue);
}
void WindowSprite::inventoryTabs(Package const &bag, vector<Sprite> &ItemsBox){
	//һ����Ʒ�Ĺ̶����
	int w = getWidthOfItem(0), h = getHeightOfItem(0);
	auto rect = getRect();
	rect.X += w, rect.Y += h;
	GP Point &position = GP Point(rect.X, rect.Y);
	//��Ʒ����λ��
	//GP Rect itemRect;
	Sprite itemRect;
	FOR(i, 0, bag.size()){
		int id = bag[i].first, itemCnt = bag[i].second;
		itemRect = GP Rect(position.X - 1, position.Y - 2, w + 1, h + 1);
		itemRect.setOriginView(i);

		ItemsBox.emplace_back(itemRect);

		drawItem(id, position);
		messenger.getGraphics()->DrawRectangle(messenger.pen(), itemRect.getRect());
		wsprintf(messageBuffer, _T("%d"), itemCnt);
		text(messageBuffer, itemRect.getRect().GetRight() - w / 3, itemRect.getRect().GetBottom() - h / 2, 11, BLACK_);

		position.X += w;
		if (position.X > rect.GetRight() - 3 * w){
			position.X = rect.X;
			position.Y += h;
			position.Y %= rect.GetBottom() - 3 * h;//��ҳ��ʾ��û��
			position.Y == 0 ? position.Y = rect.Y : 0;
		}
	}
}
