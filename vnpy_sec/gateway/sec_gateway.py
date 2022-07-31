from typing import Any, Dict, List
from datetime import datetime
from copy import copy

from vnpy.event import EventEngine
from vnpy.trader.event import EVENT_TIMER
from vnpy.trader.constant import (
    Exchange,
    Product,
    Direction,
    OrderType,
    Status,
    Offset,
    OptionType
)
from vnpy.trader.gateway import BaseGateway
from vnpy.trader.object import (
    CancelRequest,
    OrderRequest,
    SubscribeRequest,
    TickData,
    ContractData,
    OrderData,
    TradeData,
    PositionData,
    AccountData
)
from vnpy.trader.utility import get_folder_path, ZoneInfo

from ..api import (
    MdApi,
    TdApi,
    DFITCSEC_EI_SH,
    DFITCSEC_EI_SZ,
    DFITCSEC_OT_LimitPrice,
    DFITCSEC_OT_SHBESTFRTradeLeftWithdraw,
    DFITCSEC_SOP_LimitPrice,
    DFITCSEC_SOP_LastPrice,
    DFITCSEC_OCF_Open,
    DFITCSEC_OCF_Close,
    DFITCSEC_ED_Buy,
    DFITCSEC_ED_Sell,
    DFITCSEC_OT_CALL,
    DFITCSEC_OT_PUT,
    DFITCSEC_COLLECTTYPE_APEX,
    DFITCSEC_COLLECTTYPE_HS,
    DFITCSEC_COLLECTTYPE_KD,
    DFITCSEC_COLLECTTYPE_KS,
    DFITCSEC_COMPRESS_TRUE,
    DFITCSEC_COMPRESS_FALSE
)


# 多空方向映射
DIRECTION_VT2SEC: Dict[Direction, int] = {
    Direction.LONG: DFITCSEC_ED_Buy,
    Direction.SHORT: DFITCSEC_ED_Sell
}
DIRECTION_SEC2VT: Dict[int, Direction] = {v: k for k, v in DIRECTION_VT2SEC.items()}

# 股票委托类型映射
STOCK_PRICE_TYPE_SEC2VT: Dict[int, OrderType] = {
    DFITCSEC_OT_LimitPrice: OrderType.LIMIT,
    DFITCSEC_OT_SHBESTFRTradeLeftWithdraw: OrderType.MARKET
}
STOCK_PRICE_TYPE_VT2SEC: Dict[OrderType, int] = {v: k for k, v in STOCK_PRICE_TYPE_SEC2VT.items()}

# 期权委托类型映射
OPTION_PRICE_TYPE_SEC2VT: Dict[int, OrderType] = {
    DFITCSEC_SOP_LimitPrice: OrderType.LIMIT,
    DFITCSEC_SOP_LastPrice: OrderType.MARKET
}
OPTION_PRICE_TYPE_VT2SEC: Dict[OrderType, int] = {v: k for k, v in OPTION_PRICE_TYPE_SEC2VT.items()}

# 开平方向映射
OFFSET_VT2SEC: Dict[Offset, int] = {
    Offset.OPEN: DFITCSEC_OCF_Open,
    Offset.CLOSE: DFITCSEC_OCF_Close,
}
OFFSET_SEC2VT: Dict[int, Offset] = {v: k for k, v in OFFSET_VT2SEC.items()}

# 交易所映射
EXCHANGE_SEC2VT: Dict[str, Exchange] = {
    DFITCSEC_EI_SH: Exchange.SSE,
    DFITCSEC_EI_SZ: Exchange.SZSE
}
EXCHANGE_VT2SEC: Dict[Exchange, int] = {v: k for k, v in EXCHANGE_SEC2VT.items()}

# 期权类型映射
OPTION_TYPE_SEC2VT: Dict[int, OptionType] = {
    DFITCSEC_OT_CALL: OptionType.CALL,
    DFITCSEC_OT_PUT: OptionType.PUT
}

# 对冲方向映射
HEDGE_DIRECTION: Dict[int, int] = {
    DFITCSEC_ED_Buy: 2,
    DFITCSEC_ED_Sell: 1
}

# 采集类型映射
COLLECTION_TYPE_VT2SEC: Dict[str, int] = {
    "顶点": DFITCSEC_COLLECTTYPE_APEX,
    "恒生": DFITCSEC_COLLECTTYPE_HS,
    "金证": DFITCSEC_COLLECTTYPE_KD,
    "金仕达": DFITCSEC_COLLECTTYPE_KS
}

# 行情压缩映射
COMPRESS_VT2SEC: Dict[str, int] = {
    "Y": DFITCSEC_COMPRESS_TRUE,
    "N": DFITCSEC_COMPRESS_FALSE
}

# 其他常量
CHINA_TZ = ZoneInfo("Asia/Shanghai")

# 合约数据全局缓存字典
symbol_contract_map: Dict[str, ContractData] = {}


class SecGateway(BaseGateway):
    """
    VeighNa用于对接顶点飞创证券柜台的交易接口。
    """

    default_name: str = "SEC"

    default_setting: Dict[str, Any] = {
        "账号": "",
        "行情密码": "",
        "交易密码": "",
        "行情地址": "",
        "交易地址": "",
        "行情协议": ["TCP", "UDP"],
        "授权码": "",
        "产品号": "",
        "采集类型": ["顶点", "恒生", "金证", "金仕达"],
        "行情压缩": ["N", "Y"],
    }

    exchanges: List[Exchange] = list(EXCHANGE_VT2SEC.keys())

    def __init__(self, event_engine: EventEngine, gateway_name: str) -> None:
        """构造函数"""
        super().__init__(event_engine, gateway_name)

        self.md_api: "SecMdApi" = SecMdApi(self)
        self.td_api: "SecTdApi" = SecTdApi(self)

    def connect(self, setting: dict) -> None:
        """连接交易接口"""
        accountid: str = setting["账号"]
        md_password: str = setting["行情密码"]
        td_password: str = setting["交易密码"]
        md_address: str = setting["行情地址"]
        td_address: str = setting["交易地址"]
        quote_protocol: str = setting["行情协议"]
        auth_code: str = setting["授权码"]
        appid: str = setting["产品号"]
        collection_type: int = COLLECTION_TYPE_VT2SEC[setting["采集类型"]]
        compress_flag: int = COMPRESS_VT2SEC[setting["行情压缩"]]

        if (
            (not md_address.startswith("tcp://"))
            and (not md_address.startswith("ssl://"))
        ):
            md_address = "tcp://" + md_address

        if (
            (not td_address.startswith("tcp://"))
            and (not td_address.startswith("ssl://"))
        ):
            td_address = "tcp://" + td_address

        if quote_protocol == "UDP":
            md_address = md_address.replace("tcp", "udp")

        self.md_api.connect(
            accountid,
            md_password,
            md_address,
            auth_code,
            appid,
            collection_type,
            compress_flag,
        )
        self.td_api.connect(
            accountid,
            td_password,
            td_address,
            auth_code,
            appid,
            collection_type,
        )
        self.init_query()

    def subscribe(self, req: SubscribeRequest) -> None:
        """订阅行情"""
        self.md_api.subscrbie(req)

    def send_order(self, req: OrderRequest) -> str:
        """委托下单"""
        return self.td_api.send_order(req)

    def cancel_order(self, req: CancelRequest) -> None:
        """委托撤单"""
        self.td_api.cancel_order(req)

    def query_account(self) -> None:
        """查询资金"""
        self.td_api.query_account()

    def query_position(self) -> None:
        """查询持仓"""
        self.td_api.query_position()

    def close(self) -> None:
        """关闭接口"""
        self.md_api.close()
        self.td_api.close()

    def write_error(self, msg: str, error: dict) -> None:
        """输出错误信息日志"""
        error_id: int = error["errorID"]
        error_msg: str = error["errorMsg"]
        msg: str = f"{msg}，代码：{error_id}，信息：{error_msg}"
        self.write_log(msg)

    def process_timer_event(self, event) -> None:
        """定时事件处理"""
        self.count += 1
        if self.count < 2:
            return
        self.count = 0

        func = self.query_functions.pop(0)
        func()
        self.query_functions.append(func)

    def init_query(self) -> None:
        """初始化查询任务"""
        self.count: int = 0
        self.query_functions: list = [self.query_account, self.query_position]
        self.event_engine.register(EVENT_TIMER, self.process_timer_event)


class SecMdApi(MdApi):

    def __init__(self, gateway: SecGateway):
        """构造函数"""
        super().__init__()

        self.gateway: SecGateway = gateway
        self.gateway_name: str = gateway.gateway_name

        self.accountid: str = ""
        self.password: str = ""
        self.md_address: str = ""

        self.connect_status: bool = False
        self.login_status: bool = False
        self.compress_flag: int = 1
        self.auth_code: str = ""
        self.app_id: str = ""
        self.collection_type: int = 1

        self.reqid: int = 0
        self.subscribed: set = set()

        self.sse_inited: bool = False
        self.szse_inited: bool = False

    def onFrontConnected(self):
        """服务器连接成功回报"""
        self.login_server()
        self.connect_status = True

    def onFrontDisconnected(self, reason: int) -> None:
        """服务器连接断开回报"""
        self.connect_status = False
        self.login_status = False
        self.login_status_option = False
        self.gateway.write_log(f"行情服务器连接断开, 原因{reason}")

    def onRspStockUserLogin(self, data: dict, error: dict) -> None:
        """用户登录股票行情服务器请求回报"""
        if not error:
            self.gateway.write_log("股票行情服务器登录成功")
        else:
            self.gateway.write_error("股票期权行情服务器登录失败,", error)

    def onRspSOPUserLogin(self, data: dict, error: dict) -> None:
        """用户登录股票期权行情服务器请求回报"""
        if not error:
            self.gateway.write_log("股票期权行情服务器登录成功")
            self.login_status = True
        else:
            self.gateway.write_error("股票期权行情服务器登录失败,", error)

    def onRspError(self, error: dict) -> None:
        """请求报错回报"""
        self.gateway.write_error("行情接口报错", error)

    def onRspStockSubMarketData(self, data: dict, error: dict) -> None:
        """股票行情订阅回报"""
        if not error or not error["errorID"]:
            return

        self.gateway.write_error("股票行情订阅失败", error)

    def onRspSOPSubMarketData(self, data: dict, error: dict) -> None:
        """股票期权行情订阅回报"""
        if not error or not error["errorID"]:
            return

        self.gateway.write_error("股票期权行情订阅失败", error)

    def onSOPMarketData(self, data: dict) -> None:
        """股票期权行情数据推送"""
        timestamp: str = str(data["tradingDay"]) + str(data["updateTime"])
        dt: datetime = generate_datetime(timestamp)

        tick: TickData = TickData(
            symbol=data["securityID"],
            exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
            datetime=dt,
            volume=data["tradeQty"],
            last_price=data["latestPrice"],
            limit_up=data["upperLimitPrice"],
            limit_down=data["lowerLimitPrice"],
            open_price=data["openPrice"],
            high_price=data["highestPrice"],
            low_price=data["lowestPrice"],
            pre_close=data["preClosePrice"],
            gateway_name=self.gateway_name
        )
        tick.bid_price_1 = data["bidPrice1"]
        tick.bid_price_2 = data["bidPrice2"]
        tick.bid_price_3 = data["bidPrice3"]
        tick.bid_price_4 = data["bidPrice4"]
        tick.bid_price_5 = data["bidPrice5"]

        tick.ask_price_1 = data["askPrice1"]
        tick.ask_price_2 = data["askPrice2"]
        tick.ask_price_3 = data["askPrice3"]
        tick.ask_price_4 = data["askPrice4"]
        tick.ask_price_5 = data["askPrice5"]

        tick.bid_volume_1 = data["bidQty1"]
        tick.bid_volume_2 = data["bidQty2"]
        tick.bid_volume_3 = data["bidQty3"]
        tick.bid_volume_4 = data["bidQty4"]
        tick.bid_volume_5 = data["bidQty5"]

        tick.ask_volume_1 = data["askQty1"]
        tick.ask_volume_2 = data["askQty2"]
        tick.ask_volume_3 = data["askQty3"]
        tick.ask_volume_4 = data["askQty4"]
        tick.ask_volume_5 = data["askQty5"]

        contract: ContractData = symbol_contract_map.get(tick.symbol, None)
        if contract:
            tick.name = contract.name

        self.gateway.on_tick(tick)

    def onStockMarketData(self, data: dict) -> None:
        """股票行情数据推送"""
        timestamp: str = str(data["tradingDay"]) + str(data["updateTime"])
        dt: datetime = generate_datetime(timestamp)

        tick: TickData = TickData(
            symbol=data["securityID"],
            exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
            datetime=dt,
            volume=data["tradeQty"],
            last_price=data["latestPrice"],
            limit_up=data["upperLimitPrice"],
            limit_down=data["lowerLimitPrice"],
            open_price=data["openPrice"],
            high_price=data["highestPrice"],
            low_price=data["lowestPrice"],
            pre_close=data["preClosePrice"],
            gateway_name=self.gateway_name
        )
        tick.bid_price_1 = data["bidPrice1"]
        tick.bid_price_2 = data["bidPrice2"]
        tick.bid_price_3 = data["bidPrice3"]
        tick.bid_price_4 = data["bidPrice4"]
        tick.bid_price_5 = data["bidPrice5"]

        tick.ask_price_1 = data["askPrice1"]
        tick.ask_price_2 = data["askPrice2"]
        tick.ask_price_3 = data["askPrice3"]
        tick.ask_price_4 = data["askPrice4"]
        tick.ask_price_5 = data["askPrice5"]

        tick.bid_volume_1 = data["bidQty1"]
        tick.bid_volume_2 = data["bidQty2"]
        tick.bid_volume_3 = data["bidQty3"]
        tick.bid_volume_4 = data["bidQty4"]
        tick.bid_volume_5 = data["bidQty5"]

        tick.ask_volume_1 = data["askQty1"]
        tick.ask_volume_2 = data["askQty2"]
        tick.ask_volume_3 = data["askQty3"]
        tick.ask_volume_4 = data["askQty4"]
        tick.ask_volume_5 = data["askQty5"]

        contract: ContractData = symbol_contract_map.get(tick.symbol, None)
        if contract:
            tick.name = contract.name

        self.gateway.on_tick(tick)

    def connect(
        self,
        accountid: str,
        password: str,
        md_address: str,
        auth_code: str,
        appid: str,
        collection_type: int,
        compress_flag: int,
    ) -> None:
        """连接服务器"""
        self.accountid = accountid
        self.password = password
        self.md_address = md_address
        self.auth_code = auth_code
        self.app_id = appid
        self.collection_type = collection_type
        self.compress_flag = compress_flag

        if not self.connect_status:
            path: str = str(get_folder_path(self.gateway_name.lower()))
            self.createDFITCMdApi(path.encode("GBK"))
            self.init(md_address)

    def login_server(self) -> None:
        """用户登录"""
        self.reqid += 1
        data: dict = {
            "requestID": self.reqid,
            "accountID": self.accountid,
            "password": self.password,
            "compressflag": self.compress_flag,
            "authenticCode": self.auth_code,
            "appID": self.app_id,
            "collectInterType": self.collection_type
        }
        self.reqSOPUserLogin(data)

        self.reqid += 1
        data["requestID"] = self.reqid
        self.reqStockUserLogin(data)

    def subscrbie(self, req: SubscribeRequest) -> None:
        """订阅行情"""
        if self.login_status:
            self.reqid += 1

            if check_option_symbol(req.symbol):
                symbol: str = str(EXCHANGE_VT2SEC[req.exchange] + req.symbol)
                self.subscribeSOPMarketData(symbol, self.reqid)

            else:
                symbol: str = str(EXCHANGE_VT2SEC[req.exchange] + req.symbol)
                self.subscribeStockMarketData(symbol, self.reqid)

            self.subscribed.add(req.symbol)

    def close(self) -> None:
        """关闭连接"""
        if self.connect_status:
            self.exit()


class SecTdApi(TdApi):

    def __init__(self, gateway: SecGateway):
        """构造函数"""
        super().__init__()

        self.gateway: SecGateway = gateway
        self.gateway_name: str = gateway.gateway_name

        self.accountid: str = ""
        self.password: str = ""
        self.auth_code: str = ""
        self.compress_flag: int = 0
        self.auth_code: str = ""
        self.app_id: str = ""
        self.collection_type: int = 1
        self.trading_day: str = ""
        self.positions: Dict[tuple, PositionData] = {}

        self.sessionid: str = ""
        self.reqid: int = 0
        self.localid: int = 10000
        self.orders: Dict[str, OrderData] = {}
        self.tradeids: set = set()

        self.connect_status: bool = False
        self.login_status: bool = False

    def onFrontConnected(self) -> None:
        """服务器连接成功回报"""
        self.login_server()
        self.connect_status = True

    def onFrontDisconnected(self, reason: int) -> None:
        """服务器连接断开回报"""
        self.connect_status = False
        self.login_status = False
        self.gateway.write_log(f"交易服务器连接断开, 原因{reason}")

    def onRspStockUserLogin(self, data: dict, error: dict) -> None:
        """用户登录股票行情服务器请求回报"""
        if not error:
            self.gateway.write_log("股票交易服务器登录成功")
            self.query_stock_contracts()
        else:
            self.gateway.write_error("股票交易服务器登录失败", error)

    def onRspSOPUserLogin(self, data: dict, error: dict) -> None:
        """用户登录股票期权行情服务器请求回报"""
        if not error:
            self.trading_day = str(data["tradingDay"])
            self.sessionid = str(data["sessionID"])

            self.gateway.write_log("股票期权交易服务器登录成功")
            self.login_status = True

            self.query_option_contracts()
        else:
            self.gateway.write_error("股票期权交易服务器登录失败", error)

    def onRspError(self, error: dict) -> None:
        """请求报错回报"""
        self.gateway.write_error("交易接口报错", error)

    def onStockEntrustOrderRtn(self, data: dict) -> None:
        """股票委托更新推送"""
        localid: str = str(data["localOrderID"])
        sessionid: str = str(data["sessionID"])
        orderid: str = f"{sessionid}_{localid}"

        if orderid in self.orders:
            order: OrderData = self.orders[orderid]
        else:
            timestamp: str = self.trading_day + str(data["entrustTime"])
            dt: datetime = generate_datetime(timestamp)

            order: OrderData = OrderData(
                symbol=data["securityID"],
                exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
                orderid=orderid,
                direction=DIRECTION_SEC2VT[data["entrustDirection"]],
                price=data["entrustPrice"],
                volume=data["entrustQty"],
                datetime=dt,
                gateway_name=self.gateway_name
            )
            self.orders[orderid] = order

        if order.volume == order.traded:
            order.status = Status.ALLTRADED
        elif order.traded > 0:
            order.status = Status.PARTTRADED
        else:
            order.status = Status.NOTTRADED

        self.gateway.on_order(copy(order))

    def onSOPEntrustOrderRtn(self, data: dict) -> None:
        """股票期权委托更新推送"""
        localid: str = str(data["localOrderID"])
        sessionid: str = str(data["sessionID"])
        orderid: str = f"{sessionid}_{localid}"

        if orderid in self.orders:
            order: OrderData = self.orders[orderid]
        else:
            timestamp: str = self.trading_day + str(data["entrustTime"])
            dt: datetime = generate_datetime(timestamp)

            order: OrderData = OrderData(
                symbol=data["securityID"],
                exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
                orderid=orderid,
                direction=DIRECTION_SEC2VT[data["entrustDirection"]],
                offset=OFFSET_SEC2VT[data["openCloseFlag"]],
                price=data["entrustPrice"],
                volume=data["entrustQty"],
                datetime=dt,
                gateway_name=self.gateway_name
            )
            self.orders[orderid] = order

        if order.volume == order.traded:
            order.status = Status.ALLTRADED
        elif order.traded > 0:
            order.status = Status.PARTTRADED
        else:
            order.status = Status.NOTTRADED

        self.gateway.on_order(copy(order))

    def onStockTradeRtn(self, data: dict) -> None:
        """股票成交数据推送"""
        timestamp: str = self.trading_day + str(data["tradeTime"])
        dt: datetime = generate_datetime(timestamp)
        localid: str = str(data["localOrderID"])
        sessionid: str = str(data["sessionID"])
        orderid: str = f"{sessionid}_{localid}"

        trade: TradeData = TradeData(
            symbol=data["securityID"],
            exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
            tradeid=data["tradeID"],
            orderid=orderid,
            direction=DIRECTION_SEC2VT[data["entrustDirection"]],
            price=data["tradePrice"],
            volume=data["tradeQty"],
            datetime=dt,
            gateway_name=self.gateway_name
        )

        if trade.tradeid not in self.tradeids:
            self.tradeids.add(trade.tradeid)
            self.gateway.on_trade(trade)

        # 获取缓存的委托信息
        order: OrderData = self.orders.get(orderid, None)
        if not order:
            return

        order.traded += trade.volume
        order.traded = min(order.volume, order.traded)

        # 检查委托状态
        if order.volume == order.traded:
            order.status = Status.ALLTRADED
        elif order.traded > 0:
            order.status = Status.PARTTRADED

        order.datetime = trade.datetime
        self.gateway.on_order(copy(order))

    def onSOPTradeRtn(self, data: dict) -> None:
        """股票期权成交数据推送"""
        timestamp: str = self.trading_day + str(data["tradeTime"])
        dt: datetime = generate_datetime(timestamp)
        localid: str = str(data["localOrderID"])
        sessionid: str = str(data["sessionID"])
        orderid: str = f"{sessionid}_{localid}"

        trade: TradeData = TradeData(
            symbol=data["securityID"],
            exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
            tradeid=data["tradeID"],
            orderid=orderid,
            direction=DIRECTION_SEC2VT[data["entrustDirection"]],
            offset=OFFSET_SEC2VT[data["openCloseFlag"]],
            price=data["tradePrice"],
            volume=data["tradeQty"],
            datetime=dt,
            gateway_name=self.gateway_name
        )

        if trade.tradeid not in self.tradeids:
            self.tradeids.add(trade.tradeid)
            self.gateway.on_trade(trade)

        # 获取缓存的委托信息
        order: OrderData = self.orders.get(orderid, None)
        if not order:
            return

        order.traded += trade.volume
        order.traded = min(order.volume, order.traded)

        # 检查委托状态
        if order.volume == order.traded:
            order.status = Status.ALLTRADED
        elif order.traded > 0:
            order.status = Status.PARTTRADED

        order.datetime = trade.datetime
        self.gateway.on_order(copy(order))

    def onStockWithdrawOrderRtn(self, data: dict) -> None:
        """股票委托撤单数据推送"""
        localid: str = str(data["localOrderID"])
        sessionid: str = str(data["sessionID"])
        orderid: str = f"{sessionid}_{localid}"

        if orderid in self.orders:
            order: OrderData = self.orders[orderid]
            dt: datetime = datetime.now()
            dt: datetime = dt.replace(tzinfo=CHINA_TZ)
            order.datetime = dt
        else:
            timestamp: str = self.trading_day + str(data["entrustTime"])
            dt: datetime = generate_datetime(timestamp)
            order: OrderData = OrderData(
                symbol=data["securityID"],
                exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
                orderid=orderid,
                direction=DIRECTION_SEC2VT[data["entrustDirection"]],
                price=data["entrustPrice"],
                traded=data["tradeQty"],
                volume=data["tradeQty"] + data["withdrawQty"],
                datetime=dt,
                gateway_name=self.gateway_name
            )
            self.orders[orderid] = order

        if data["withdrawQty"]:
            order.status = Status.CANCELLED

        self.gateway.on_order(copy(order))

    def onSOPWithdrawOrderRtn(self, data: dict) -> None:
        """股票期权委托撤单数据推送"""
        localid: str = str(data["localOrderID"])
        sessionid: str = str(data["sessionID"])
        orderid: str = f"{sessionid}_{localid}"

        if orderid in self.orders:
            order: OrderData = self.orders[orderid]
        else:
            timestamp: str = self.trading_day + str(data["entrustTime"])
            dt: datetime = generate_datetime(timestamp)

            order: OrderData = OrderData(
                symbol=data["securityID"],
                exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
                orderid=orderid,
                direction=DIRECTION_SEC2VT[data["entrustDirection"]],
                price=data["entrustPrice"],
                traded=data["tradeQty"],
                offset=OFFSET_SEC2VT[data["openCloseFlag"]],
                volume=data["tradeQty"] + data["withdrawQty"],
                datetime=dt,
                gateway_name=self.gateway_name
            )
            self.orders[orderid] = order

        if data["withdrawQty"]:
            order.status = Status.CANCELLED

        self.gateway.on_order(copy(order))

    def OnRspStockEntrustOrder(self, data: dict, error: dict) -> None:
        """股票委托下单失败回报"""
        if error:
            localid: str = str(error["localOrderID"])
            sessionid: str = str(error["sessionID"])
            orderid: str = f"{sessionid}_{localid}"
            order: OrderData = self.orders.get(orderid, None)

            if order:
                dt: datetime = datetime.now()
                dt: datetime = dt.replace(tzinfo=CHINA_TZ)
                order.datetime = dt
                order.status = Status.REJECTED
                self.gateway.on_order(order)

                self.gateway.write_error("股票委托错误", error)

    def onRspSOPEntrustOrder(self, data: dict, error: dict) -> None:
        """股票期权委托下单失败回报"""
        if error:
            localid: str = str(error["localOrderID"])
            sessionid: str = str(error["sessionID"])
            orderid: str = f"{sessionid}_{localid}"
            order: OrderData = self.orders.get(orderid, None)

            if order:
                dt: datetime = datetime.now()
                dt: datetime = dt.replace(tzinfo=CHINA_TZ)
                order.datetime = dt
                order.status = Status.REJECTED
                self.gateway.on_order(order)

                self.gateway.write_error("期权委托错误", error)

    def onRspSOPWithdrawOrder(self, data: dict, error: dict) -> None:
        """股票期权委托撤单失败回报"""
        if error:
            self.gateway.write_error("撤单错误", error)

    def onRspStockQryStockStaticInfo(self, data: dict, error: dict, last: bool) -> None:
        """股票合约查询回报"""
        if not data:
            return

        if data["securityID"] == "510050":
            contract: ContractData = ContractData(
                symbol=data["securityID"],
                exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
                name=data["securityName"],
                size=data["tradeUnit"],
                pricetick=0.001,
                product=Product.EQUITY,
                gateway_name=self.gateway_name
            )
            self.gateway.on_contract(contract)

            symbol_contract_map[contract.symbol] = contract

        if last:
            msg: str = "股票交易合约信息获取完成"
            self.gateway.write_log(msg)

    def onRspSOPQryContactInfo(self, data: dict, error: dict, last: bool) -> None:
        """股票期权合约查询回报"""
        if not data:
            return

        contract: ContractData = ContractData(
            symbol=data["securityOptionID"],
            exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
            name=data["contractName"],
            size=data["contactUnit"],
            pricetick=data["miniPriceChange"],
            product=Product.OPTION,
            option_portfolio=data["securityID"] + "_O",
            option_type=OPTION_TYPE_SEC2VT[data["optType"]],
            option_strike=data["execPrice"],
            option_underlying="-".join([data["securityID"], str(data["endDate"])[:-2]]),
            option_expiry=datetime.strptime(str(data["endDate"]), "%Y%m%d"),
            gateway_name=self.gateway_name
        )

        contract.option_index = get_option_index(
            contract.option_strike, data["contractID"]
        )

        self.gateway.on_contract(contract)

        symbol_contract_map[contract.symbol] = contract

        if last:
            msg: str = "期权交易合约信息获取完成"
            self.gateway.write_log(msg)

            # 本地生成ETF合约数据
            etfs: list = [
                ("510050", Exchange.SSE, "华夏上证50ETF"),
                ("510300", Exchange.SSE, "华泰柏瑞沪深300ETF"),
                ("159919", Exchange.SZSE, "嘉实沪深300ETF"),
            ]
            for symbol, exchange, name in etfs:
                contract: ContractData = ContractData(
                    symbol=symbol,
                    exchange=exchange,
                    name=name,
                    size=1,
                    pricetick=0.0001,
                    product=Product.ETF,
                    gateway_name=self.gateway_name
                )
                self.gateway.on_contract(contract)

    def onRspStockQryCapitalAccountInfo(self, data: dict, error: dict, last: bool) -> None:
        """股票资金查询回报"""
        if not data:
            return

        account: AccountData = AccountData(
            accountid=data["accountID"] + "_Stock",
            balance=data["totalFunds"],
            frozen=data["totalFunds"] - data["availableFunds"],
            gateway_name=self.gateway_name
        )
        self.gateway.on_account(account)

    def onRspSOPQryCapitalAccountInfo(self, data: dict, error: dict) -> None:
        """股票期权资金查询回报"""
        if not data:
            return

        account: AccountData = AccountData(
            accountid=data["accountID"] + "_Option",
            balance=data["totalFunds"],
            frozen=data["totalFunds"] - data["availableFunds"],
            gateway_name=self.gateway_name
        )
        self.gateway.on_account(account)

    def onRspStockQryPosition(self, data: dict, error: dict, last: bool) -> None:
        """股票持仓查询回报"""
        if not data or not data["securityID"]:
            return

        pos: PositionData = PositionData(
            symbol=data["securityID"],
            exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
            direction=Direction.NEt,
            volume=data["totalQty"],
            price=data["avgPositionPrice"],
            gateway_name=self.gateway_name
        )
        self.gateway.on_position(pos)

    def onRspSOPQryPosition(self, data: dict, error: dict, last: bool) -> None:
        """股票期权持仓查询回报"""
        if not data or not data["securityOptionID"]:
            return

        # 生成持仓键，并查询持仓缓存
        key: tuple = (data["securityOptionID"], data["entrustDirection"])
        pos: PositionData = self.positions.get(key, None)

        # 如果没有持仓则初始化
        if not pos:
            pos = PositionData(
                symbol=data["securityOptionID"],
                exchange=EXCHANGE_SEC2VT[data["exchangeID"]],
                direction=DIRECTION_SEC2VT[data["entrustDirection"]],
                gateway_name=self.gateway_name
            )
            self.positions[key] = pos

        # 更新持仓信息
        pos.volume = data["totalQty"]
        pos.price = data["openAvgPrice"]

        # 如果查询结束则推送
        if last:
            for pos in self.positions.values():
                self.gateway.on_position(pos)

    def connect(
        self,
        accountid: str,
        password: str,
        td_address: str,
        auth_code: str,
        appid: str,
        collection_type: int,
    ) -> None:
        """连接服务器"""
        self.accountid = accountid
        self.password = password
        self.auth_code = auth_code
        self.app_id = appid
        self.collection_type = collection_type

        if not self.connect_status:
            path: str = str(get_folder_path(self.gateway_name.lower()))
            self.createDFITCSECTraderApi(path.encode("GBK"))

            self.subscribePrivateTopic(2)
            self.init(td_address)

    def login_server(self) -> None:
        """用户登录"""
        self.reqid += 1
        sop: dict = {
            "requestID": self.reqid,
            "accountID": self.accountid,
            "password": self.password,
            "compressflag": self.compress_flag,
            "authenticCode": self.auth_code,
            "appID": self.app_id,
            "collectInterType": self.collection_type
        }
        self.reqSOPUserLogin(sop)

        self.reqid += 1
        stock: dict = {
            "requestID": self.reqid,
            "accountID": self.accountid,
            "password": self.password,
            "compressflag": self.compress_flag,
            "collectInterType": self.collection_type
        }
        self.reqStockUserLogin(stock)

    def send_order(self, req: OrderRequest) -> str:
        """委托下单"""
        self.reqid += 1
        self.localid += 1

        sec_req: dict = {
            "securityID": req.symbol,
            "exchangeID": EXCHANGE_VT2SEC[req.exchange],
            "entrustPrice": req.price,
            "entrustQty": int(req.volume),
            "localOrderID": self.localid,
            "accountID": self.accountid,
            "requestID": self.reqid,
            "entrustDirection": DIRECTION_VT2SEC[req.direction]
        }

        # 股票期权委托
        if check_option_symbol(req.symbol):
            sec_req["orderType"] = OPTION_PRICE_TYPE_VT2SEC[req.type]
            sec_req["openCloseFlag"] = OFFSET_VT2SEC[req.offset]
            self.reqSOPEntrustOrder(sec_req)

        # 股票委托
        else:
            sec_req["orderType"] = STOCK_PRICE_TYPE_VT2SEC[req.type]
            self.reqStockEntrustOrder(sec_req)

        localid: str = str(self.localid)
        orderid: str = f"{self.sessionid}_{localid}"

        order: OrderData = OrderData(
            symbol=req.symbol,
            exchange=req.exchange,
            orderid=orderid,
            price=req.price,
            volume=req.volume,
            direction=req.direction,
            offset=req.offset,
            gateway_name=self.gateway_name,
        )
        self.orders[orderid] = order

        return order.vt_orderid

    def cancel_order(self, req: CancelRequest) -> None:
        """委托撤单"""
        self.reqid += 1
        sessionid, localid = req.orderid.split("_")

        sec_req: dict = {
            "localOrderID": int(localid),
            "accountID": self.accountid,
            "requestID": self.reqid,
            "sessionID": int(sessionid)
        }

        order: OrderData = self.orders.get(req.orderid, None)

        if not order:
            self.gateway.write_log("找不到撤单委托")
            return

        if check_option_symbol(req.symbol):
            self.reqSOPWithdrawOrder(sec_req)
        else:
            self.reqStockWithdrawOrder(sec_req)

    def query_account(self) -> None:
        """查询资金"""
        self.reqid += 1
        req: dict = {
            "requestID": self.reqid,
            "accountID": self.accountid
        }
        self.reqSOPQryCapitalAccountInfo(req)

    def query_position(self) -> None:
        """查询资金"""
        # 清除缓存的持仓数量和价格
        for pos in self.positions.values():
            pos.volume = 0
            pos.price = 0

        # 发起查询请求
        self.reqid += 1
        req: dict = {
            "requestID": self.reqid,
            "accountID": self.accountid
        }
        self.reqSOPQryPosition(req)

    def query_option_contracts(self) -> None:
        """查询股票期权合约"""
        self.reqid += 1
        req: dict = {
            "requestID": self.reqid,
            "accountID": self.accountid
        }
        self.reqSOPQryContactInfo(req)

    def query_stock_contracts(self) -> None:
        """查询股票合约"""
        self.reqid += 1
        req: dict = {
            "requestID": self.reqid,
            "accountID": self.accountid
        }
        self.reqStockQryStockStaticInfo(req)

    def close(self) -> None:
        """关闭连接"""
        if self.connect_status:
            self.exit()


def check_option_symbol(symbol) -> bool:
    """检查期权合约"""
    if len(symbol) > 6:
        return True
    return False


def get_option_index(strike_price: float, exchange_instrument_id: str) -> str:
    """获取期权索引"""
    exchange_instrument_id: str = exchange_instrument_id.replace(" ", "")

    if "M" in exchange_instrument_id:
        n: int = exchange_instrument_id.index("M")
    elif "A" in exchange_instrument_id:
        n: int = exchange_instrument_id.index("A")
    elif "B" in exchange_instrument_id:
        n: int = exchange_instrument_id.index("B")
    else:
        return str(strike_price)

    index: str = exchange_instrument_id[n:]
    option_index: str = f"{strike_price:.3f}-{index}"

    return option_index


def generate_datetime(timestamp: str) -> datetime:
    """生成时间"""
    dt: datetime = datetime.strptime(timestamp, "%Y%m%d%H:%M:%S.%f")
    dt: datetime = dt.replace(tzinfo=CHINA_TZ)
    return dt
