#include "ordering/pbft/lock_free_collector_pool.h"

#include <glog/logging.h>

namespace resdb {

namespace {
uint32_t GetCapacity(uint32_t size) {
  int i = 0;
  for (i = 0; (1u << i) <= size; ++i)
    ;
  return (1u << i);
}
}  // namespace

LockFreeCollectorPool::LockFreeCollectorPool(const std::string& name,
                                             uint32_t size,
                                             TransactionExecutor* executor,
                                             bool enable_viewchange)
    : name_(name),
      capacity_(GetCapacity(size * 2)),
      mask_((capacity_ << 1) - 1),
      executor_(executor),
      enable_viewchange_(enable_viewchange) {
  collector_.resize(capacity_ << 1);
  for (size_t i = 0; i < (capacity_ << 1); ++i) {
    collector_[i] = std::make_unique<TransactionCollector>(i, executor_,
                                                           enable_viewchange_);
  }
  LOG(ERROR) << "name:" << name_ << " create pool done. capacity:" << capacity_
             << " enable viewchange:" << enable_viewchange_ << " done";
}

void LockFreeCollectorPool::Update(uint64_t seq) {
  uint32_t idx = seq & mask_;
  if (collector_[idx]->Seq() != seq) {
    LOG(ERROR) << "seq not match, skip update:" << seq;
    return;
  }
  // update:"<<(idx^capacity_)<<" seq:"<<seq+capacity_<<
  //	  " cap:"<<capacity_;
  collector_[idx ^ capacity_] = std::make_unique<TransactionCollector>(
      seq + capacity_, executor_, enable_viewchange_);
}

TransactionCollector* LockFreeCollectorPool::GetCollector(uint64_t seq) {
  uint32_t idx = seq & mask_;
  return collector_[idx].get();
}

}  // namespace resdb
