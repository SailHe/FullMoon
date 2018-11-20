////////////////////////////////////////////////////////////////////
// �������ƣ�������ͷʿҡ���ֶ�(Beatles)
// ���뻷����VC6.0 / 2010��EasyX 2011���ݰ�
// ԭ �� �ߣ�����������Ա Roman Cortes
// ԭ �� ��http://www.romancortes.com/blog/a-tribute-to-the-beatles/ (javascript �汾)
// ��ֲ���ߣ�krissi <zh@easyx.cn>
// ����޸ģ�2012-3-5
//
#include <graphics.h>
#include <conio.h>
#include <math.h>

#define PI      3.1415926535
#define WIDTH   640
#define HEIGHT  480


// �����ַ�Ԫ�ؽṹ��
struct ELEMENT
{
	TCHAR c;
	int  x, y;
	int  xo, yo;
	int  xd, yd;
};


// ����ȫ�ֱ���
TCHAR   g_beatles[]                 // ���
= _T("Yesterday,\nAll my troubles seemed so far away,\nNow it looks as though they're here to stay,\nOh, I believe in yesterday.\nSuddenly,\nI'm not half the man I used to be,\nThere's a shadow hanging over me,\nOh, yesterday came suddenly.\nWhy she\nHad to go I don't know, she wouldn't say.\nI said,\nSomething wrong, now I long for yesterday.\nYesterday,\nLove was such an easy game to play,\nNow I need a place to hide away,\nOh, I believe in yesterday.\nWhy she\nHad to go I don't know, she wouldn't say.\nI said,\nSomething wrong, now I long for yesterday.\nYesterday,\nLove was such an easy game to play,\nNow I need a place to hide away,\nOh, I believe in yesterday.");
char    g_data[]                    // �ַ���������
= "Q;PQD0R1F1CMF=;I73F5=QP2<OQUN0=@49CA4KK54BOQM?BW;46G1CETWSVGB;QC0EOISUKVRF3EHL44C2T?RESL17KP:GA6@@MM6P@L59I1MA;ME?O58MKLT3CSNS6PD<:KRSCORFU8WLAURN=P:03CFP3Q>WUHCWNAOP:8LF77E43BL4S?UEHNW?27UGBFJ2CC0WNI9;G?P1;G0GL35WRTMSBVB:@LJI>I03=J:RF1>>V3U?M3G=U7BF;GQ=E4=8RFBU23IMQ>GH1PINK:7D:JWC0UEPM9L>MR<SS7BL3B2LOSGW1;UI4MN:U>A6QRF?;>C7TLCU89CP?92A2U7UQCOEK<9A1S9W@7>AL562GTI7OF9?IE2UQPNSVNFK=PML1LNKFB8A32T4QOJ9W=QK?CEP6VA;UMQTA5K35W64MO25V3E5G7QC3BC=S0SB=A2GAO7VBTF8JF?BKLGQ8WPD>RECRPWSP9BPVKA2O56N?Q7DOO=3RDW;W53;QORWIF1:L22KV5SKV4=;@3=6W@0ECMJTBODP3HL=8TUMD2N1CNFSFR71D=PE>O?IN3EA5IM6HPSP@>U2S@OBG81NR551533394<K5TMTD076=;@N<C9MV>VATGMVBCR>TSKU5NGIQVES?;UD:URR7T?1E8GEAIUGS0O>PVS1GSWTP@SLLKKC?S3@34EUK<9HQ9J90:VGGI<Q6JIAG37UF32874DW3WC7>C82GGMI=?UOOOPL99:8H;>IBMLC?N3:W>4RRTR6MVA0NNEHSIFN6S?CTHGBA8A25Q;=R5@E6IT=;MADSN55909<UL>94MOWR4TIU59NGM?L4AJVN4LJG@1RRLO0CR>33<94>KE6Q5AI8V==9;T5CNU2F;O0O3GEKNISQQER0?U8@X8O1RUM7U@M8N9TQ=>UL=TQ=<ICG;JF6K5GH3<U4874M6D2QGP=PMQ>G26GESGGFC=GF3POOMNONJ3QCGT<WTRI6766;7F>H4=0ULE6;A@DRNK17D1RTD4FVTPO5WA6DVK1CVI7Q@Q?PPCQCD>QEH;FQVQ9OBQLHI?4R9P>QM3KJSDPN<N7FME6I:5T;7TE5889<RM02VMCIISGMG;0AQD:8A@MDFOCQMOC;G@;OWKK5CN66Q=GE=BTDTNIIR;M16QE3OOEW>2M87>RTL@2P:=JU6RPP@5T?>GMGASRQSQVVGS6>2=GB?RQ=JJ7AHFV@FD:NNVDPGE?<CTVCHIGP=IGKQM77H<HQ@A?2<U85FWOEN3I<1S3N1BI5P1HIQEHDHJ3QVNPMIMG@S:GVADRN5W<@JEDDHD3<KJUO28UQW>T4@J15PLURMQSER><G8GA;WGW<H75;40222J1L1W57EJ7QO5WD4D16:85DHNQN7:LFMD=>UB=K>PP3S8HJ7HVPP>;SANMTHVO0O0HBOP5M1ABQ73GE@O5@6P6PU9WRHQQ3W;F6A@ECVOWI1S5E35@<?M728A<<EPB:H:TODGAEJ41IUCUNTATBFU5TGE6=762>=P2KS97O89U2:U>OLB91J>FF>2MWV8P;1F<R1";
int     an = -1;               // ������ֵ��
int     dir = -1;               // �������з���
bool    bAni = false;            // �������б�־
ELEMENT g_Element[520];             // �ַ�Ԫ�����顣�� 520 ���ַ���


// ��ʼ��ÿ���ַ�Ԫ��
void init()
{
	TCHAR s[45];                    // ���һ������ 44 ���ַ�
	int x, y;                       // ÿ���ַ�������
	int rowwidth;                   // ÿ���ʵĿ��
	int ig = 0, ir = 0, ie = 0;     // �±����(ȫ������±ꡢһ�����±ꡢ�ַ�Ԫ���±�)
	y = (HEIGHT - 470) / 2;         // �����һ�е� y ����

	// ����ÿ���ַ�Ԫ�ص���ʼ����
	while (ie < 520)
	{
		if ((g_beatles[ig] != '\n') && (g_beatles[ig] != 0))
		{
			s[ir++] = g_beatles[ig];
		}
		else
		{
			s[ir] = 0;
			rowwidth = textwidth(s);
			x = (WIDTH - rowwidth) / 2;

			for (int i = 0; i < ir; i++)
			{
				if (s[i] != ' ')
				{
					g_Element[ie].c = s[i];
					g_Element[ie].x = g_Element[ie].xo = x;
					g_Element[ie].y = g_Element[ie].yo = y;
					outtextxy(g_Element[ie].xo, y, s[i]);
					x += textwidth(g_Element[ie].c);
					ie++;
				}
				else
					x += textwidth(' ');
			}

			ir = 0;
			y += 19;
		}

		ig++;
	}

	// ����ÿ���ַ�Ԫ�ص���ֹ����
	for (int n = 0; n < 520; n++)
	{
		int p = g_data[n * 3] * 1600 + g_data[n * 3 + 1] * 40 + g_data[n * 3 + 2] - 78768;
		g_Element[n].yd = p % 284;
		g_Element[n].xd = (p - g_Element[n].yd) / 284;
		g_Element[n].yd += 74 + (HEIGHT - 470) / 2;
		g_Element[n].xd += 111 + (WIDTH - 450) / 2;
	}

	// ��ʾˢ��
	FlushBatchDraw();
}


// ����
void ani()
{
	int i, m, n;

	// ������Ҫ�˶����ַ�Ԫ��
	for (n = 0; n < 130; n++)
	{
		m = 519 - n;
		if ((an - n >= 0) && (an - n <= 30))
		{
			double b = (cos((an - n) * PI / 30) + 1) / 2;
			double a = 1 - b;
			for (i = 0; i <= 130; i += 130)
			{
				g_Element[n + i].x = int((g_Element[n + i].xd) * a + g_Element[n + i].xo * b + 0.5);
				g_Element[n + i].y = int((g_Element[n + i].yd) * a + g_Element[n + i].yo * b + 0.5);
				g_Element[m - i].x = int((g_Element[m - i].xd) * a + g_Element[m - i].xo * b + 0.5);
				g_Element[m - i].y = int((g_Element[m - i].yd) * a + g_Element[m - i].yo * b + 0.5);
			}
		}
	}

	// ��ʾȫ���ַ�Ԫ��
	for (i = 0; i < 520; i++)
		outtextxy(g_Element[i].x, g_Element[i].y, g_Element[i].c);

	an += dir;

	if ((an < 0) || (an > 160))
		bAni = false;
}


// �����¼�
void anim()
{
	dir *= -1;      // �ı䶯������

	if ((an < 0) || (an > 160))
		bAni = true;
}


// ������
void main()
{
	// ������ʼ��
	initgraph(640, 480);            // ������ͼ����
	setbkcolor(WHITE);              // ���ñ���ɫΪ��ɫ
	setcolor(RGB(64, 64, 64));      // ����������ɫΪ���ɫ
	setbkmode(TRANSPARENT);         // �������ֱ���Ϊ͸��
	cleardevice();                  // �����Ļ
	BeginBatchDraw();               // ����������ͼģʽ
	setfont(-15, 0, _T("Arial"), 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);   // ��������

	// ��ʼ��ÿ���ַ�Ԫ��
	init();

	// ��������
	char c = 0;
	while (c != 27)
	{
		if (_kbhit())
		{
			c = _getch();
			anim();
		}
		if (bAni)
		{
			cleardevice();          // ����
			ani();                  // ִ�ж���
			FlushBatchDraw();       // ��ʾˢ��
		}

		Sleep(30);
	}

	// ������������Դ
	EndBatchDraw();
	closegraph();
}