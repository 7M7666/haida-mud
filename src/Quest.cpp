#include "Quest.hpp"

namespace hx {
void QuestLog::add(const Quest& q) { data_[q.id] = q; }
void QuestLog::complete(const std::string& id) { auto it = data_.find(id); if (it!=data_.end()) it->second.completed = true; }
bool QuestLog::isCompleted(const std::string& id) const { auto it=data_.find(id); return it!=data_.end() && it->second.completed; }
} // namespace hx
