#pragma once

#include "bytebool.h"
#include "math/vector.h"
#include "math/plane.h"


#define VectorCopy(a, b)  ((b)[0] = (a)[0], (b)[1] = (a)[1], (b)[2] = (a)[2])
#define RGBTOGRAY(x)  ((float)((x)[0]) * 0.2989f + (float)((x)[1]) * 0.5870f + (float)((x)[2]) * 0.1140f)
#define VectorFastNormalize  VectorNormalize


template<typename T>
inline void value_maximize(T &value, const T &other) {
    value = std::max(value, other);
}


template<typename T>
inline void value_minimize(T &value, const T &other) {
    value = std::min(value, other);
}


inline bool bit_is_enabled(const byte *bytes, int bit_index) {
    return (bytes[bit_index >> 3] & (1 << (bit_index & 7))) != 0;
}


inline void bit_enable(byte *bytes, int bit_index){
    bytes[bit_index >> 3] |= (1 << (bit_index & 7));
}


template<typename T>
struct MinMax___ {
    BasicVector3<T> mins;
    BasicVector3<T> maxs;

    MinMax___() :
        mins(std::numeric_limits<T>::max()),
        maxs(std::numeric_limits<T>::lowest()) {}

    template<typename U>
    MinMax___(const BasicVector3<U> &min, const BasicVector3<U> &max) :
        mins(min), maxs(max) {}

    void clear() { *this = MinMax___(); }
    bool valid() const { return mins.x() < maxs.x() && mins.y() < maxs.y() && mins.z() < maxs.z(); }

    template<typename U>
    void extend(const BasicVector3<U> &point) {
        for (size_t i = 0; i < 3; ++i) {
            const auto val = point[i];
            if (val < mins[i]) { mins[i] = val; }
            if (val > maxs[i]) { maxs[i] = val; }
        }
    }

    T area() const {
        T x = maxs.x() - mins.x();
        T y = maxs.y() - mins.y();
        T z = maxs.z() - mins.z();
        return (x * y + y * z + z * x) * 2;
    }

    template<typename U>
    void extend(const MinMax___<U> &other) {
        extend(other.mins);
        extend(other.maxs);
    }

    template<typename U>  // true, if point is within the bounds
    bool test(const BasicVector3<U> &point) const {
        return point.x() >= mins.x() && point.y() >= mins.y() && point.z() >= mins.z()
            && point.x() <= maxs.x() && point.y() <= maxs.y() && point.z() <= maxs.z();
    }

    template<typename U, typename E>  // true, if point is within the bounds expanded by epsilon
    bool test(const BasicVector3<U> &point, const E epsilon) const {
        return point.x() >= mins.x() - epsilon && point.y() >= mins.y() - epsilon && point.z() >= mins.z() - epsilon
            && point.x() <= maxs.x() + epsilon && point.y() <= maxs.y() + epsilon && point.z() <= maxs.z() + epsilon;
    }

    template<typename U>  // true, if there is an intersection
    bool test( const MinMax___<U> &other) const {
            return other.maxs.x() >= mins.x() && other.maxs.y() >= mins.y() && other.maxs.z() >= mins.z()
                && other.mins.x() <= maxs.x() && other.mins.y() <= maxs.y() && other.mins.z() <= maxs.z();
    }

    template<typename U>  // true, if other is completely enclosed by this
    bool surrounds(const MinMax___<U> &other) const {
            return other.mins.x() >= mins.x() && other.mins.y() >= mins.y() && other.mins.z() >= mins.z()
                && other.maxs.x() <= maxs.x() && other.maxs.y() <= maxs.y() && other.maxs.z() <= maxs.z();
    }

    BasicVector3<T> origin()  const { return (mins + maxs) * 0.5; }
    BasicVector3<T> extents() const { return (maxs - mins) * 0.5; }
};

using MinMax = MinMax___<float>;


template<typename T>
struct MinMax2D___ {
    BasicVector2<T> mins;
    BasicVector2<T> maxs;

    MinMax2D___() :
        mins(std::numeric_limits<T>::max()),
        maxs(std::numeric_limits<T>::lowest()) {}

    template<typename U>
    MinMax2D___(const BasicVector2<U> &min, const BasicVector2<U> &max) :
        mins(min), maxs(max) {}

    void clear() { *this = MinMax2D___(); }
    bool valid() const { return mins.x() < maxs.x() && mins.y() < maxs.y(); }

    template<typename U>
    void extend(const BasicVector2<U>& point) {
        for (size_t i = 0; i < 2; ++i) {
            const auto val = point[i];
            if (val < mins[i]) { mins[i] = val; }
            if (val > maxs[i]) { maxs[i] = val; }
        }
    }

    T area() const {
        T width  = maxs.x() - mins.x();
        T height = maxs.y() - mins.y();
        return width * height;
    }

    template<typename U>
    void extend(const MinMax2D___<U> &other) {
        extend(other.mins);
        extend(other.maxs);
    }

    template<typename U>  // true, if point is within the bounds
    bool test(const BasicVector2<U> &point) const {
        return point.x() >= mins.x() && point.y() >= mins.y()
            && point.x() <= maxs.x() && point.y() <= maxs.y();
    }

    template<typename U, typename E>  // true, if point is within the bounds expanded by epsilon
    bool test( const BasicVector2<U> &point, const E epsilon ) const {
        return point.x() >= mins.x() - epsilon && point.y() >= mins.y() - epsilon
            && point.x() <= maxs.x() + epsilon && point.y() <= maxs.y() + epsilon;
    }
    template<typename U>  // true, if there is an intersection
    bool test(const MinMax2D___<U> &other) const {
        return other.maxs.x() >= mins.x() && other.maxs.y() >= mins.y()
            && other.mins.x() <= maxs.x() && other.mins.y() <= maxs.y();
    }

    template<typename U>  // true, if other is completely enclosed by this
    bool surrounds(const MinMax2D___<U> &other) const {
        return other.mins.x() >= mins.x() && other.mins.y() >= mins.y()
            && other.maxs.x() <= maxs.x() && other.maxs.y() <= maxs.y();
    }

    BasicVector2<T> origin()  const { return (mins + maxs) * 0.5; }
    BasicVector2<T> extents() const { return (maxs - mins) * 0.5; }
};

using MinMax2D = MinMax2D___<float>;



template<typename T>
struct Color4___ : public BasicVector4<T> {
    using BasicVector4<T>::BasicVector4;

    Color4___( const BasicVector4<T>& vector ) :
        BasicVector4<T>(vector) {}

          BasicVector3<T> &rgb()       { return this->vec3(); }
    const BasicVector3<T> &rgb() const { return this->vec3(); }
          T &alpha()       { return this->w(); }
    const T &alpha() const { return this->w(); }
};

using Color4f = Color4___<float>;
using Color4b = Color4___<byte>;
using Vector3b = BasicVector3<byte>;


inline byte color_to_byte(float color) {
    return std::clamp(color, 0.f, 255.f);
}


inline Vector3b color_to_byte(const Vector3 &color) {
    return Vector3b(color_to_byte(color.x()), color_to_byte(color.y()), color_to_byte(color.z()));
}


inline Color4b color_to_byte(const Color4f &color) {
    return Color4b(color_to_byte(color.rgb()), color_to_byte(color.alpha()));
}


template<typename T>
T VectorNormalize(BasicVector3<T> &vector) {
    const DoubleVector3 v(vector);  // intermediate vector to be sure to do in double
    const double length = vector3_length(v);
    if (length == 0) {
        vector.set(0);
        return 0;
    } else {
        vector = v / length;
        return length;
    }
}


template<typename T>
BasicVector3<T> VectorNormalized(const BasicVector3<T> &vector) {
    BasicVector3<T> vec(vector);
    VectorNormalize(vec);
    return vec;
}


const float EQUAL_EPSILON = 0.001;


inline bool VectorCompare(const Vector3 &v1, const Vector3 &v2){
    return vector3_equal_epsilon( v1, v2, EQUAL_EPSILON);
}


template<typename T>
T VectorMax(const BasicVector3<T> &v){
    return (v[0] > v[1]) ? ((v[0] > v[2]) ? v[0] : v[2]) : ((v[1] > v[2]) ? v[1] : v[2]);
}


inline bool VectorIsOnAxis(const Vector3 &v) {
    int zeroComponentCount = 0;
    for (int i = 0; i < 3; ++i) { if ( v[i] == 0.0 ) { zeroComponentCount++; } }
    return zeroComponentCount > 1;  // The zero vector will be on axis.
}


/* (pitch yaw roll) -> (roll pitch yaw) */
inline Vector3 angles_pyr2rpy(const Vector3 &angles) {
    return Vector3(angles.z(), angles.x(), angles.y());
}


/*
   =====================
   PlaneFromPoints

   Returns false if the triangle is degenrate.
   The normal will point out of the clock for clockwise ordered points
   =====================
 */
template<typename P, typename V>
bool PlaneFromPoints(Plane3___<P> &plane, const BasicVector3<V> &p0, const BasicVector3<V> &p1, const BasicVector3<V> &p2) {
    plane.normal() = vector3_cross(p2 - p0, p1 - p0);
    if (VectorNormalize(plane.normal()) == 0 ) {
        plane.dist() = 0;
        return false;
    } else {
        plane.dist() = vector3_dot(p0, plane.normal());
        return true;
    }
}

template<typename P, typename V>
bool PlaneFromPoints(Plane3___<P> &plane, const BasicVector3<V> planepts[3]) {
    return PlaneFromPoints(plane, planepts[0], planepts[1], planepts[2]);
}


/*
   ComputeAxisBase()
   computes the base texture axis for brush primitive texturing
   note: ComputeAxisBase here and in editor code must always BE THE SAME!
   warning: special case behaviour of atan2( y, x ) <-> atan( y / x ) might not be the same everywhere when x == 0
   rotation by (0,RotY,RotZ) assigns X to normal
 */

template <typename Element, typename OtherElement>
inline void ComputeAxisBase(const BasicVector3<Element> &normal, BasicVector3<OtherElement> &texS, BasicVector3<OtherElement> &texT) {
    const BasicVector3<Element> up(  0, 0,  1);
    const BasicVector3<Element> down(0, 0, -1);
    if        (vector3_equal_epsilon(normal,   up, Element(1e-6))) {
        texS = BasicVector3<OtherElement>( 0, 1, 0);
        texT = BasicVector3<OtherElement>( 1, 0, 0);
    } else if (vector3_equal_epsilon(normal, down, Element(1e-6))) {
        texS = BasicVector3<OtherElement>( 0, 1, 0);
        texT = BasicVector3<OtherElement>(-1, 0, 0);
    } else {
        texS = vector3_normalised(vector3_cross(normal, up));
        texT = vector3_normalised(vector3_cross(normal, texS));
        vector3_negate(texS);
    }
}


/*
   ================
   MakeNormalVectors

   Given a normalized forward vector, create two
   other perpendicular vectors
   ================
 */
inline void MakeNormalVectors(const Vector3 &forward, Vector3 &right, Vector3 &up) {
    right = VectorNormalized(vector3_cross(g_vector3_axes[vector3_min_abs_component_index(forward)], forward));
    up = vector3_cross(right, forward);
}


/*
** NormalToLatLong
**
** We use two byte encoded normals in some space critical applications.
** Lat = 0 at (1,0,0) to 360 (-1,0,0), encoded in 8-bit sine table format
** Lng = 0 at (0,0,1) to 180 (0,0,-1), encoded in 8-bit sine table format
**
*/
inline void NormalToLatLong( const Vector3& normal, byte bytes[2] ) {
    if (normal[0] == 0 && normal[1] == 0) {  // check for singularities
        if (normal[2] > 0) {  // lat = 0, long = 0
            bytes[0] = 0;
            bytes[1] = 0;
        } else {  // lat = 0, long = 128
            bytes[0] = 128;
            bytes[1] = 0;
        }
    } else {
        const int a = radians_to_degrees(atan2(normal[1], normal[0])) * (255.0 / 360.0);
        const int b = radians_to_degrees(acos(normal[2])) * (255.0 / 360.0);

        bytes[0] = b & 0xFF;  // longitude
        bytes[1] = a & 0xFF;  // latitude
    }
}


// plane types are used to speed some tests
// 0-2 are axial planes
enum EPlaneType : int {
    ePlaneX = 0,
    ePlaneY = 1,
    ePlaneZ = 2,
    ePlaneNonAxial = 3
};


inline EPlaneType PlaneTypeForNormal(const Vector3 &normal) {
    if      (normal[0] == 1.0 || normal[0] == -1.0) { return ePlaneX; }
    else if (normal[1] == 1.0 || normal[1] == -1.0) { return ePlaneY; }
    else if (normal[2] == 1.0 || normal[2] == -1.0) { return ePlaneZ; }
    else                                            { return ePlaneNonAxial; }
}


inline void ColorNormalize(Vector3 &color) {
    const float max = VectorMax(color);
    if (max == 0) { color.set(1); }
    else          { color *= (1.f / max); }
}
