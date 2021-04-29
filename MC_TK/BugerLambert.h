#pragma once

#include <cmath>

template < typename T> // NORMAL INCIDENCE!!!
T Rborder(T n_slab, T n_slide) {
    const auto cached1 = FresnelR(n_slab, n_slide, static_cast<T>(1));
    const auto cached2 = FresnelR(n_slide, static_cast<T>(1), static_cast<T>(1));
    const auto cached3 = cached1 * cached2;
    return (cached1 + cached2 - 2 * cached3) / (1 - cached3);
}

template < typename T>
T BugerLambert(T tau, T n_slab, T n_slide_top, T n_slide_bottom) {
    auto Rbtop = Rborder<T>(n_slab, n_slide_top);
    auto Rbbottom = Rborder<T>(n_slab, n_slide_bottom);
    return ((1 - Rbtop) * (1 - Rbbottom) * exp(-tau)) / (1 - Rbtop * Rbbottom * exp(-2 * tau));
}