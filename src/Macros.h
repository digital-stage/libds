#ifndef DS_MACROS
#define DS_MACROS

#define STORE_GETTER(TYPE, NAME, MUTEX, VAR)                                   \
  std::optional<TYPE> get##NAME(const std::string& id)                         \
  {                                                                            \
    std::lock_guard<std::recursive_mutex>(this->MUTEX);                        \
    if(VAR.count(id) > 0) {                                                    \
      return VAR[id].get<TYPE>();                                              \
    }                                                                          \
    return std::nullopt;                                                       \
  }

#define STORE_CREATOR(TYPE, NAME, MUTEX, VAR)                                  \
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
  void remove##NAME()                                                          \
  {                                                                            \
    std::lock_guard<std::recursive_mutex>(this->MUTEX);                        \
    VAR.clear();                                                               \
  }

#define ADD_STORE_ENTRY(TYPE, NAME, VAR)                                       \
public:                                                                        \
  STORE_GETTER(TYPE, NAME, mutex_##VAR, VAR)                                   \
  STORE_CREATOR(TYPE, NAME, mutex_##VAR, VAR)                                  \
  STORE_UPDATE(TYPE, NAME, mutex_##VAR, VAR)                                   \
  STORE_REMOVE(TYPE, NAME, mutex_##VAR, VAR)                                   \
private:                                                                       \
  std::recursive_mutex mutex_##VAR;                                            \
  std::map<std::string, json> VAR;

#endif // DS_MACROS