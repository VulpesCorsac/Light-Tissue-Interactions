#pragma once

template < typename T >
class Medium {
public:
    Medium(const T& new_n, const T& new_ua, const T& new_us, const T& new_D, const T& new_g);
    ~Medium() noexcept = default;

    const T n = 1.0; // refraction coeff
    const T ua = 0.0; // absorption coeff
    const T us = 0.0; // absorption coeff
    const T D = 0.0; // thickness/depth
    const T g = 0.0; // scattering anisotropy
    const T ut = ua + us; // total attenuation coeff
    const T a = us / ut; // albedo
    const T tau = D * ut; // optical thickness

 /*   inline T getN() const noexcept { return n; }
    inline T getMa() const noexcept { return ua; }
    inline T getMs() const noexcept { return us; }
    inline T getD() const noexcept { return D; }
    inline T getG() const noexcept { return g; }
    inline T getMt() const noexcept { return ut; }
    inline T getA() const noexcept { return a; }
    inline T getTau() const noexcept { return tau; }

protected:
    T n = 1.0; // refraction coeff
    T ua = 0.0; // absorption coeff
    T us = 0.0; // absorption coeff
    T D = 0.0; // thickness/depth
    T g = 0.0; // scattering anisotropy
    T ut = ua + us; // total attenuation coeff
    T a = us / ut; // albedo
    T tau = D * ut; // optical thickness*/
};

template < typename T >
Medium<T>::Medium(const T& new_n, const T& new_ua, const T& new_us, const T& new_D, const T& new_g) :
    n(new_n),
    ua(new_ua),
    us(new_us),
    D(new_D),
    g(new_g),
    ut(ua + us), // total attenuation coeff
    a(us / ut), // albedo
    tau(D * ut) { // optical thickness
}
