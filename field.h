#ifndef FIELD_H
#define FIELD_H

#include <iostream>
#include <string>
#include <cstdint>

// Simplified field arithmetic over a small prime (for educational purposes)
// Using a 64-bit prime for simplicity
class FieldElement {
private:
    uint64_t value;
    static const uint64_t PRIME = 21888242871839275222246405745257275088548364400416034343698204186575808495617ULL;
    // For simplicity, using a smaller prime that fits in 64 bits
    static const uint64_t SMALL_PRIME = 2147483647ULL; // 2^31 - 1 (Mersenne prime)

public:
    FieldElement() : value(0) {}
    FieldElement(uint64_t val) : value(val % SMALL_PRIME) {}
    
    uint64_t getValue() const { return value; }
    
    // Addition
    FieldElement operator+(const FieldElement& other) const {
        return FieldElement((value + other.value) % SMALL_PRIME);
    }
    
    // Subtraction
    FieldElement operator-(const FieldElement& other) const {
        return FieldElement((value + SMALL_PRIME - other.value) % SMALL_PRIME);
    }
    
    // Multiplication
    FieldElement operator*(const FieldElement& other) const {
        __uint128_t temp = (__uint128_t)value * (__uint128_t)other.value;
        return FieldElement(temp % SMALL_PRIME);
    }
    
    // Division (multiplication by modular inverse)
    FieldElement operator/(const FieldElement& other) const {
        return (*this) * other.inverse();
    }
    
    // Modular inverse using Extended Euclidean Algorithm
    FieldElement inverse() const {
        if (value == 0) {
            throw std::runtime_error("Cannot invert zero");
        }
        
        int64_t t = 0, newt = 1;
        int64_t r = SMALL_PRIME, newr = value;
        
        while (newr != 0) {
            int64_t quotient = r / newr;
            
            int64_t temp = t;
            t = newt;
            newt = temp - quotient * newt;
            
            temp = r;
            r = newr;
            newr = temp - quotient * newr;
        }
        
        if (r > 1) {
            throw std::runtime_error("Value is not invertible");
        }
        if (t < 0) {
            t = t + SMALL_PRIME;
        }
        
        return FieldElement(t);
    }
    
    // Power (for elliptic curve operations)
    FieldElement power(uint64_t exp) const {
        if (exp == 0) return FieldElement(1);
        
        FieldElement result(1);
        FieldElement base = *this;
        
        while (exp > 0) {
            if (exp & 1) {
                result = result * base;
            }
            base = base * base;
            exp >>= 1;
        }
        
        return result;
    }
    
    bool operator==(const FieldElement& other) const {
        return value == other.value;
    }
    
    bool operator!=(const FieldElement& other) const {
        return value != other.value;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const FieldElement& fe) {
        os << fe.value;
        return os;
    }
    
    static uint64_t getPrime() { return SMALL_PRIME; }
};

#endif // FIELD_H
