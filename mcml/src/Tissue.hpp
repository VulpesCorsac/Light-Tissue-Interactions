#pragma once

#include "Photon.hpp"


template<class T>
class Tissue {
public:
    T width;
    T n;
    T mfpl; //mean free path length
    T att;
    T g;

    Tissue() {}
    Tissue(T width, T n, T mfpl, T g, T att) : width(width), n(n), mfpl(mfpl), g(g), att(att) {}

    T path_length() const noexcept;

    void scatter(Photon<T>&, bool) const;


    T Frenel_refraction(Photon<T>&, bool) const;
};

template<class T>
T Tissue<T>::path_length() const noexcept {
    T rnd = fast_random<T>();
    if(rnd == 0)
        return this->width;

    T tmp = -mfpl*log(rnd);
    if(tmp  > this->width)
        return this->width;
    return tmp;
}

template<class T>
void Tissue<T>::scatter(Photon<T>& p, bool debug) const {
    T r1 = fast_random<T>();
    T r2 = fast_random<T>();
    T cos_th;
    T _x = p.direction.e[0];
    T _y = p.direction.e[1];
    T _z = p.direction.e[2];
    T x, y, z;

    if(g == 0)
        cos_th = 2*r1 - 1;
    else
        cos_th = (1 + g*g - (1-g*g)/(1 - g + 2*g*r1)
                           *(1-g*g)/(1 - g + 2*g*r1))/(2*g);

    T phi = 2*pi*r2;
    T sin_th;
    if(fabs(cos_th) > 0.99999)
        sin_th = 0;
    else
        sin_th = sqrt(1 - cos_th*cos_th);

    T cos_phi = cos(phi);
    T sin_phi = sin(phi);
    T temp = sqrt(1 - _z*_z);

    if(_z > 0.95) {
        x = sin_th*cos_phi;
        y = sin_th*sin_phi;
        z = cos_th;
    } else if (_z < -0.95) {
        x = sin_th*cos_phi;
        y = sin_th*sin_phi;
        z = cos_th;
    } else {
        x = sin_th*(_x*_z*cos_phi - _y*sin_phi)/temp + _x*cos_th;
        y = sin_th*(_y*_z*cos_phi + _x*sin_phi)/temp + _y*cos_th;
        z = -sin_th*cos_phi*temp + _z*cos_th;
    }
    if(debug) {
        std::cerr << "\n|||||||||||HG SCATTER|||||||||||||\n"
                  << "cos_th=" << cos_th << " sin_th=" << sin_th << " temp=" << temp
                  << "\n_x=" << _x << " _y=" << _y << " _z=" << _z
                  << "\nx=" << x << " y=" << y << " z=" << z
                  << "\n||||||||||||||||||||||||||||||||||\n";
    }

    p.direction = Vector3<T>(x, y, z);
}

template<class T>
T Tissue<T>::Frenel_refraction(Photon<T>& p, bool debug) const {
    T cos_th1 = fabs(p.direction.e[2]);
    T sin_th1 = sqrt(1 - cos_th1*cos_th1);
    T sin_th2 = n*sin_th1;
    if(sin_th2 >= 1)
        return 1;
    T cos_th2 = sqrt(1 - sin_th2*sin_th2);
    if(sin_th1 == 0)
        return (1-n)*(1-n)/((1+n)*(1+n));

    if(debug){
        std::cerr << "\n-----------------------------------\n"
                  << "Frenel refraction code\n"
                  << cos_th1 << ' ' << sin_th1 << ' ' << cos_th2 << ' ' << sin_th2
                  << "\n-----------------------------------\n";
    }

    return 0.5*(sin_th1*cos_th2 - cos_th1*sin_th2)*(sin_th1*cos_th2 - cos_th1*sin_th2)
            * ((cos_th1*cos_th2 + sin_th1*sin_th2)*(cos_th1*cos_th2 + sin_th1*sin_th2)
             + (cos_th1*cos_th2 - sin_th1*sin_th2)*(cos_th1*cos_th2 - sin_th1*sin_th2))
            / ((sin_th1*cos_th2 + cos_th1*sin_th2)*(sin_th1*cos_th2 + cos_th1*sin_th2)
             * (cos_th1*cos_th2 + sin_th1*sin_th2)*(cos_th1*cos_th2 + sin_th1*sin_th2));
}