#include "Inventory.hpp"

namespace hx {
void Inventory::add(const Item& item,int qty){ auto &entry = data_[item.id]; if(entry.second==0) entry.first=item; entry.second+=qty; entry.first.count=entry.second; }
bool Inventory::remove(const std::string& id,int qty){ auto it=data_.find(id); if(it==data_.end()) return false; if(it->second.second<qty) return false; it->second.second-=qty; it->second.first.count=it->second.second; if(it->second.second==0) data_.erase(it); return true; }
int Inventory::quantity(const std::string& id) const{ auto it=data_.find(id); return it==data_.end()?0:it->second.second; }
std::vector<Item> Inventory::list() const{ std::vector<Item> out; for(auto &kv:data_) out.push_back(kv.second.first); return out; }
std::vector<Item> Inventory::asSimpleItems() const{ return list(); }
void Inventory::setFromSimple(const std::vector<Item>& items){ data_.clear(); for(auto &it:items) data_[it.id] = {it, it.count}; }
} // namespace hx
