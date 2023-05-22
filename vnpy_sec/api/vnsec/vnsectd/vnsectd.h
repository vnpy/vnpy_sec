//ϵͳ
#ifdef WIN32
#include "pch.h"
#endif

#include "vnsec.h"
#include "pybind11/pybind11.h"
#include "DFITCSECTraderApi.h"

using namespace pybind11;
using namespace std;

//����

#define ONFRONTCONNECTED 0
#define ONFRONTDISCONNECTED 1
#define ONRTNNOTICE 2
#define ONRSPERROR 3
#define ONRSPSTOCKUSERLOGIN 4
#define ONRSPSTOCKUSERLOGOUT 5
#define ONRSPSTOCKUSERPASSWORDUPDATE 6
#define ONRSPSTOCKENTRUSTORDER 7
#define ONRSPSTOCKWITHDRAWORDER 8
#define ONRSPSTOCKQRYENTRUSTORDER 9
#define ONRSPSTOCKQRYREALTIMETRADE 10
#define ONRSPSTOCKQRYSERIALTRADE 11
#define ONRSPSTOCKQRYPOSITION 12
#define ONRSPSTOCKQRYCAPITALACCOUNTINFO 13
#define ONRSPSTOCKQRYACCOUNTINFO 14
#define ONRSPSTOCKQRYSHAREHOLDERINFO 15
#define ONRSPSTOCKTRANSFERFUNDS 16
#define ONRSPSTOCKENTRUSTBATCHORDER 17
#define ONRSPSTOCKWITHDRAWBATCHORDER 18
#define ONRSPSTOCKCALCABLEENTRUSTQTY 19
#define ONRSPSTOCKCALCABLEPURCHASEETFQTY 20
#define ONRSPSTOCKQRYFREEZEFUNDSDETAIL 21
#define ONRSPSTOCKQRYFREEZESTOCKDETAIL 22
#define ONRSPSTOCKQRYTRANSFERSTOCKDETAIL 23
#define ONRSPSTOCKQRYTRANSFERFUNDSDETAIL 24
#define ONRSPSTOCKQRYSTOCKINFO 25
#define ONRSPSTOCKQRYSTOCKSTATICINFO 26
#define ONRSPSTOCKQRYTRADETIME 27
#define ONSTOCKENTRUSTORDERRTN 28
#define ONSTOCKTRADERTN 29
#define ONSTOCKWITHDRAWORDERRTN 30
#define ONRSPSOPUSERLOGIN 31
#define ONRSPSOPUSERLOGOUT 32
#define ONRSPSOPUSERPASSWORDUPDATE 33
#define ONRSPSOPENTRUSTORDER 34
#define ONRSPSOPQUOTEENTRUSTORDER 35
#define ONRSPSOPGROUPSPLIT 36
#define ONRSPSOPGROUPEXECTUEORDER 37
#define ONRSPSOPQRYGROUPPOSITION 38
#define ONRSPSOPLOCKOUNLOCKSTOCK 39
#define ONRSPSOPWITHDRAWORDER 40
#define ONRSPSOPQRYENTRUSTORDER 41
#define ONRSPSOPQRYSERIALTRADE 42
#define ONRSPSOPQRYPOSITION 43
#define ONRSPSOPQRYCOLLATERALPOSITION 44
#define ONRSPSOPQRYCAPITALACCOUNTINFO 45
#define ONRSPSOPQRYACCOUNTINFO 46
#define ONRSPSOPQRYSHAREHOLDERINFO 47
#define ONRSPSOPCALCABLEENTRUSTQTY 48
#define ONRSPSOPQRYABLELOCKSTOCK 49
#define ONRSPSOPQRYCONTACTINFO 50
#define ONRSPSOPEXECTUEORDER 51
#define ONRSPSOPQRYEXECASSIINFO 52
#define ONRSPSOPQRYTRADETIME 53
#define ONRSPSOPQRYEXCHANGEINFO 54
#define ONRSPSOPQRYCOMMISSION 55
#define ONRSPSOPQRYDEPOSIT 56
#define ONRSPSOPQRYCONTRACTOBJECTINFO 57
#define ONSOPENTRUSTORDERRTN 58
#define ONSOPTRADERTN 59
#define ONSOPWITHDRAWORDERRTN 60
#define ONRSPSOPCAPITALTRANINOUT 61
#define ONRSPFASLUSERLOGIN 62
#define ONRSPFASLUSERLOGOUT 63
#define ONRSPFASLQRYABLEFININFO 64
#define ONRSPFASLQRYABLESLOINFO 65
#define ONRSPFASLTRANSFERCOLLATERAL 66
#define ONRSPFASLDIRECTREPAYMENT 67
#define ONRSPFASLREPAYSTOCKTRANSFER 68
#define ONRSPFASLENTRUSTCRDTORDER 69
#define ONRSPFASLENTRUSTORDER 70
#define ONRSPFASLCALCABLEENTRUSTCRDTQTY 71
#define ONRSPFASLQRYCRDTFUNDS 72
#define ONRSPFASLQRYCRDTCONTRACT 73
#define ONRSPFASLQRYCRDTCONCHANGEINFO 74
#define ONRSPFASLTRANSFERFUNDS 75
#define ONRSPFASLQRYACCOUNTINFO 76
#define ONRSPFASLQRYCAPITALACCOUNTINFO 77
#define ONRSPFASLQRYSHAREHOLDERINFO 78
#define ONRSPFASLQRYPOSITION 79
#define ONRSPFASLQRYENTRUSTORDER 80
#define ONRSPFASLQRYSERIALTRADE 81
#define ONRSPFASLQRYREALTIMETRADE 82
#define ONRSPFASLQRYFREEZEFUNDSDETAIL 83
#define ONRSPFASLQRYFREEZESTOCKDETAIL 84
#define ONRSPFASLQRYTRANSFERFUNDSDETAIL 85
#define ONRSPFASLWITHDRAWORDER 86
#define ONRSPFASLQRYSYSTEMTIME 87
#define ONRSPFASLQRYTRANSFERREDCONTRACT 88
#define ONRSPFASLDESIRABLEFUNDSOUT 89
#define ONRSPFASLQRYGUARANTEEDCONTRACT 90
#define ONRSPFASLQRYUNDERLYINGCONTRACT 91
#define ONFASLENTRUSTORDERRTN 92
#define ONFASLTRADERTN 93
#define ONFASLWITHDRAWORDERRTN 94

///-------------------------------------------------------------------------------------
///C++ SPI�Ļص���������ʵ��
///-------------------------------------------------------------------------------------

//API�ļ̳�ʵ��

class TdApi : public DFITCSECTraderSpi
{
private:
	DFITCSECTraderApi* api;            //API����
	thread task_thread;                    //�����߳�ָ�루��python���������ݣ�
	TaskQueue task_queue;                //�������
	bool active = false;                //����״̬
public:
	TdApi()
	{
	};

	~TdApi()
	{
		if (this->active)
		{
			this->exit();
		}
	};
	//-------------------------------------------------------------------------------------
	//API�ص�����
	//-------------------------------------------------------------------------------------

    /**
     * SEC-��������������Ӧ
     */
    virtual void OnFrontConnected();
    /**
     * SEC-�������Ӳ�������Ӧ
     */
    virtual void OnFrontDisconnected(int nReason);
    /**
     * SEC-��Ϣ֪ͨ
     */
    virtual void OnRtnNotice(DFITCSECRspNoticeField* pNotice);
    /**
    * ERR-����Ӧ��
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ַ
    */
    virtual void OnRspError(DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-��¼��Ӧ
    * @param pData:ָ�����ǿ�,�����û���¼��Ӧ��Ϣ�ṹ��ĵ�ַ,������¼����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ��������¼����ʧ��
    */
    virtual void OnRspStockUserLogin(DFITCSECRspUserLoginField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-�ǳ���Ӧ
    * @param pData:ָ�����ǿ�,�����û��ǳ���Ӧ��Ϣ�ṹ��ĵ�ַ,�����ǳ�����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�������ǳ�����ʧ��
    */
    virtual void OnRspStockUserLogout(DFITCSECRspUserLogoutField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-���������Ӧ
    * @param pData:ָ�����ǿ�,�����û����������Ӧ��Ϣ�ṹ��ĵ�ַ,���������������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�����������������ʧ��
    */
    virtual void OnRspStockUserPasswordUpdate(DFITCSECRspPasswordUpdateField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-ί�б�����Ӧ
    * @param pData:ָ�����ǿ�,�����û�ί�б�����Ӧ��Ϣ�ṹ��ĵ�ַ,������������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ������ί�б�������ʧ��
    */
    virtual void OnRspStockEntrustOrder(DFITCStockRspEntrustOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-ί�г�����Ӧ
    * @param pData:ָ�����ǿ�,�����û�ί�г�����Ӧ��Ϣ�ṹ��ĵ�ַ,������������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ������ί�г�������ʧ��
    */
    virtual void OnRspStockWithdrawOrder(DFITCSECRspWithdrawOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-ί�в�ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û�ί�в�ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,������ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ������ί�в�ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQryEntrustOrder(DFITCStockRspQryEntrustOrderField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-ʵʱ�ɽ���ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û�ʵʱ�ɽ���ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,����ʵʱ�ɽ���ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ������ʵʱ�ɽ���ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQryRealTimeTrade(DFITCStockRspQryRealTimeTradeField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-�ֱʳɽ���ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ֱʳɽ���ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,�����ֱʳɽ���ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�������ֱʳɽ���ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQrySerialTrade(DFITCStockRspQrySerialTradeField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-�ֲֲ�ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ֲֲ�ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,�����ֲֲ�ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�������ֲֲ�ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQryPosition(DFITCStockRspQryPositionField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-�ʽ��˺Ų�ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ʽ��˺Ų�ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,�����ʽ��˺Ų�ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�������ʽ��˺Ų�ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQryCapitalAccountInfo(DFITCStockRspQryCapitalAccountField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-�����˺Ų�ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û������˺Ų�ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,���������˺Ų�ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�����������˺Ų�ѯ����ʧ��
    */
    virtual void OnRspStockQryAccountInfo(DFITCStockRspQryAccountField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-�ɶ��Ų�ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ɶ��Ų�ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,�����ɶ��Ų�ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�������ɶ��Ų�ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQryShareholderInfo(DFITCStockRspQryShareholderField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-�ʽ������Ӧ
    * @param pData:ָ�����ǿ�,�����û��ʽ������Ӧ��Ϣ�ṹ��ĵ�ַ,�������ʽ��������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�������ʽ��������ʧ��
    */
    virtual void OnRspStockTransferFunds(DFITCStockRspTransferFundsField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-����ί����Ӧ
    * @param pData:ָ�����ǿ�,�����û�����ί����Ӧ��Ϣ�ṹ��ĵ�ַ,��������ί������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ����������ί������ʧ��
    */
    virtual void OnRspStockEntrustBatchOrder(DFITCStockRspEntrustBatchOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-����������Ӧ
    * @param pData:ָ�����ǿ�,�����û�����������Ӧ��Ϣ�ṹ��ĵ�ַ,����������������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ������������������ʧ��
    */
    virtual void OnRspStockWithdrawBatchOrder(DFITCStockRspWithdrawBatchOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-�����ί��������Ӧ
    * @param pData:ָ�����ǿ�,�����û������ί��������Ӧ��Ϣ�ṹ��ĵ�ַ,���������ί����������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�����������ί����������ʧ��
    */
    virtual void OnRspStockCalcAbleEntrustQty(DFITCStockRspCalcAbleEntrustQtyField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-�����깺ETF������Ӧ
    * @param pData:ָ�����ǿ�,�����û������깺ETF������Ӧ��Ϣ�ṹ��ĵ�ַ,���������깺ETF��������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�����������깺ETF��������ʧ��
    */
    virtual void OnRspStockCalcAblePurchaseETFQty(DFITCStockRspCalcAblePurchaseETFQtyField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-�����ʽ���ϸ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û������ʽ���ϸ��ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,���������ʽ���ϸ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�����������ʽ���ϸ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQryFreezeFundsDetail(DFITCStockRspQryFreezeFundsDetailField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-����֤ȯ��ϸ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û�����֤ȯ��ϸ��ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,��������֤ȯ��ϸ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ����������֤ȯ��ϸ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQryFreezeStockDetail(DFITCStockRspQryFreezeStockDetailField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-����֤ȯ��ϸ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û�����֤ȯ��ϸ��ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,��������֤ȯ��ϸ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ����������֤ȯ��ϸ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQryTransferStockDetail(DFITCStockRspQryTransferStockDetailField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-�����ʽ���ϸ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û������ʽ���ϸ��ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,���������ʽ���ϸ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ�����������ʽ���ϸ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQryTransferFundsDetail(DFITCStockRspQryTransferFundsDetailField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-֤ȯ��Ϣ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û�֤ȯ��Ϣ��ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,����֤ȯ��Ϣ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ������֤ȯ��Ϣ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQryStockInfo(DFITCStockRspQryStockField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-֤ȯ��̬��Ϣ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û�֤ȯ��̬��Ϣ��ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,����֤ȯ��̬��Ϣ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ������֤ȯ��̬��Ϣ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspStockQryStockStaticInfo(DFITCStockRspQryStockStaticField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * STOCK-����ʱ���ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û�����ʱ���ѯ��Ӧ��Ϣ�ṹ��ĵ�ַ,��������ʱ���ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ�ṹ��ĵ�ַ����������ʱ���ѯ����ʧ��
    */
    virtual void OnRspStockQryTradeTime(DFITCStockRspQryTradeTimeField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * STOCK-ί�лر���Ӧ
    * @param pData:����ί�лر��ṹ��ĵ�ַ
    */
    virtual void OnStockEntrustOrderRtn(DFITCStockEntrustOrderRtnField* pData);
    /**
    * STOCK-�ɽ��ر���Ӧ
    * @param pData:���سɽ��ر��ṹ��ĵ�ַ
    */
    virtual void OnStockTradeRtn(DFITCStockTradeRtnField* pData);
    /**
    * STOCK-�����ر���Ӧ
    * @param pData:���س����ر��ṹ��ĵ�ַ
    */
    virtual void OnStockWithdrawOrderRtn(DFITCStockWithdrawOrderRtnField* pData);

    /**
    * SOP-��¼��Ӧ
    * @param pRspUserLogin:ָ�����ǿ�,�����û���¼��Ӧ��Ϣ�ṹ��ַ,������¼����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ��������¼����ʧ��
    */
    virtual void OnRspSOPUserLogin(DFITCSECRspUserLoginField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
     * SOP-�ǳ���Ӧ
     * @param pData:ָ�����ǿ�,�����û��ǳ���Ӧ��Ϣ�ṹ��ַ,�����ǳ�����ɹ�
     * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ǳ�����ʧ��
     */
    virtual void OnRspSOPUserLogout(DFITCSECRspUserLogoutField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * SOP-�û����������Ӧ
    * @param pData:ָ�����ǿ�,�����û����������Ӧ��Ϣ�ṹ��ַ,�����û������������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������û������������ʧ��
    */
    virtual void OnRspSOPUserPasswordUpdate(DFITCSECRspPasswordUpdateField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * SOP-������Ӧ
    * @param pData:ָ�����ǿ�,�����û�������Ӧ��Ϣ�ṹ��ַ,������������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ��������������ʧ��
    */
    virtual void OnRspSOPEntrustOrder(DFITCSOPRspEntrustOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * SOP-�����̱�����Ӧ
    * @param pData:ָ�����ǿ�,�����û�����ί����Ӧ��Ϣ�ṹ��ַ,���������̱�������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�����������̱�������ʧ��
    */
    virtual void OnRspSOPQuoteEntrustOrder(DFITCSOPRspQuoteEntrustOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * SOP-��ϲ��ί����Ӧ
    * @param pData:ָ�����ǿ�,�����û���ϲ��ί����Ӧ��Ϣ�ṹ��ַ,������ϲ��ί������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ��������ϲ��ί������ʧ��
    */
    virtual void OnRspSOPGroupSplit(DFITCSOPRspEntrustOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
     * SOP-��Ȩ�����Ȩί����Ӧ
     * @param pData:ָ�����ǿ�,�����û���Ȩ�����Ȩί����Ӧ��Ϣ�ṹ��ַ,������Ȩ�����Ȩί������ɹ�
     * @return pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ��������Ȩ�����Ȩί������ʧ�ܣ�������������error.xml
     */
    virtual void OnRspSOPGroupExectueOrder(DFITCSOPRspGroupExectueOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * SOP-��ѯ�ͻ���ϳֲ���ϸ��Ӧ
    * @param pData:ָ�����ǿ�,�����û���ѯ�ͻ���ϳֲ���ϸ��Ӧ�ṹ��ַ,������ѯ�ͻ���ϳֲ���ϸ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ��������ѯ�ͻ���ϳֲ���ϸ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryGroupPosition(DFITCSOPRspQryGroupPositionField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-֤ȯ����������Ӧ
    * @param pData:ָ�����ǿ�,�����û�֤ȯ����������Ӧ��Ϣ�ṹ��ַ,����֤ȯ������������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ������֤ȯ������������ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPLockOUnLockStock(DFITCSOPRspLockOUnLockStockField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-������Ӧ
    * @param pData:ָ�����ǿ�,�����û�������Ӧ��Ϣ�ṹ��ַ,������������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ��������������ʧ��
    */
    virtual void OnRspSOPWithdrawOrder(DFITCSECRspWithdrawOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * SOP-ί�в�ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û�ί�в�ѯ��Ӧ��Ϣ�ṹ��ַ,����ί�в�ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ������ί�в�ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryEntrustOrder(DFITCSOPRspQryEntrustOrderField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-�ֱʳɽ���ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ֱʳɽ���ѯ��Ӧ��Ϣ�ṹ��ַ,�����ֱʳɽ���ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ֱʳɽ���ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQrySerialTrade(DFITCSOPRspQrySerialTradeField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-�ֲֲ�ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ֲֲ�ѯ��Ӧ��Ϣ�ṹ��ַ,�����ֲֲ�ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ֲֲ�ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryPosition(DFITCSOPRspQryPositionField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-�ͻ������ֲֲ�ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ͻ������ֲֲ�ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ������ֲֲ�ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ������ֲֲ�ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryCollateralPosition(DFITCSOPRspQryCollateralPositionField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-�ͻ��ʽ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ͻ��ʽ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ͻ��ʽ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ͻ��ʽ��ѯ����ʧ��
    */
    virtual void OnRspSOPQryCapitalAccountInfo(DFITCSOPRspQryCapitalAccountField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * SOP-�ͻ���Ϣ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ͻ���Ϣ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ͻ���Ϣ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ͻ���Ϣ��ѯ����ʧ��
    */
    virtual void OnRspSOPQryAccountInfo(DFITCSOPRspQryAccountField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * SOP-�ɶ���Ϣ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ɶ���Ϣ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ɶ���Ϣ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ɶ���Ϣ��ѯ����ʧ��
    */
    virtual void OnRspSOPQryShareholderInfo(DFITCSOPRspQryShareholderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * SOP-��ί��������ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û���ί��������ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ���ί��������ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ���ί��������ѯ����ʧ��
    */
    virtual void OnRspSOPCalcAbleEntrustQty(DFITCSOPRspCalcAbleEntrustQtyField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * SOP-�ͻ�������֤ȯ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ͻ�������֤ȯ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ͻ�������֤ȯ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ͻ�������֤ȯ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryAbleLockStock(DFITCSOPRspQryAbleLockStockField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-��Ȩ��Լ�����ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û���Ȩ��Լ�����ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ���Ȩ��Լ�����ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ���Ȩ��Լ�����ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryContactInfo(DFITCSOPRspQryContactField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-ִ��ί����Ӧ
    * @param pData:ָ�����ǿ�,�����û�ִ��ί����Ӧ��Ϣ�ṹ��ַ,�����ͻ�ִ��ί������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ�ִ��ί������ʧ��
    */
    virtual void OnRspSOPExectueOrder(DFITCSOPRspExectueOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * SOP-�ͻ���Ȩָ����Ϣ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ͻ���Ȩָ����Ϣ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ͻ���Ȩָ����Ϣ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ͻ���Ȩָ����Ϣ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryExecAssiInfo(DFITCSOPRspQryExecAssiInfoField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-��ѯ����ʱ����Ӧ
    * @param pData:ָ�����ǿ�,�����û���ѯ����ʱ����Ӧ��Ϣ�ṹ��ַ,�����ͻ���ѯ����ʱ������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ���ѯ����ʱ������ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryTradeTime(DFITCSOPRspQryTradeTimeField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-��ȡ���н�����������Ӧ
    * @param pData:ָ�����ǿ�,�����û���ȡ���н�����������Ӧ��Ϣ�ṹ��ַ,�����ͻ���ȡ���н�������������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ���ȡ���н�������������ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryExchangeInfo(DFITCSOPRspQryExchangeInfoField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-��ѯ�����Ѳ�����Ӧ
    * @param pData:ָ�����ǿ�,�����û���ѯ�����Ѳ�����Ӧ��Ϣ�ṹ��ַ,�����ͻ���ѯ�����Ѳ�������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ���ѯ�����Ѳ�������ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryCommission(DFITCSOPRspQryCommissionField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-��ѯ��֤���ʲ�����Ӧ
    * @param pData:ָ�����ǿ�,�����û���ѯ��֤���ʲ�����Ӧ��Ϣ�ṹ��ַ,�����ͻ���ѯ��֤���ʲ�������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ���ѯ��֤���ʲ�������ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryDeposit(DFITCSOPRspQryDepositField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-��Ȩ�����Ϣ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û���Ȩ�����Ϣ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ���Ȩ�����Ϣ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ���Ȩ�����Ϣ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspSOPQryContractObjectInfo(DFITCSOPRspQryContractObjectField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * SOP-ί�лر���Ӧ
    * @param pData:����ί�лر��ṹ��ĵ�ַ
    */
    virtual void OnSOPEntrustOrderRtn(DFITCSOPEntrustOrderRtnField* pData);
    /**
    * SOP-�ɽ��ر���Ӧ
    * @param pData:���سɽ��ر��ṹ��ĵ�ַ
    */
    virtual void OnSOPTradeRtn(DFITCSOPTradeRtnField* pData);
    /**
    * SOP-�����ر���Ӧ
    * @param pData:���س����ر��ṹ��ĵ�ַ
    */
    virtual void OnSOPWithdrawOrderRtn(DFITCSOPWithdrawOrderRtnField* pData);

    /**
    * SOP-�ʽ���������Ӧ
    * @param pData:ָ�����ǿ�,�����ʽ���������Ӧ��Ϣ�ṹ��ַ,�����ͻ��ʽ�����������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ʽ�����������ʧ��
    */
    virtual void OnRspSOPCapitalTranInOut(DFITCSOPRspCapitalTranInOutField* pData, DFITCSECRspInfoField* pRspInfo);

    /**
    * FASL-��¼��Ӧ
    * @param pData:ָ�����ǿ�,�����û�������ȯ��¼��Ӧ��Ϣ�ṹ��ַ,�����ͻ�������ȯ��¼����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ�������ȯ��¼����ʧ��
    */
    virtual void OnRspFASLUserLogin(DFITCSECRspUserLoginField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-�ǳ���Ӧ
    * @param pData:ָ�����ǿ�,�����û�������ȯ�ǳ���Ӧ��Ϣ�ṹ��ַ,�����ͻ�������ȯ�ǳ�����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ�������ȯ�ǳ�����ʧ��
    */
    virtual void OnRspFASLUserLogout(DFITCSECRspUserLogoutField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-�ͻ���������Ϣ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ͻ���������Ϣ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ͻ���������Ϣ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ͻ���������Ϣ����ʧ��
    */
    virtual void OnRspFASLQryAbleFinInfo(DFITCFASLRspAbleFinInfoField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-�ͻ�����ȯ��Ϣ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ͻ�����ȯ��Ϣ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ͻ�����ȯ��Ϣ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ͻ�����ȯ��Ϣ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryAbleSloInfo(DFITCFASLRspAbleSloInfoField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-�����ﻮת��Ӧ
    * @param pData:ָ�����ǿ�,�����û������ﻮת��Ӧ��Ϣ�ṹ��ַ,�����ͻ������ﻮת����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ������ﻮת����ʧ��
    */
    virtual void OnRspFASLTransferCollateral(DFITCFASLRspTransferCollateralField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-ֱ�ӻ�����Ӧ
    * @param pData:ָ�����ǿ�,�����û�ֱ�ӻ�����Ӧ��Ϣ�ṹ��ַ,�����ͻ�ֱ�ӻ�������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ�ֱ�ӻ�������ʧ��
    */
    virtual void OnRspFASLDirectRepayment(DFITCFASLRspDirectRepaymentField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-��ȯ��ת��Ӧ
    * @param pData:ָ�����ǿ�,�����û���ȯ��ת��Ӧ��Ϣ�ṹ��ַ,�����ͻ���ȯ��ת����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ���ȯ��ת����ʧ��
    */
    virtual void OnRspFASLRepayStockTransfer(DFITCFASLRspRepayStockTransferField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-���ý�����Ӧ
    * @param pData:ָ�����ǿ�,�����û����ý�����Ӧ��Ϣ�ṹ��ַ,�����ͻ����ý�������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ����ý�������ʧ��
    */
    virtual void OnRspFASLEntrustCrdtOrder(DFITCFASLRspEntrustCrdtOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-������ȯ������Ӧ
    * @param pData:ָ�����ǿ�,�����û�������ȯ������Ӧ��Ϣ�ṹ��ַ,�����ͻ�������ȯ��������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ�������ȯ��������ʧ��
    */
    virtual void OnRspFASLEntrustOrder(DFITCFASLRspEntrustOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-���ÿ�ί��������ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û����ÿ�ί��������ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ����ÿ�ί��������ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ����ÿ�ί��������ѯ����ʧ��
    */
    virtual void OnRspFASLCalcAbleEntrustCrdtQty(DFITCFASLRspCalcAbleEntrustCrdtQtyField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-��ѯ�����ʽ���Ӧ
    * @param pData:ָ�����ǿ�,�����û���ѯ�����ʽ���Ӧ��Ϣ�ṹ��ַ,�����ͻ���ѯ�����ʽ�����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ���ѯ�����ʽ�����ʧ��
    */
    virtual void OnRspFASLQryCrdtFunds(DFITCFASLRspQryCrdtFundsField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-���ú�Լ��Ϣ��Ӧ
    * @param pData:ָ�����ǿ�,�����û����ú�Լ��Ϣ��Ӧ��Ϣ�ṹ��ַ,�����ͻ����ú�Լ��Ϣ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ����ú�Լ��Ϣ����ʧ��
    */
    virtual void OnRspFASLQryCrdtContract(DFITCFASLRspQryCrdtContractField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLat);
    /**
    * FASL-���ú�Լ�䶯��Ϣ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û����ú�Լ�䶯��Ϣ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ����ú�Լ�䶯��Ϣ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ����ú�Լ�䶯��Ϣ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryCrdtConChangeInfo(DFITCFASLRspQryCrdtConChangeInfoField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-�ʽ��ת��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ʽ��ת��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ʽ��ת����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ʽ��ת����ʧ��
    */
    virtual void OnRspFASLTransferFunds(DFITCStockRspTransferFundsField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-�ͻ���Ϣ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ͻ���Ϣ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ͻ���Ϣ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ͻ���Ϣ��ѯ����ʧ��
    */
    virtual void OnRspFASLQryAccountInfo(DFITCStockRspQryAccountField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-�ͻ��ʽ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ͻ��ʽ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ͻ��ʽ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ͻ��ʽ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryCapitalAccountInfo(DFITCStockRspQryCapitalAccountField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-�ɶ���Ϣ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ɶ���Ϣ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ɶ���Ϣ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ɶ���Ϣ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryShareholderInfo(DFITCStockRspQryShareholderField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-�ֲֲ�ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ֲֲ�ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ֲֲ�ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ֲֲ�ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryPosition(DFITCStockRspQryPositionField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-ί�в�ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û�ί�в�ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ�ί�в�ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ�ί�в�ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryEntrustOrder(DFITCStockRspQryEntrustOrderField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-�ֱʳɽ���ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ֱʳɽ���ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ֱʳɽ���ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ֱʳɽ���ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQrySerialTrade(DFITCStockRspQrySerialTradeField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-ʵʱ�ɽ���ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û�ʵʱ�ɽ���ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ�ʵʱ�ɽ���ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ�ʵʱ�ɽ���ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryRealTimeTrade(DFITCStockRspQryRealTimeTradeField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-�ʽ𶳽���ϸ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ʽ𶳽���ϸ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ʽ𶳽���ϸ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ʽ𶳽���ϸ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryFreezeFundsDetail(DFITCStockRspQryFreezeFundsDetailField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-֤ȯ������ϸ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û�֤ȯ������ϸ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ�֤ȯ������ϸ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ�֤ȯ������ϸ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryFreezeStockDetail(DFITCStockRspQryFreezeStockDetailField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-�ʽ������ϸ��ѯ��Ӧ
    * @param pData:ָ�����ǿ�,�����û��ʽ������ϸ��ѯ��Ӧ��Ϣ�ṹ��ַ,�����ͻ��ʽ������ϸ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ��ʽ������ϸ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryTransferFundsDetail(DFITCStockRspQryTransferFundsDetailField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-������Ӧ
    * @param pData:ָ�����ǿ�,�����û�������Ӧ��Ϣ�ṹ��ַ,������������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ��������������ʧ��
    */
    virtual void OnRspFASLWithdrawOrder(DFITCFASLRspWithdrawOrderField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-��ǰϵͳʱ���ѯ������Ӧ
    * @param pData:ָ�����ǿ�,�����û�ϵͳʱ���ѯ��Ӧ��Ϣ�ṹ��ַ,����ϵͳʱ���ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ������ϵͳʱ���ѯ����ʧ��
    */
    virtual void OnRspFASLQrySystemTime(DFITCFASLRspQryTradeTimeField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-��ת�뵣��֤ȯ��ѯ������Ӧ
    * @param pData:ָ�����ǿ�,���ؿ�ת�뵣��֤ȯ��ѯ��Ӧ��Ϣ�ṹ��ַ,������ת�뵣��֤ȯ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ��������ת�뵣��֤ȯ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryTransferredContract(DFITCFASLRspQryTransferredContractField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-�ͻ���ȡ�ʽ����������Ӧ
    * @param pData:ָ�����ǿ�,���ؿͻ���ȡ�ʽ������Ӧ��Ϣ�ṹ��ַ,�����ͻ���ȡ�ʽ��������ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ�������ͻ���ȡ�ʽ��������ʧ��
    */
    virtual void OnRspFASLDesirableFundsOut(DFITCFASLRspDesirableFundsOutField* pData, DFITCSECRspInfoField* pRspInfo);
    /**
    * FASL-����֤ȯ��ѯ������Ӧ
    * @param pData:ָ�����ǿ�,���ص���֤ȯ��ѯ��Ӧ��Ϣ�ṹ��ַ,��������֤ȯ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ����������֤ȯ��ѯ����ʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryGuaranteedContract(DFITCFASLRspQryGuaranteedContractField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-���֤ȯ��ѯ������Ӧ
    * @param pData:ָ�����ǿ�,���ر��֤ȯ��ѯ��Ӧ��Ϣ�ṹ��ַ,�������֤ȯ��ѯ����ɹ�
    * @param pRspInfo:ָ�����ǿգ����ش�����Ϣ��ַ���������֤ȯ��ѯʧ��
    * @param bIsLast:����ֵ�����Ƿ������һ����Ӧ��Ϣ(0-��,1-��)
    */
    virtual void OnRspFASLQryUnderlyingContract(DFITCFASLRspQryUnderlyingContractField* pData, DFITCSECRspInfoField* pRspInfo, bool bIsLast);
    /**
    * FASL-ί�лر���Ӧ
    * @param pData:����ί�лر��ṹ��ĵ�ַ
    */
    virtual void OnFASLEntrustOrderRtn(DFITCStockEntrustOrderRtnField* pData);
    /**
    * FASL-�ɽ��ر���Ӧ
    * @param pData:���سɽ��ر��ṹ��ĵ�ַ
    */
    virtual void OnFASLTradeRtn(DFITCStockTradeRtnField* pData);
    /**
    * FASL-�����ر���Ӧ
    * @param pData:���س����ر��ṹ��ĵ�ַ
    */
    virtual void OnFASLWithdrawOrderRtn(DFITCStockWithdrawOrderRtnField* pData);

	//-------------------------------------------------------------------------------------
	//task������
	//-------------------------------------------------------------------------------------
	void processTask();

	void processFrontConnected(Task *task);

	void processFrontDisconnected(Task *task);

	void processRtnNotice(Task *task);

	void processRspError(Task *task);

	void processRspStockUserLogin(Task *task);

	void processRspStockUserLogout(Task *task);

	void processRspStockUserPasswordUpdate(Task *task);

	void processRspStockEntrustOrder(Task *task);

	void processRspStockWithdrawOrder(Task *task);

	void processRspStockQryEntrustOrder(Task *task);

	void processRspStockQryRealTimeTrade(Task *task);

	void processRspStockQrySerialTrade(Task *task);

	void processRspStockQryPosition(Task *task);

	void processRspStockQryCapitalAccountInfo(Task *task);

	void processRspStockQryAccountInfo(Task *task);

	void processRspStockQryShareholderInfo(Task *task);

	void processRspStockTransferFunds(Task *task);

	void processRspStockEntrustBatchOrder(Task *task);

	void processRspStockWithdrawBatchOrder(Task *task);

	void processRspStockCalcAbleEntrustQty(Task *task);

	void processRspStockCalcAblePurchaseETFQty(Task *task);

	void processRspStockQryFreezeFundsDetail(Task *task);

	void processRspStockQryFreezeStockDetail(Task *task);

	void processRspStockQryTransferStockDetail(Task *task);

	void processRspStockQryTransferFundsDetail(Task *task);

	void processRspStockQryStockInfo(Task *task);

	void processRspStockQryStockStaticInfo(Task *task);

	void processRspStockQryTradeTime(Task *task);

	void processStockEntrustOrderRtn(Task *task);

	void processStockTradeRtn(Task *task);

	void processStockWithdrawOrderRtn(Task *task);

	void processRspSOPUserLogin(Task *task);

	void processRspSOPUserLogout(Task *task);

	void processRspSOPUserPasswordUpdate(Task *task);

	void processRspSOPEntrustOrder(Task *task);

	void processRspSOPQuoteEntrustOrder(Task *task);

	void processRspSOPGroupSplit(Task *task);

    void processRspSOPGroupExectueOrder(Task* task);

	void processRspSOPQryGroupPosition(Task *task);

	void processRspSOPLockOUnLockStock(Task *task);

	void processRspSOPWithdrawOrder(Task *task);

	void processRspSOPQryEntrustOrder(Task *task);

	void processRspSOPQrySerialTrade(Task *task);

	void processRspSOPQryPosition(Task *task);

	void processRspSOPQryCollateralPosition(Task *task);

	void processRspSOPQryCapitalAccountInfo(Task *task);

	void processRspSOPQryAccountInfo(Task *task);

	void processRspSOPQryShareholderInfo(Task *task);

	void processRspSOPCalcAbleEntrustQty(Task *task);

	void processRspSOPQryAbleLockStock(Task *task);

	void processRspSOPQryContactInfo(Task *task);

	void processRspSOPExectueOrder(Task *task);

	void processRspSOPQryExecAssiInfo(Task *task);

	void processRspSOPQryTradeTime(Task *task);

	void processRspSOPQryExchangeInfo(Task *task);

	void processRspSOPQryCommission(Task *task);

	void processRspSOPQryDeposit(Task *task);

	void processRspSOPQryContractObjectInfo(Task *task);

	void processSOPEntrustOrderRtn(Task *task);

	void processSOPTradeRtn(Task *task);

	void processSOPWithdrawOrderRtn(Task *task);

    void processRspSOPCapitalTranInOut(Task* task);

	void processRspFASLUserLogin(Task *task);

	void processRspFASLUserLogout(Task *task);

	void processRspFASLQryAbleFinInfo(Task *task);

	void processRspFASLQryAbleSloInfo(Task *task);

	void processRspFASLTransferCollateral(Task *task);

	void processRspFASLDirectRepayment(Task *task);

	void processRspFASLRepayStockTransfer(Task *task);

	void processRspFASLEntrustCrdtOrder(Task *task);

	void processRspFASLEntrustOrder(Task *task);

	void processRspFASLCalcAbleEntrustCrdtQty(Task *task);

	void processRspFASLQryCrdtFunds(Task *task);

	void processRspFASLQryCrdtContract(Task *task);

	void processRspFASLQryCrdtConChangeInfo(Task *task);

	void processRspFASLTransferFunds(Task *task);

	void processRspFASLQryAccountInfo(Task *task);

	void processRspFASLQryCapitalAccountInfo(Task *task);

	void processRspFASLQryShareholderInfo(Task *task);

	void processRspFASLQryPosition(Task *task);

	void processRspFASLQryEntrustOrder(Task *task);

	void processRspFASLQrySerialTrade(Task *task);

	void processRspFASLQryRealTimeTrade(Task *task);

	void processRspFASLQryFreezeFundsDetail(Task *task);

	void processRspFASLQryFreezeStockDetail(Task *task);

	void processRspFASLQryTransferFundsDetail(Task *task);

	void processRspFASLWithdrawOrder(Task *task);

	void processRspFASLQrySystemTime(Task *task);

	void processRspFASLQryTransferredContract(Task *task);

	void processRspFASLDesirableFundsOut(Task *task);

	void processRspFASLQryGuaranteedContract(Task *task);

	void processRspFASLQryUnderlyingContract(Task *task);

	void processFASLEntrustOrderRtn(Task *task);

	void processFASLTradeRtn(Task *task);

	void processFASLWithdrawOrderRtn(Task *task);

	//-------------------------------------------------------------------------------------
	//data���ص������������ֵ�
	//error���ص������Ĵ����ֵ�
	//id������id
	//last���Ƿ�Ϊ��󷵻�
	//i������
	//-------------------------------------------------------------------------------------

	virtual void onFrontConnected() {};

	virtual void onFrontDisconnected(int reqid) {};

	virtual void onRtnNotice(const dict &data) {};

	virtual void onRspError(const dict &error) {};

	virtual void onRspStockUserLogin(const dict &data, const dict &error) {};

	virtual void onRspStockUserLogout(const dict &data, const dict &error) {};

	virtual void onRspStockUserPasswordUpdate(const dict &data, const dict &error) {};

	virtual void onRspStockEntrustOrder(const dict &data, const dict &error) {};

	virtual void onRspStockWithdrawOrder(const dict &data, const dict &error) {};

	virtual void onRspStockQryEntrustOrder(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockQryRealTimeTrade(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockQrySerialTrade(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockQryPosition(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockQryCapitalAccountInfo(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockQryAccountInfo(const dict &data, const dict &error) {};

	virtual void onRspStockQryShareholderInfo(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockTransferFunds(const dict &data, const dict &error) {};

	virtual void onRspStockEntrustBatchOrder(const dict &data, const dict &error) {};

	virtual void onRspStockWithdrawBatchOrder(const dict &data, const dict &error) {};

	virtual void onRspStockCalcAbleEntrustQty(const dict &data, const dict &error) {};

	virtual void onRspStockCalcAblePurchaseETFQty(const dict &data, const dict &error) {};

	virtual void onRspStockQryFreezeFundsDetail(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockQryFreezeStockDetail(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockQryTransferStockDetail(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockQryTransferFundsDetail(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockQryStockInfo(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockQryStockStaticInfo(const dict &data, const dict &error, bool last) {};

	virtual void onRspStockQryTradeTime(const dict &data, const dict &error) {};

	virtual void onStockEntrustOrderRtn(const dict &data) {};

	virtual void onStockTradeRtn(const dict &data) {};

	virtual void onStockWithdrawOrderRtn(const dict &data) {};

	virtual void onRspSOPUserLogin(const dict &data, const dict &error) {};

	virtual void onRspSOPUserLogout(const dict &data, const dict &error) {};

	virtual void onRspSOPUserPasswordUpdate(const dict &data, const dict &error) {};

	virtual void onRspSOPEntrustOrder(const dict &data, const dict &error) {};

	virtual void onRspSOPQuoteEntrustOrder(const dict &data, const dict &error) {};

	virtual void onRspSOPGroupSplit(const dict &data, const dict &error) {};

    virtual void onRspSOPGroupExectueOrder(const dict& data, const dict& error) {};

	virtual void onRspSOPQryGroupPosition(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPLockOUnLockStock(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPWithdrawOrder(const dict &data, const dict &error) {};

	virtual void onRspSOPQryEntrustOrder(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPQrySerialTrade(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPQryPosition(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPQryCollateralPosition(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPQryCapitalAccountInfo(const dict &data, const dict &error) {};

	virtual void onRspSOPQryAccountInfo(const dict &data, const dict &error) {};

	virtual void onRspSOPQryShareholderInfo(const dict &data, const dict &error) {};

	virtual void onRspSOPCalcAbleEntrustQty(const dict &data, const dict &error) {};

	virtual void onRspSOPQryAbleLockStock(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPQryContactInfo(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPExectueOrder(const dict &data, const dict &error) {};

	virtual void onRspSOPQryExecAssiInfo(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPQryTradeTime(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPQryExchangeInfo(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPQryCommission(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPQryDeposit(const dict &data, const dict &error, bool last) {};

	virtual void onRspSOPQryContractObjectInfo(const dict &data, const dict &error, bool last) {};

	virtual void onSOPEntrustOrderRtn(const dict &data) {};

	virtual void onSOPTradeRtn(const dict &data) {};

	virtual void onSOPWithdrawOrderRtn(const dict &data) {};

    virtual void onRspSOPCapitalTranInOut(const dict& data, const dict& error) {};

	virtual void onRspFASLUserLogin(const dict &data, const dict &error) {};

	virtual void onRspFASLUserLogout(const dict &data, const dict &error) {};

	virtual void onRspFASLQryAbleFinInfo(const dict &data, const dict &error) {};

	virtual void onRspFASLQryAbleSloInfo(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLTransferCollateral(const dict &data, const dict &error) {};

	virtual void onRspFASLDirectRepayment(const dict &data, const dict &error) {};

	virtual void onRspFASLRepayStockTransfer(const dict &data, const dict &error) {};

	virtual void onRspFASLEntrustCrdtOrder(const dict &data, const dict &error) {};

	virtual void onRspFASLEntrustOrder(const dict &data, const dict &error) {};

	virtual void onRspFASLCalcAbleEntrustCrdtQty(const dict &data, const dict &error) {};

	virtual void onRspFASLQryCrdtFunds(const dict &data, const dict &error) {};

	virtual void onRspFASLQryCrdtContract(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLQryCrdtConChangeInfo(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLTransferFunds(const dict &data, const dict &error) {};

	virtual void onRspFASLQryAccountInfo(const dict &data, const dict &error) {};

	virtual void onRspFASLQryCapitalAccountInfo(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLQryShareholderInfo(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLQryPosition(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLQryEntrustOrder(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLQrySerialTrade(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLQryRealTimeTrade(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLQryFreezeFundsDetail(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLQryFreezeStockDetail(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLQryTransferFundsDetail(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLWithdrawOrder(const dict &data, const dict &error) {};

	virtual void onRspFASLQrySystemTime(const dict &data, const dict &error) {};

	virtual void onRspFASLQryTransferredContract(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLDesirableFundsOut(const dict &data, const dict &error) {};

	virtual void onRspFASLQryGuaranteedContract(const dict &data, const dict &error, bool last) {};

	virtual void onRspFASLQryUnderlyingContract(const dict &data, const dict &error, bool last) {};

	virtual void onFASLEntrustOrderRtn(const dict &data) {};

	virtual void onFASLTradeRtn(const dict &data) {};

	virtual void onFASLWithdrawOrderRtn(const dict &data) {};

	//-------------------------------------------------------------------------------------
	//req:���������������ֵ�
	//-------------------------------------------------------------------------------------

	void createDFITCSECTraderApi(string pszFlowPath);

	void release();

	int init(string protocol);

	int exit();

	int subscribePrivateTopic(int nResumeType);

	int reqStockUserLogin(const dict &req);

	int reqStockUserLogout(const dict &req);

	int reqStockUserPasswordUpdate(const dict &req);

	int reqStockEntrustOrder(const dict &req);

	int reqStockWithdrawOrder(const dict &req);

	int reqStockQryEntrustOrder(const dict &req);

	int reqStockQryRealTimeTrade(const dict &req);

	int reqStockQrySerialTrade(const dict &req);

	int reqStockQryPosition(const dict &req);

	int reqStockQryCapitalAccountInfo(const dict &req);

	int reqStockQryAccountInfo(const dict &req);

	int reqStockQryShareholderInfo(const dict &req);

	int reqStockTransferFunds(const dict &req);

	int reqStockEntrustBatchOrder(const dict &req);

	int reqStockWithdrawBatchOrder(const dict &req);

	int reqStockCalcAbleEntrustQty(const dict &req);

	int reqStockCalcAblePurchaseETFQty(const dict &req);

	int reqStockQryFreezeFundsDetail(const dict &req);

	int reqStockQryFreezeStockDetail(const dict &req);

	int reqStockQryTransferFundsDetail(const dict &req);

	int reqStockQryTransferStockDetail(const dict &req);

	int reqStockQryStockInfo(const dict &req);

	int reqStockQryStockStaticInfo(const dict &req);

	int reqStockQryTradeTime(const dict &req);

	int reqSOPUserLogin(const dict &req);

	int reqSOPUserLogout(const dict &req);

	int reqSOPUserPasswordUpdate(const dict &req);

	int reqSOPEntrustOrder(const dict &req);

	int reqSOPQuoteEntrustOrder(const dict &req);

	int reqSOPGroupSplit(const dict &req);

    int reqSOPGroupExectueOrder(const dict& req);

	int reqSOPQryGroupPosition(const dict &req);

	int reqSOPLockOUnLockStock(const dict &req);

	int reqSOPWithdrawOrder(const dict &req);

	int reqSOPQryEntrustOrder(const dict &req);

	int reqSOPQrySerialTrade(const dict &req);

	int reqSOPQryPosition(const dict &req);

	int reqSOPQryCollateralPosition(const dict &req);

	int reqSOPQryCapitalAccountInfo(const dict &req);

	int reqSOPQryAccountInfo(const dict &req);

	int reqSOPQryShareholderInfo(const dict &req);

	int reqSOPCalcAbleEntrustQty(const dict &req);

	int reqSOPQryAbleLockStock(const dict &req);

	int reqSOPQryContactInfo(const dict &req);

	int reqSOPExectueOrder(const dict &req);

	int reqSOPQryExecAssiInfo(const dict &req);

	int reqSOPQryTradeTime(const dict &req);

	int reqSOPQryExchangeInfo(const dict &req);

	int reqSOPQryCommission(const dict &req);

	int reqSOPQryDeposit(const dict &req);

	int reqSOPQryContractObjectInfo(const dict &req);

    int reqSOPCapitalTranInOut(const dict& req);

	int reqFASLUserLogin(const dict &req);

	int reqFASLUserLogout(const dict &req);

	int reqFASLQryAbleFinInfo(const dict &req);

	int reqFASLQryAbleSloInfo(const dict &req);

	int reqFASLTransferCollateral(const dict &req);

	int reqFASLDirectRepayment(const dict &req);

	int reqFASLRepayStockTransfer(const dict &req);

	int reqFASLEntrustCrdtOrder(const dict &req);

	int reqFASLEntrsuctOrder(const dict &req);

	int reqFASLWithdrawOrder(const dict &req);

	int reqFASLCalcAbleEntrustCrdtQty(const dict &req);

	int reqFASLQryCrdtFunds(const dict &req);

	int reqFASLQryCrdtContract(const dict &req);

	int reqFASLQryCrdtConChangeInfo(const dict &req);

	int reqFASLTransferFunds(const dict &req);

	int reqFASLQryAccountInfo(const dict &req);

	int reqFASLQryCapitalAccountInfo(const dict &req);

	int reqFASLQryShareholderInfo(const dict &req);

	int reqFASLQryPosition(const dict &req);

	int reqFASLQryEntrustOrder(const dict &req);

	int reqFASLQrySerialTrade(const dict &req);

	int reqFASLQryRealTimeTrade(const dict &req);

	int reqFASLQryFreezeFundsDetail(const dict &req);

	int reqFASLQryFreezeStockDetail(const dict &req);

	int reqFASLQryTransferFundsDetail(const dict &req);

	int reqFASLQrySystemTime(const dict &req);

	int reqFASLQryTransferredContract(const dict &req);

	int reqFASLDesirableFundsOut(const dict &req);

	int reqFASLQryGuaranteedContract(const dict &req);

	int reqFASLQryUnderlyingContract(const dict &req);
};