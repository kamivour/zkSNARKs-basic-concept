#ifndef ELLIPTIC_CURVE_H
#define ELLIPTIC_CURVE_H

#include "field.h"
#include <iostream>

// Elliptic curve point in Weierstrass form: y^2 = x^3 + ax + b
// Using a simplified curve for educational purposes
class ECPoint {
private:
    FieldElement x, y;
    bool is_infinity;
    
    // Curve parameters: y^2 = x^3 + ax + b
    static FieldElement a;
    static FieldElement b;
    
public:
    ECPoint() : x(0), y(0), is_infinity(true) {}
    
    ECPoint(const FieldElement& x_val, const FieldElement& y_val) 
        : x(x_val), y(y_val), is_infinity(false) {
        // Verify point is on curve
        FieldElement left = y * y;
        FieldElement right = x * x * x + a * x + b;
        if (left != right) {
            std::cout << "Warning: Point (" << x << ", " << y << ") may not be on curve" << std::endl;
        }
    }
    
    static void setCurveParams(const FieldElement& a_val, const FieldElement& b_val) {
        a = a_val;
        b = b_val;
    }
    
    bool isInfinity() const { return is_infinity; }
    FieldElement getX() const { return x; }
    FieldElement getY() const { return y; }
    
    // Point addition
    ECPoint operator+(const ECPoint& other) const {
        if (is_infinity) return other;
        if (other.is_infinity) return *this;
        
        // If points have same x but different y, result is point at infinity
        if (x == other.x && y != other.y) {
            return ECPoint();
        }
        
        FieldElement lambda;
        
        if (x == other.x && y == other.y) {
            // Point doubling
            FieldElement numerator = x * x * FieldElement(3) + a;
            FieldElement denominator = y * FieldElement(2);
            lambda = numerator / denominator;
        } else {
            // Point addition
            FieldElement numerator = other.y - y;
            FieldElement denominator = other.x - x;
            lambda = numerator / denominator;
        }
        
        FieldElement x_new = lambda * lambda - x - other.x;
        FieldElement y_new = lambda * (x - x_new) - y;
        
        return ECPoint(x_new, y_new);
    }
    
    // Scalar multiplication (double-and-add algorithm)
    ECPoint operator*(uint64_t scalar) const {
        if (scalar == 0 || is_infinity) {
            return ECPoint();
        }
        
        ECPoint result;
        ECPoint base = *this;
        
        while (scalar > 0) {
            if (scalar & 1) {
                result = result + base;
            }
            base = base + base;
            scalar >>= 1;
        }
        
        return result;
    }
    
    bool operator==(const ECPoint& other) const {
        if (is_infinity && other.is_infinity) return true;
        if (is_infinity || other.is_infinity) return false;
        return (x == other.x) && (y == other.y);
    }
    
    friend std::ostream& operator<<(std::ostream& os, const ECPoint& point) {
        if (point.is_infinity) {
            os << "Point at Infinity";
        } else {
            os << "(" << point.x << ", " << point.y << ")";
        }
        return os;
    }
};

// Initialize static members
FieldElement ECPoint::a(0);
FieldElement ECPoint::b(7);

#endif // ELLIPTIC_CURVE_H
