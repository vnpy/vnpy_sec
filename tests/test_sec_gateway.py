from collections.abc import Callable, Iterator
from datetime import datetime
from typing import Any

import pytest

pytest.importorskip("vnpy_sec.api", reason="缺少 SEC 原生扩展")

from vnpy.event import EventEngine  # noqa: E402
from vnpy.trader.constant import (  # noqa: E402
    Direction,
    Exchange,
    Offset,
    OrderType,
    Status,
)
from vnpy.trader.object import (  # noqa: E402
    OrderData,
    OrderRequest,
    PositionData,
    TickData,
)

from vnpy_sec.api import (  # noqa: E402
    DFITCSEC_ED_Buy,
    DFITCSEC_EI_SH,
    DFITCSEC_OCF_Open,
    DFITCSEC_OT_LimitPrice,
    DFITCSEC_SOP_LimitPrice,
)
from vnpy_sec.gateway import sec_gateway  # noqa: E402
from vnpy_sec.gateway.sec_gateway import (  # noqa: E402
    CHINA_TZ,
    SecGateway,
    SecMdApi,
    SecTdApi,
)


class Sink:
    def __init__(self) -> None:
        self.logs: list[str] = []
        self.ticks: list[TickData] = []
        self.orders: list[OrderData] = []
        self.positions: list[PositionData] = []

    def attach(self, gateway: SecGateway) -> None:
        gateway.write_log = self.logs.append
        gateway.on_tick = self.ticks.append
        gateway.on_order = self.orders.append
        gateway.on_position = self.positions.append


class CallRecorder:
    def __init__(self) -> None:
        self.calls: list[tuple[str, Any]] = []

    def patch(self, monkeypatch: pytest.MonkeyPatch, api: object, names: list[str]) -> None:
        for name in names:
            monkeypatch.setattr(api, name, self.make_stub(name))

    def make_stub(self, name: str) -> Callable[..., int]:
        def stub(*args: Any) -> int:
            self.calls.append((name, args[0] if args else None))
            return 0
        return stub

    def names(self) -> list[str]:
        return [name for name, _ in self.calls]


TD_METHODS: list[str] = [
    "createDFITCSECTraderApi",
    "subscribePrivateTopic",
    "init",
    "exit",
    "reqSOPUserLogin",
    "reqStockUserLogin",
    "reqSOPEntrustOrder",
    "reqStockEntrustOrder",
    "reqSOPWithdrawOrder",
    "reqStockWithdrawOrder",
    "reqSOPQryCapitalAccountInfo",
    "reqSOPQryPosition",
    "reqSOPQryContactInfo",
    "reqStockQryStockStaticInfo",
]

MD_METHODS: list[str] = [
    "createDFITCMdApi",
    "init",
    "exit",
    "reqSOPUserLogin",
    "reqStockUserLogin",
    "subscribeSOPMarketData",
    "subscribeStockMarketData",
]


@pytest.fixture(autouse=True)
def clear_contracts() -> Iterator[None]:
    sec_gateway.symbol_contract_map.clear()
    yield
    sec_gateway.symbol_contract_map.clear()


@pytest.fixture
def sink() -> Sink:
    return Sink()


@pytest.fixture
def recorder() -> CallRecorder:
    return CallRecorder()


@pytest.fixture
def gateway(sink: Sink, recorder: CallRecorder, monkeypatch: pytest.MonkeyPatch) -> SecGateway:
    engine: EventEngine = EventEngine()
    gateway: SecGateway = SecGateway(engine, "SEC")
    sink.attach(gateway)
    recorder.patch(monkeypatch, gateway.td_api, TD_METHODS)
    recorder.patch(monkeypatch, gateway.md_api, MD_METHODS)
    return gateway


@pytest.fixture
def td_api(gateway: SecGateway) -> SecTdApi:
    return gateway.td_api


@pytest.fixture
def md_api(gateway: SecGateway) -> SecMdApi:
    return gateway.md_api


def connect_setting(**overrides: str) -> dict[str, str]:
    data: dict[str, str] = {
        "账号": "u1",
        "行情密码": "mp",
        "交易密码": "tp",
        "行情地址": "127.0.0.1:41213",
        "交易地址": "127.0.0.1:41205",
        "行情协议": "TCP",
        "授权码": "",
        "产品号": "",
        "采集类型": "顶点",
        "行情压缩": "N",
    }
    data.update(overrides)
    return data


def order_request(
    symbol: str = "600000",
    offset: Offset = Offset.NONE,
) -> OrderRequest:
    return OrderRequest(
        symbol=symbol,
        exchange=Exchange.SSE,
        direction=Direction.LONG,
        type=OrderType.LIMIT,
        volume=200,
        price=10.5,
        offset=offset,
    )


def stock_entrust(**overrides: Any) -> dict[str, Any]:
    data: dict[str, Any] = {
        "localOrderID": 7,
        "sessionID": 3,
        "entrustTime": "09:30:00.500000",
        "securityID": "600000",
        "exchangeID": DFITCSEC_EI_SH,
        "entrustDirection": DFITCSEC_ED_Buy,
        "entrustPrice": 10.5,
        "entrustQty": 200,
    }
    data.update(overrides)
    return data


def option_entrust(**overrides: Any) -> dict[str, Any]:
    data: dict[str, Any] = stock_entrust(
        securityID="10001234",
        openCloseFlag=DFITCSEC_OCF_Open,
        entrustPrice=0.05,
        entrustQty=2,
    )
    data.update(overrides)
    return data


def market_data(**overrides: Any) -> dict[str, Any]:
    data: dict[str, Any] = {
        "tradingDay": "20250926",
        "updateTime": "09:30:00.500000",
        "securityID": "600000",
        "exchangeID": DFITCSEC_EI_SH,
        "tradeQty": 1000,
        "latestPrice": 10.5,
        "upperLimitPrice": 11.5,
        "lowerLimitPrice": 9.5,
        "openPrice": 10.2,
        "highestPrice": 10.8,
        "lowestPrice": 10.1,
        "preClosePrice": 10.4,
        "bidPrice1": 10.49,
        "bidPrice2": 10.48,
        "bidPrice3": 10.47,
        "bidPrice4": 10.46,
        "bidPrice5": 10.45,
        "askPrice1": 10.51,
        "askPrice2": 10.52,
        "askPrice3": 10.53,
        "askPrice4": 10.54,
        "askPrice5": 10.55,
        "bidQty1": 1,
        "bidQty2": 2,
        "bidQty3": 3,
        "bidQty4": 4,
        "bidQty5": 5,
        "askQty1": 6,
        "askQty2": 7,
        "askQty3": 8,
        "askQty4": 9,
        "askQty5": 10,
    }
    data.update(overrides)
    return data


def test_connect_prefixes_bare_address(gateway: SecGateway, monkeypatch: pytest.MonkeyPatch) -> None:
    seen: dict[str, str] = {}
    monkeypatch.setattr(gateway.md_api, "connect", lambda *args: seen.__setitem__("md", args[2]))
    monkeypatch.setattr(gateway.td_api, "connect", lambda *args: seen.__setitem__("td", args[2]))

    setting: dict[str, str] = connect_setting()
    setting["交易地址"] = "127.0.0.1:41205"
    setting["行情地址"] = "ssl://127.0.0.1:41213"
    gateway.connect(setting)

    assert seen["td"] == "tcp://127.0.0.1:41205"
    assert seen["md"] == "ssl://127.0.0.1:41213"


def test_connect_rewrites_udp_quote(gateway: SecGateway, monkeypatch: pytest.MonkeyPatch) -> None:
    seen: dict[str, str] = {}
    monkeypatch.setattr(gateway.md_api, "connect", lambda *args: seen.__setitem__("md", args[2]))
    monkeypatch.setattr(gateway.td_api, "connect", lambda *args: seen.__setitem__("td", args[2]))

    setting: dict[str, str] = connect_setting(行情协议="UDP")
    setting["行情地址"] = "127.0.0.1:41213"
    setting["交易地址"] = "tcp://127.0.0.1:41205"
    gateway.connect(setting)

    assert seen["md"] == "udp://127.0.0.1:41213"
    assert seen["td"] == "tcp://127.0.0.1:41205"


def test_stock_order_id_uses_session_and_local(td_api: SecTdApi, recorder: CallRecorder) -> None:
    td_api.sessionid = "3"
    vt_orderid: str = td_api.send_order(order_request())

    assert vt_orderid == "SEC.3_10001"
    assert recorder.names() == ["reqStockEntrustOrder"]
    request: dict[str, Any] = recorder.calls[0][1]
    assert request["localOrderID"] == 10001
    assert request["orderType"] == DFITCSEC_OT_LimitPrice
    assert "openCloseFlag" not in request
    assert td_api.orders["3_10001"].orderid == "3_10001"


def test_option_order_id_uses_session_and_local(td_api: SecTdApi, recorder: CallRecorder) -> None:
    td_api.sessionid = "8"
    vt_orderid: str = td_api.send_order(order_request(symbol="10001234", offset=Offset.OPEN))

    assert vt_orderid == "SEC.8_10001"
    assert recorder.names() == ["reqSOPEntrustOrder"]
    request: dict[str, Any] = recorder.calls[0][1]
    assert request["orderType"] == DFITCSEC_SOP_LimitPrice
    assert request["openCloseFlag"] == DFITCSEC_OCF_Open


def test_stock_entrust_is_not_traded(td_api: SecTdApi, sink: Sink) -> None:
    td_api.trading_day = "20250926"
    td_api.onStockEntrustOrderRtn(stock_entrust())

    order: OrderData = sink.orders[0]
    assert order.orderid == "3_7"
    assert order.status == Status.NOTTRADED
    assert order.exchange == Exchange.SSE
    assert order.datetime == datetime(2025, 9, 26, 9, 30, 0, 500000, tzinfo=CHINA_TZ)


def test_option_entrust_is_not_traded(td_api: SecTdApi, sink: Sink) -> None:
    td_api.trading_day = "20250926"
    td_api.onSOPEntrustOrderRtn(option_entrust())

    order: OrderData = sink.orders[0]
    assert order.orderid == "3_7"
    assert order.status == Status.NOTTRADED
    assert order.offset == Offset.OPEN


def test_option_partial_fill_is_part_traded(td_api: SecTdApi, sink: Sink) -> None:
    td_api.trading_day = "20250926"
    td_api.orders["3_7"] = OrderData(
        symbol="10001234",
        exchange=Exchange.SSE,
        orderid="3_7",
        direction=Direction.LONG,
        offset=Offset.OPEN,
        price=0.05,
        volume=2,
        traded=1,
        gateway_name="SEC",
    )
    td_api.onSOPEntrustOrderRtn(option_entrust())

    assert sink.orders[0].status == Status.PARTTRADED


def test_option_full_fill_is_all_traded(td_api: SecTdApi, sink: Sink) -> None:
    td_api.trading_day = "20250926"
    td_api.orders["3_7"] = OrderData(
        symbol="10001234",
        exchange=Exchange.SSE,
        orderid="3_7",
        direction=Direction.LONG,
        offset=Offset.OPEN,
        price=0.05,
        volume=2,
        traded=2,
        gateway_name="SEC",
    )
    td_api.onSOPEntrustOrderRtn(option_entrust())

    assert sink.orders[0].status == Status.ALLTRADED


def test_stock_withdraw_sets_cancelled(td_api: SecTdApi, sink: Sink) -> None:
    td_api.orders["3_7"] = OrderData(
        symbol="600000",
        exchange=Exchange.SSE,
        orderid="3_7",
        direction=Direction.LONG,
        price=10.5,
        volume=200,
        gateway_name="SEC",
    )
    td_api.onStockWithdrawOrderRtn({
        "localOrderID": 7,
        "sessionID": 3,
        "withdrawQty": 200,
    })

    assert sink.orders[0].status == Status.CANCELLED
    assert sink.orders[0].datetime is not None
    assert sink.orders[0].datetime.tzinfo == CHINA_TZ


def test_option_withdraw_sets_cancelled(td_api: SecTdApi, sink: Sink) -> None:
    td_api.trading_day = "20250926"
    td_api.onSOPWithdrawOrderRtn({
        "localOrderID": 7,
        "sessionID": 3,
        "entrustTime": "09:31:00.000000",
        "securityID": "10001234",
        "exchangeID": DFITCSEC_EI_SH,
        "entrustDirection": DFITCSEC_ED_Buy,
        "openCloseFlag": DFITCSEC_OCF_Open,
        "entrustPrice": 0.05,
        "tradeQty": 1,
        "withdrawQty": 1,
    })

    order: OrderData = sink.orders[0]
    assert order.orderid == "3_7"
    assert order.status == Status.CANCELLED
    assert order.volume == 2


def test_option_entrust_error_sets_rejected(td_api: SecTdApi, sink: Sink) -> None:
    td_api.sessionid = "8"
    td_api.send_order(order_request(symbol="10001234", offset=Offset.OPEN))
    td_api.onRspSOPEntrustOrder({}, {
        "localOrderID": 10001,
        "sessionID": 8,
        "errorID": 15,
        "errorMsg": "拒单",
    })

    assert sink.orders[0].status == Status.REJECTED
    assert sink.orders[0].orderid == "8_10001"
    assert "期权委托错误" in sink.logs[0]


def test_stock_position_volume_pushes_immediately(td_api: SecTdApi, sink: Sink) -> None:
    td_api.onRspStockQryPosition({
        "securityID": "600000",
        "exchangeID": DFITCSEC_EI_SH,
        "totalQty": 800,
        "avgPositionPrice": 11.2,
    }, {}, False)

    position: PositionData = sink.positions[0]
    assert position.symbol == "600000"
    assert position.exchange == Exchange.SSE
    assert position.direction == Direction.NET
    assert position.volume == 800
    assert position.price == 11.2
    assert position.yd_volume == 0


def test_option_position_volume_flushes_on_last(td_api: SecTdApi, sink: Sink) -> None:
    td_api.onRspSOPQryPosition({
        "securityOptionID": "10001234",
        "exchangeID": DFITCSEC_EI_SH,
        "entrustDirection": DFITCSEC_ED_Buy,
        "totalQty": 5,
        "openAvgPrice": 0.2,
    }, {}, False)
    assert sink.positions == []

    td_api.onRspSOPQryPosition({
        "securityOptionID": "10002222",
        "exchangeID": DFITCSEC_EI_SH,
        "entrustDirection": DFITCSEC_ED_Buy,
        "totalQty": 2,
        "openAvgPrice": 0.3,
    }, {}, True)

    assert [position.volume for position in sink.positions] == [5, 2]
    assert sink.positions[0].direction == Direction.LONG
    assert sink.positions[0].price == 0.2
    assert sink.positions[1].price == 0.3


def test_empty_stock_position_is_ignored(td_api: SecTdApi, sink: Sink) -> None:
    td_api.onRspStockQryPosition({}, {}, True)

    assert sink.positions == []


def test_market_data_datetime(md_api: SecMdApi, sink: Sink) -> None:
    data: dict[str, Any] = market_data()
    md_api.onSOPMarketData(data)
    md_api.onStockMarketData(data)

    assert len(sink.ticks) == 2
    tick: TickData
    for tick in sink.ticks:
        assert tick.datetime == datetime(2025, 9, 26, 9, 30, 0, 500000, tzinfo=CHINA_TZ)
        assert tick.exchange == Exchange.SSE
        assert tick.last_price == 10.5


def test_close_without_connection_does_not_exit(
    td_api: SecTdApi,
    md_api: SecMdApi,
    recorder: CallRecorder,
) -> None:
    td_api.close()
    md_api.close()

    assert recorder.calls == []
