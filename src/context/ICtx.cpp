#include "ICtx.hpp"

namespace wss {

CThostFtdcInputOrderField* ICtx::getOrder() {
  CThostFtdcInputOrderField* req;
  _orderQueue.wait_dequeue(req);
  return req;
}

CThostFtdcInputOrderActionField* ICtx::getCancel() {
  CThostFtdcInputOrderActionField* req;
  _cancelQueue.wait_dequeue(req);
  return req;
}

void ICtx::subTick(std::shared_ptr<IStrategy> sp,
                   std::vector<std::string>& ids) {
  for (auto& id : ids) {
    _tickMap[id].insert(sp);
  }
}

void ICtx::subBar(std::shared_ptr<IStrategy> sp,
                  std::vector<std::string>& ids) {
  for (auto& id : ids) {
    _barMap[id].insert(sp);
  }
}

void ICtx::onTick(std::string id,
                  std::shared_ptr<CThostFtdcDepthMarketDataField> data) {
  for (auto sp : _tickMap["id"]) {
    sp->onTick(data);
  }
}

void ICtx::onBar(std::string id,
                  std::shared_ptr<CThostFtdcDepthMarketDataField> data) {
  for (auto sp : _barMap["id"]) {
    sp->onTick(data);
  }
}

}  // namespace wss