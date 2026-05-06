#pragma once
#include <cstdint>
#include <type_traits>
#include <limits>

template<typename A, typename B>
using wider_int_t =
  std::conditional_t<
    (sizeof(A) >= sizeof(B)),
    std::conditional_t<std::is_signed_v<A>, A, std::make_signed_t<A>>,
    std::conditional_t<std::is_signed_v<B>, B, std::make_signed_t<B>>>;

template<typename Rep, int FracBits>
struct fixed {
  static_assert(std::is_integral_v<Rep>, "Rep must be a integral type");
  static_assert(FracBits >= 0, "FracBits must be non-negative");
public:
  using rep_type = Rep;
  static constexpr int frac_bits = FracBits;
  static constexpr rep_type scale = rep_type(1) << FracBits;

  constexpr fixed() noexcept : raw_(0) {}
  template <typename Int>
    requires std::is_integral_v<Int>
  constexpr fixed(Int v) noexcept
    : raw_(rep_type(v) * scale) {}
  template <typename Float>
    requires std::is_floating_point_v<Float>
  explicit constexpr fixed(Float v) noexcept
    : raw_(rep_type(v * scale)) {}

  static constexpr fixed from_raw(rep_type raw) noexcept
  {
    fixed x;
    x.raw_ = raw;
    return x;
  }

  constexpr rep_type raw() const noexcept { return raw_; }

  constexpr double to_double() const noexcept {
    return double(raw_) / double(scale);
  }

  friend constexpr fixed operator+(fixed a, fixed b) noexcept
  {
    return from_raw(a.raw_ + b.raw_);
  }
  friend constexpr fixed operator-(fixed a, fixed b) noexcept
  {
    return from_raw(a.raw_ - b.raw_);
  }
  friend constexpr fixed operator-(fixed a) noexcept
  {
    return from_raw(-a.raw_);
  }
  friend constexpr fixed operator*(fixed a, fixed b) noexcept
  {
    using wide = std::conditional_t<sizeof(rep_type) <= 4, int64_t, __int128_t>;
    wide tmp = wide(a.raw_) * wide(b.raw_);
    return from_raw(rep_type(tmp >> FracBits));
  }
  friend constexpr fixed operator/(fixed a, fixed b) noexcept
  {
    using wide = std::conditional_t<sizeof(rep_type) <= 4, int64_t, __int128_t>;
    wide tmp = (wide(a.raw_) << FracBits) / wide(b.raw_);
    return from_raw(rep_type(tmp));
  }
  friend constexpr bool operator==(fixed a, fixed b) noexcept
  {
    return a.raw_ == b.raw_;
  }
  friend constexpr bool operator<(fixed a, fixed b) noexcept
  {
    return a.raw_ < b.raw_;
  }
  friend constexpr bool operator!=(fixed a, fixed b) noexcept { return !(a==b); }
  friend constexpr bool operator>(fixed a, fixed b) noexcept { return b < a; }
  friend constexpr bool operator<=(fixed a, fixed b) noexcept { return !(b<a); }
  friend constexpr bool operator>=(fixed a, fixed b) noexcept { return !(a<b); }

private:
    rep_type raw_;
};

template<typename Ra, int Fa, typename Rb, int Fb>
constexpr auto operator+(fixed<Ra, Fa> a, fixed<Rb, Fb> b)
{
  constexpr int F = (Fa > Fb) ? Fa : Fb;
  using wide = wider_int_t<Ra, Rb>;

  wide ar = wide(a.raw()) << (F - Fa);
  wide br = wide(b.raw()) << (F - Fb);

  using result_t = fixed<wide, F>;
  return result_t::from_raw(ar + br);
}

template<typename Ra, int Fa, typename Rb, int Fb>
constexpr auto operator-(fixed<Ra, Fa> a, fixed<Rb, Fb> b)
{
  constexpr int F = (Fa > Fb) ? Fa : Fb;
  using wide = wider_int_t<Ra, Rb>;

  wide ar = wide(a.raw()) << (F - Fa);
  wide br = wide(b.raw()) << (F - Fb);

  using result_t = fixed<wide, F>;
  return result_t::from_raw(ar - br);
}

template<typename Ra, int Fa, typename Rb, int Fb>
constexpr auto operator*(fixed<Ra, Fa> a, fixed<Rb, Fb> b)
{
  using wide =
    std::conditional_t<
      (sizeof(Ra) + sizeof(Rb) <= 8),
    int64_t,
    __int128_t
    >;

  wide raw = wide(a.raw()) * wide(b.raw());

  using result_t = fixed<wide, Fa + Fb>;
  return result_t::from_raw(raw);
}

template<typename Ra, int Fa, typename Rb, int Fb>
constexpr auto operator/(fixed<Ra, Fa> a, fixed<Rb, Fb> b)
{
  using wide =
    std::conditional_t<
      (sizeof(Ra) + sizeof(Rb) <= 8),
    int64_t,
    __int128_t
    >;

  wide num = wide(a.raw()) << Fb;
  wide raw = num / wide(b.raw());

  using result_t = fixed<wide, Fa>;
  return result_t::from_raw(raw);
}

using q8_8 = fixed<int16_t, 8>;
using q16_16 = fixed<int32_t, 16>;
using q12_4 = fixed<int16_t, 4>;
using q24_8 = fixed<int32_t, 8>;
