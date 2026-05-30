#ifndef ZLAY_COMPONENTS_META_ZLAY_CRTP_HPP
#define ZLAY_COMPONENTS_META_ZLAY_CRTP_HPP

namespace zlay {

template <class Derived>
class Crtp {
protected:
  Derived& derived() { return static_cast<Derived&>(*this); }
  const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_META_ZLAY_CRTP_HPP
