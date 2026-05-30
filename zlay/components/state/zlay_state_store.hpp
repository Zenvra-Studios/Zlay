#ifndef ZLAY_COMPONENTS_STATE_ZLAY_STATE_STORE_HPP
#define ZLAY_COMPONENTS_STATE_ZLAY_STATE_STORE_HPP

#include <map>
#include <memory>
#include <string>

namespace zlay {

class StateStore {
public:
  template <class T>
  void set(const std::string& name, const T& value) {
    values_[name].reset(new Holder<T>(value));
  }

  template <class T>
  T* get(const std::string& name) {
    std::map<std::string, std::shared_ptr<Base> >::iterator it = values_.find(name);
    if (it == values_.end()) {
      return 0;
    }
    Holder<T>* holder = dynamic_cast<Holder<T>*>(it->second.get());
    return holder ? &holder->value : 0;
  }

private:
  struct Base {
    virtual ~Base() {}
  };

  template <class T>
  struct Holder : Base {
    explicit Holder(const T& v) : value(v) {}
    T value;
  };

  std::map<std::string, std::shared_ptr<Base> > values_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_STATE_ZLAY_STATE_STORE_HPP
