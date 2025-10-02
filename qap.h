#ifndef QAP_H
#define QAP_H

#include "field.h"
#include "r1cs.h"
#include <vector>
#include <iostream>

// Polynomial representation
class Polynomial {
public:
    std::vector<FieldElement> coefficients; // coefficients[i] is the coefficient of x^i
    
    Polynomial() {}
    Polynomial(const std::vector<FieldElement>& coeffs) : coefficients(coeffs) {}
    
    // Evaluate polynomial at point x
    FieldElement evaluate(const FieldElement& x) const {
        FieldElement result(0);
        FieldElement x_power(1);
        
        for (const auto& coeff : coefficients) {
            result = result + coeff * x_power;
            x_power = x_power * x;
        }
        
        return result;
    }
    
    // Add polynomials
    Polynomial operator+(const Polynomial& other) const {
        int max_size = std::max(coefficients.size(), other.coefficients.size());
        std::vector<FieldElement> result_coeffs(max_size, FieldElement(0));
        
        for (size_t i = 0; i < coefficients.size(); i++) {
            result_coeffs[i] = result_coeffs[i] + coefficients[i];
        }
        for (size_t i = 0; i < other.coefficients.size(); i++) {
            result_coeffs[i] = result_coeffs[i] + other.coefficients[i];
        }
        
        return Polynomial(result_coeffs);
    }
    
    // Multiply polynomials
    Polynomial operator*(const Polynomial& other) const {
        if (coefficients.empty() || other.coefficients.empty()) {
            return Polynomial();
        }
        
        std::vector<FieldElement> result_coeffs(
            coefficients.size() + other.coefficients.size() - 1, 
            FieldElement(0)
        );
        
        for (size_t i = 0; i < coefficients.size(); i++) {
            for (size_t j = 0; j < other.coefficients.size(); j++) {
                result_coeffs[i + j] = result_coeffs[i + j] + coefficients[i] * other.coefficients[j];
            }
        }
        
        return Polynomial(result_coeffs);
    }
    
    // Scalar multiplication
    Polynomial operator*(const FieldElement& scalar) const {
        std::vector<FieldElement> result_coeffs(coefficients.size());
        for (size_t i = 0; i < coefficients.size(); i++) {
            result_coeffs[i] = coefficients[i] * scalar;
        }
        return Polynomial(result_coeffs);
    }
    
    void print() const {
        std::cout << "[";
        for (size_t i = 0; i < coefficients.size(); i++) {
            std::cout << coefficients[i];
            if (i < coefficients.size() - 1) std::cout << ", ";
        }
        std::cout << "]";
    }
};

// Lagrange interpolation for creating polynomials
class LagrangeInterpolation {
public:
    // Create Lagrange basis polynomial L_j(x) that is 1 at x_j and 0 at all other x_i
    static Polynomial basisPolynomial(int j, const std::vector<FieldElement>& x_values) {
        Polynomial result({FieldElement(1)});
        
        for (size_t i = 0; i < x_values.size(); i++) {
            if (i == j) continue;
            
            // Multiply by (x - x_i) / (x_j - x_i)
            FieldElement denominator = x_values[j] - x_values[i];
            
            // (x - x_i) represented as [-x_i, 1]
            Polynomial numerator({FieldElement(0) - x_values[i], FieldElement(1)});
            
            result = result * numerator;
            result = result * (FieldElement(1) / denominator);
        }
        
        return result;
    }
    
    // Interpolate polynomial through points (x_i, y_i)
    static Polynomial interpolate(const std::vector<FieldElement>& x_values,
                                  const std::vector<FieldElement>& y_values) {
        if (x_values.size() != y_values.size()) {
            throw std::runtime_error("x and y value sizes must match");
        }
        
        Polynomial result({FieldElement(0)});
        
        for (size_t j = 0; j < x_values.size(); j++) {
            Polynomial basis = basisPolynomial(j, x_values);
            result = result + basis * y_values[j];
        }
        
        return result;
    }
};

// QAP: Quadratic Arithmetic Program
class QAP {
public:
    std::vector<Polynomial> A_polys;
    std::vector<Polynomial> B_polys;
    std::vector<Polynomial> C_polys;
    Polynomial Z; // Target polynomial
    int num_variables;
    
    // Convert R1CS to QAP using Lagrange interpolation
    static QAP fromR1CS(const R1CS& r1cs) {
        std::cout << "\n=== Converting R1CS to QAP ===" << std::endl;
        
        QAP qap;
        qap.num_variables = r1cs.num_variables;
        
        // Create evaluation points (1, 2, 3, ..., m) where m is number of constraints
        std::vector<FieldElement> x_values;
        for (int i = 0; i < r1cs.num_constraints; i++) {
            x_values.push_back(FieldElement(i + 1));
        }
        
        std::cout << "Evaluation points: ";
        for (const auto& x : x_values) {
            std::cout << x << " ";
        }
        std::cout << std::endl;
        
        // For each variable, create polynomials that interpolate constraint values
        for (int var = 0; var < r1cs.num_variables; var++) {
            std::vector<FieldElement> a_values, b_values, c_values;
            
            for (int constraint = 0; constraint < r1cs.num_constraints; constraint++) {
                a_values.push_back(r1cs.A[constraint][var]);
                b_values.push_back(r1cs.B[constraint][var]);
                c_values.push_back(r1cs.C[constraint][var]);
            }
            
            Polynomial a_poly = LagrangeInterpolation::interpolate(x_values, a_values);
            Polynomial b_poly = LagrangeInterpolation::interpolate(x_values, b_values);
            Polynomial c_poly = LagrangeInterpolation::interpolate(x_values, c_values);
            
            qap.A_polys.push_back(a_poly);
            qap.B_polys.push_back(b_poly);
            qap.C_polys.push_back(c_poly);
            
            std::cout << "Variable " << var << " polynomials created" << std::endl;
        }
        
        // Create target polynomial Z(x) = (x - 1)(x - 2)...(x - m)
        qap.Z = Polynomial({FieldElement(1)});
        for (const auto& x : x_values) {
            Polynomial factor({FieldElement(0) - x, FieldElement(1)});
            qap.Z = qap.Z * factor;
        }
        
        std::cout << "Target polynomial Z(x) = ";
        qap.Z.print();
        std::cout << std::endl;
        
        return qap;
    }
    
    // Compute A(x), B(x), C(x) for a given witness
    void computePolynomials(const std::vector<FieldElement>& witness,
                           Polynomial& A_x, Polynomial& B_x, Polynomial& C_x) const {
        A_x = Polynomial({FieldElement(0)});
        B_x = Polynomial({FieldElement(0)});
        C_x = Polynomial({FieldElement(0)});
        
        for (int i = 0; i < num_variables; i++) {
            A_x = A_x + A_polys[i] * witness[i];
            B_x = B_x + B_polys[i] * witness[i];
            C_x = C_x + C_polys[i] * witness[i];
        }
    }
};

#endif // QAP_H
