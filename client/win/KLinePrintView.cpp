
// KLinePrintView.cpp : CKLinePrintView ���ʵ��
//

#include "stdafx.h"
#include "KLinePrint.h"

#include "KLinePrintDoc.h"
#include "KLinePrintView.h"
#include "KLineCollection.h"
#include "TickReader.h"
#include "KLineRenderer.h"
#include "Utility.h"
#include "PlaybackConfDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKLinePrintView

IMPLEMENT_DYNCREATE(CKLinePrintView, CView)

BEGIN_MESSAGE_MAP(CKLinePrintView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CKLinePrintView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_PLAYBACK_BEGIN, &CKLinePrintView::OnPlaybackBegin)
	ON_COMMAND(ID_PLAYBACK_END, &CKLinePrintView::OnPlaybackEnd)
	ON_COMMAND(ID_PLAYBACK_FORWARD, &CKLinePrintView::OnPlaybackForward)
	ON_WM_TIMER()
	ON_COMMAND(ID_PLAYBACK_PAUSE, &CKLinePrintView::OnPlaybackPause)
	ON_COMMAND(ID_PLAYBACK_FASTFW, &CKLinePrintView::OnPlaybackFastfw)
	ON_COMMAND(ID_PLAYBACK_FASTREV, &CKLinePrintView::OnPlaybackFastrev)
	ON_COMMAND(ID_PLAYBACK_STOP, &CKLinePrintView::OnPlaybackStop)
	ON_COMMAND(ID_PLAYBACK_CONF, &CKLinePrintView::OnPlaybackConf)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CKLinePrintView ����/����

CKLinePrintView::CKLinePrintView()
{
	m_pTradeDialog = 0;
	m_bMouseDown = FALSE;
}

CKLinePrintView::~CKLinePrintView()
{

}

void CKLinePrintView::Set1MinData(KLineCollection* pData) 
{ 
	klr_1min.SetKLineData(pData, 240); 
	klr_1min.SetRenderMode(KLineRenderer::enAxisMode);
	klr_1min.SetShowAvg(true);
	klr_1min.SetShowMA(false);
	klr_1min.SetShowCriticalTime(true);
	klr_1min.SetShowMaxMin(false);	//	��ʾ���������ͼ�
	klr_1min.SetOpenIndex(1);
}

BOOL CKLinePrintView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	m_graph.SetParent( this );

	return CView::PreCreateWindow(cs);
}

// CKLinePrintView ����

void CKLinePrintView::OnDraw(CDC* pDC)
{
	CKLinePrintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect rc;
	GetClientRect(&rc);

//	m_graph.Redraw( pDC, rc );

#if 1
	CString txtSpeed;

	txtSpeed.Format(_T("%dX(%c)"), PBCONFIG.nPlaySpeed, PBCONFIG.bRealTime ? 'R':'-');

	m_MemDC.TextOut(rc.right - m_MemDC.GetTextExtent(txtSpeed).cx, rc.top, txtSpeed);

	//���ڴ��е�ͼ��������Ļ�Ͻ�����ʾ
	pDC->BitBlt(0,0,rc.Width(),rc.Height(),&m_MemDC,0,0,SRCCOPY);
#endif
}


// CKLinePrintView ��ӡ


void CKLinePrintView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CKLinePrintView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CKLinePrintView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ���Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CKLinePrintView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ���Ӵ�ӡ����е���������
}

void CKLinePrintView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
//	OnContextMenu(this, point);
}

// CKLinePrintView ���

#ifdef _DEBUG
void CKLinePrintView::AssertValid() const
{
	CView::AssertValid();
}

void CKLinePrintView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKLinePrintDoc* CKLinePrintView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKLinePrintDoc)));
	return (CKLinePrintDoc*)m_pDocument;
}
#endif //_DEBUG


// CKLinePrintView ��Ϣ��������

void CKLinePrintView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CKLinePrintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if(!pDoc->m_CurCsvFile.size()) return;

	if(nChar == VK_F1)
	{
		pDoc->AppendTradeRecord(EXCHANGE.Buy());
	}
	
	if(nChar == VK_F2)
	{
		pDoc->AppendTradeRecord(EXCHANGE.Sell());
	}

	if(nChar == VK_F3)
	{
		pDoc->AppendTradeRecord(EXCHANGE.Close());
	}

	if(nChar == VK_F4)
	{
		TradeRecordCollection trs = EXCHANGE.Reverse();

		for(int i = 0 ; i < trs.size(); i++)
			pDoc->AppendTradeRecord(trs[i]);	
	}

	if(nChar == VK_DELETE)
	{
		klr_1min.DelTrigger();
	}

	if(nChar == VK_F5)
	{
		klr_1min.ToggleTimeLine();
	}

	if(nChar == VK_F12)
	{
		if(m_pTradeDialog)
			m_pTradeDialog->ShowWindow(SW_SHOW);
	}

	if(nChar == VK_PRIOR )
	{
		pDoc->ViewNeighborDate(TRUE);
	}

	if(nChar == VK_NEXT )
	{
		pDoc->ViewNeighborDate(FALSE);
	}

	if(nChar == VK_UP)
	{
		klr_1min.ZoomIn();
	}

	if(nChar == VK_DOWN)
	{
		klr_1min.ZoomOut();
	}

	if(nChar == VK_LEFT)
	{
		klr_1min.MovePrev();
	}

	if(nChar == VK_RIGHT)
	{
		klr_1min.MoveNext();
	}

	if(nChar == VK_END)
	{
		klr_1min.ToggleRenderMode();
	}

	if(nChar == 'R')
	{
		PBCONFIG.bRealTime = !PBCONFIG.bRealTime;
		Utility::SavePlaybackConfig(PBCONFIG);
	}

	if(nChar == 'K')
	{
		klr_1min.ToggleKeyPrice();
	}	

	Render();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CKLinePrintView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bMouseDown)
	{
		int pr = klr_1min.GetPriceByPosition(point);
		klr_1min.SetTriggerPrice(pr);
		klr_1min.SetLossStopPrice(pr);
		klr_1min.SetProfitStopPrice(pr);
		Render();
	}

	CView::OnMouseMove(nFlags, point);
}

void CKLinePrintView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CKLinePrintDoc* pDoc = GetDocument();

	if(EXCHANGE.m_nPosition.nSlot)
	{
		pDoc->AppendTradeRecord(EXCHANGE.Close());
	}
	else
	{
		int price = klr_1min.GetPriceByPosition(point);

		//	����ڵ�ǰ�۸�֮�£�����
		if(price <= EXCHANGE.m_nTick.price)
		{
			pDoc->AppendTradeRecord(EXCHANGE.Buy(TP.nDefaultSlots, price));
		}
		//	����ڵ�ǰ�۸�֮�ϣ�����
		else
		{
			pDoc->AppendTradeRecord(EXCHANGE.Sell(TP.nDefaultSlots, price));
		}
	}

	CView::OnLButtonDblClk(nFlags, point);
}

void CKLinePrintView::Render()
{
	CKLinePrintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	klr_1min.SetTradeRecord(pDoc->GetTradeRecord());
	klr_1min.Render(&m_MemDC);

	Invalidate(FALSE);
}

void CKLinePrintView::Layout()
{
	CRect rc;
	
	GetClientRect(&rc);

	klr_1min.SetRect(rc);
}

void CKLinePrintView::OnSize(UINT nType, int cx, int cy)
{
	CRect rc;
	GetClientRect(&rc);

	//	�ؽ�MEMDC
	m_MemBitmap.DeleteObject();
	m_MemDC.DeleteDC();

	m_MemDC.CreateCompatibleDC(GetDC());
	m_MemBitmap.CreateCompatibleBitmap(GetDC(),cx,cy);

	m_MemDC.SelectObject(&m_MemBitmap);
	m_MemDC.FillSolidRect(&rc,RGB(255,255,255));

	Layout();
	
	Render();

	CView::OnSize(nType, cx, cy);
}

void CKLinePrintView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
	m_bMouseDown = false;

	CView::OnLButtonUp(nFlags, point);
}

void CKLinePrintView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CKLinePrintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	m_bMouseDown = true;

	klr_1min.Select(point);
	Render();

	CView::OnLButtonDown(nFlags, point);
}

void CKLinePrintView::OnPlaybackBegin()
{
	KillTimer(1);

	CKLinePrintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	EXCHANGE.SetTick(pDoc->GetTick());
	EXCHANGE.Close();

	if(m_pTradeDialog)
		m_pTradeDialog->EnableTrade(FALSE);

	pDoc->DisplayTill(0);
}

void CKLinePrintView::OnPlaybackEnd()
{
	KillTimer(1);

	CKLinePrintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	EXCHANGE.SetTick(pDoc->GetTick());
	EXCHANGE.Close();

	if(m_pTradeDialog)
		m_pTradeDialog->EnableTrade(FALSE);

	pDoc->LoadNextDay();
	pDoc->DisplayTill(PBCONFIG.nStartTime * 1000);
}

void CKLinePrintView::OnPlaybackForward()
{
	CKLinePrintDoc* pDoc = GetDocument();

	pDoc->DisplayTill(klr_1min.GetCurTime());

	SetTimer(1,1000,NULL); 

	if(!m_pTradeDialog)
	{
		m_pTradeDialog = new CTradeDialog;
		m_pTradeDialog->Create(IDD_TRADE,this);
		m_pTradeDialog->CenterWindow();
		m_pTradeDialog->ShowWindow(SW_SHOW); 
	}
	else
	{
		m_pTradeDialog->ShowWindow(SW_SHOW); 
	}
}

void CKLinePrintView::OnTimer(UINT_PTR nIDEvent)
{
	CKLinePrintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	KillTimer(1);

	Tick tick = pDoc->GetTick(0);
	Tick tick_next = pDoc->GetTick(1);
	Tick tick_next_2 = pDoc->GetTick(2);

	int nTickTime = pDoc->GetCurTickTime();

	if( nTickTime >= PBCONFIG.nEndTime * 1000)
	{
		/* �����Զ�ƽ�� */
		EXCHANGE.SetTick(pDoc->GetTick());
		EXCHANGE.Close();
		m_pTradeDialog->UpdateAccountInfo();

		//	TODO : ��ʾ���յĳɽ�����

		pDoc->LoadNextDay();
		pDoc->DisplayTill(PBCONFIG.nStartTime * 1000);
		
	}
	else if(nTickTime < PBCONFIG.nStartTime * 1000)
	{
		pDoc->DisplayTill(PBCONFIG.nStartTime * 1000);
	}
	else
	{
		if(PBCONFIG.bRealTime)
		{
			//	��������ǰʱ��
			pDoc->PlayTillTime(tick_next.time_ms);

			//	���㲥������һ��tick��ʱ��
			int this_tick_in_millisec = tick_next.time_ms;
			int next_tick_in_millisec = tick_next_2.time_ms;

			if(next_tick_in_millisec - this_tick_in_millisec > 300 * 1000)
			{
				//	������Ϣ��ͣ10s��ǿ��ƽ�֣��ڼ䲻��������

				if(TP.bForceCloseOnBreak)
				{
					EXCHANGE.SetTick(pDoc->GetTick());
					EXCHANGE.Close();
				}

				m_pTradeDialog->UpdateAccountInfo();
				m_pTradeDialog->EnableTrade(FALSE);
				SetTimer(1, 10 * 1000, NULL);
			}
			else
			{
				SetTimer(1, (next_tick_in_millisec - this_tick_in_millisec) / PBCONFIG.nPlaySpeed, NULL); 
				m_pTradeDialog->EnableTrade(TRUE);
			}
		}
		else
		{
			m_pTradeDialog->EnableTrade(TRUE);
			pDoc->PlayTillTime(pDoc->GetCurTickTime() + PBCONFIG.nPlaySpeed * 1000);
			SetTimer(1,1000,NULL);
		}
	}

	EXCHANGE.SetTick(pDoc->GetTick());

	int nPrevPrice = pDoc->GetTick(-1).price;
	int nCurPrice = pDoc->GetTick().price;

	/* ִ�д����� */
	if(EXCHANGE.m_nPosition.nTrigger)
	{
		if((nPrevPrice - EXCHANGE.m_nPosition.nTrigger) * (nCurPrice - EXCHANGE.m_nPosition.nTrigger) < 0
			|| nCurPrice == EXCHANGE.m_nPosition.nTrigger)
		{
			//	��������ֹ����ܴ���
			if(EXCHANGE.m_nPosition.nLossStop)
			{
				if(EXCHANGE.m_nPosition.nLossStop < EXCHANGE.m_nPosition.nTrigger)
				{
					pDoc->AppendTradeRecord(EXCHANGE.Buy(TP.nDefaultSlots, EXCHANGE.m_nPosition.nLossStop));
					AfxMessageBox(_T("��������!"));
				}
				else if(EXCHANGE.m_nPosition.nLossStop > EXCHANGE.m_nPosition.nTrigger)
				{
					pDoc->AppendTradeRecord(EXCHANGE.Sell(TP.nDefaultSlots, EXCHANGE.m_nPosition.nLossStop));					
					AfxMessageBox(_T("��������!"));
				}
			}
		}
	}

	if(EXCHANGE.m_nPosition.nSlot && EXCHANGE.m_nPosition.nLossStop &&
		((nPrevPrice - EXCHANGE.m_nPosition.nLossStop) * (nCurPrice - EXCHANGE.m_nPosition.nLossStop) < 0
		|| nCurPrice == EXCHANGE.m_nPosition.nLossStop))
	{
		pDoc->AppendTradeRecord(EXCHANGE.Close());
		AfxMessageBox(_T("ֹ��ƽ��!"));
	}

	if(EXCHANGE.m_nPosition.nSlot && EXCHANGE.m_nPosition.nProfitStop &&
		((nPrevPrice - EXCHANGE.m_nPosition.nProfitStop) * (nCurPrice - EXCHANGE.m_nPosition.nProfitStop) < 0
		|| nCurPrice == EXCHANGE.m_nPosition.nProfitStop))
	{
		pDoc->AppendTradeRecord(EXCHANGE.Close());
		AfxMessageBox(_T("ֹӯƽ��!"));
	}

	klr_1min.SelectLastK();

	Render();
	Invalidate(FALSE);

	if(m_pTradeDialog)
		m_pTradeDialog->UpdateAccountInfo();

	CView::OnTimer(nIDEvent);
}

void CKLinePrintView::OnPlaybackPause()
{
	//	��ͣʱ��ֹ����
	if(m_pTradeDialog)
		m_pTradeDialog->EnableTrade(FALSE);
	KillTimer(1);
}

void CKLinePrintView::OnPlaybackFastfw()
{
	if(PBCONFIG.bRealTime)
	{
		if(PBCONFIG.nPlaySpeed < 10)
			PBCONFIG.nPlaySpeed++;
	}
	else
	{
		if(PBCONFIG.nPlaySpeed < 30)
			PBCONFIG.nPlaySpeed += 5;
	}

	Utility::SavePlaybackConfig(PBCONFIG);

	Invalidate(FALSE);
}

void CKLinePrintView::OnPlaybackFastrev()
{
	if(PBCONFIG.bRealTime)
	{
		if(PBCONFIG.nPlaySpeed > 1)
			PBCONFIG.nPlaySpeed--;
	}
	else
	{
		if(PBCONFIG.nPlaySpeed >= 6)
			PBCONFIG.nPlaySpeed -= 5;
		else
			PBCONFIG.nPlaySpeed = 1;
	}

	Utility::SavePlaybackConfig(PBCONFIG);

	Invalidate(FALSE);
}

void CKLinePrintView::OnPlaybackStop()
{
	CKLinePrintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	EXCHANGE.SetTick(pDoc->GetTick());
	EXCHANGE.Close();
	
	if(m_pTradeDialog)
	{
		m_pTradeDialog->UpdateAccountInfo();
		m_pTradeDialog->EnableTrade(FALSE);
	}

	KillTimer(1);
	pDoc->DisplayTill(-1, -1);
}

void CKLinePrintView::OnPlaybackConf()
{
	CKLinePrintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CPlaybackConfDialog dlg;

	dlg.SetPlaybackConfig(PBCONFIG);

	if(IDOK == dlg.DoModal())
	{
		theApp.LoadPlaybackCalendar(PBCONFIG);
	}
}
