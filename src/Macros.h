#ifndef DS_MACROS
#define DS_MACROS

#define STORE_GET(TYPE, NAME, MUTEX, VAR)                                      \
  std::optional<const TYPE> get##NAME(const std::string& id) const             \
  {                                                                            \
    std::lock_guard<std::recursive_mutex>(this->MUTEX);                        \
    if(VAR.count(id) > 0) {                                                    \
      return VAR.at(id).get<const TYPE>();                                     \
    }                                                                          \
    return std::nullopt;                                                       \
  }

#define STORE_GET_ALL(TYPE, NAME, MUTEX, VAR)                                  \
  std::vector<const TYPE> get##NAME##s() const                                 \
  {                                                                            \
    std::lock_guard<std::recursive_mutex>(this->MUTEX);                        \
    std::vector<const TYPE> items = std::vector<const TYPE>();                 \
    for(const auto& item : VAR) {                                              \
      items.push_back(item.second.get<const TYPE>());                          \
    }                                                                          \
    return items;                                                              \
  }

#define STORE_CREATE(TYPE, NAME, MUTEX, VAR)                                   \
  void create##NAME(const json payload)                                        \
  {                                                                            \
    std::lock_guard<std::recursive_mutex>(this->MUTEX);                        \
    const std::string _id = payload.at("_id").get<std::string>();              \
    VAR[_id] = payload;                                                        \
  }

#define STORE_UPDATE(TYPE, NAME, MUTEX, VAR)                                   \
  void update##NAME(const json payload)                                        \
  {                                                                            \
    const std::string& id = payload.at("_id").get<std::string>();              \
    std::lock_guard<std::recursive_mutex>(this->MUTEX);                        \
    VAR[id].merge_patch(payload);                                              \
  }

#define STORE_REMOVE(TYPE, NAME, MUTEX, VAR)                                   \
  void remove##NAME(const std::string& id)                                     \
  {                                                                            \
    std::lock_guard<std::recursive_mutex>(this->MUTEX);                        \
    VAR.erase(id);                                                             \
  }

#define STORE_REMOVE_ALL(TYPE, NAME, MUTEX, VAR)                               \
  void removeAll##NAME##s()                                                    \
  {                                                                            \
    std::lock_guard<std::recursive_mutex>(this->MUTEX);                        \
    for(const auto& item : VAR) {                                              \
      remove##NAME(item.first);                                                \
    }                                                                          \
  }

#define ADD_STORE_ENTRY(TYPE, NAME, VAR)                                       \
public:                                                                        \
  STORE_GET(TYPE, NAME, mutex_##VAR, VAR)                                      \
  STORE_GET_ALL(TYPE, NAME, mutex_##VAR, VAR)                                  \
  STORE_CREATE(TYPE, NAME, mutex_##VAR, VAR)                                   \
  STORE_UPDATE(TYPE, NAME, mutex_##VAR, VAR)                                   \
  STORE_REMOVE(TYPE, NAME, mutex_##VAR, VAR)                                   \
  STORE_REMOVE_ALL(TYPE, NAME, mutex_##VAR, VAR)                               \
private:                                                                       \
  mutable std::recursive_mutex mutex_##VAR;                                    \
  std::map<std::string, json> VAR;

#endif // DS_MACROS