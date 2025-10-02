#ifndef R1CS_H
#define R1CS_H

#include "field.h"
#include <vector>
#include <iostream>
#include <iomanip>

// R1CS: Rank-1 Constraint System
// Represents constraints of the form: (A·s) * (B·s) = (C·s)
// where s is the solution vector
class R1CS {
public:
    std::vector<std::vector<FieldElement>> A;
    std::vector<std::vector<FieldElement>> B;
    std::vector<std::vector<FieldElement>> C;
    int num_variables;
    int num_constraints;
    
    R1CS(int vars, int constraints) 
        : num_variables(vars), num_constraints(constraints) {
        A.resize(constraints, std::vector<FieldElement>(vars, FieldElement(0)));
        B.resize(constraints, std::vector<FieldElement>(vars, FieldElement(0)));
        C.resize(constraints, std::vector<FieldElement>(vars, FieldElement(0)));
    }
    
    void setConstraint(int idx, 
                      const std::vector<FieldElement>& a,
                      const std::vector<FieldElement>& b,
                      const std::vector<FieldElement>& c) {
        if (idx >= num_constraints) {
            throw std::runtime_error("Constraint index out of bounds");
        }
        A[idx] = a;
        B[idx] = b;
        C[idx] = c;
    }
    
    // Verify that a witness satisfies the R1CS
    bool verify(const std::vector<FieldElement>& witness) const {
        if (witness.size() != num_variables) {
            std::cout << "Witness size mismatch: expected " << num_variables 
                     << ", got " << witness.size() << std::endl;
            return false;
        }
        
        std::cout << "\n=== Verifying R1CS Constraints ===" << std::endl;
        
        for (int i = 0; i < num_constraints; i++) {
            FieldElement a_val(0), b_val(0), c_val(0);
            
            // Calculate A·witness
            for (int j = 0; j < num_variables; j++) {
                a_val = a_val + A[i][j] * witness[j];
            }
            
            // Calculate B·witness
            for (int j = 0; j < num_variables; j++) {
                b_val = b_val + B[i][j] * witness[j];
            }
            
            // Calculate C·witness
            for (int j = 0; j < num_variables; j++) {
                c_val = c_val + C[i][j] * witness[j];
            }
            
            std::cout << "Constraint " << i << ": (" << a_val << ") * (" 
                     << b_val << ") = " << (a_val * b_val) << " ?= " << c_val;
            
            if ((a_val * b_val) != c_val) {
                std::cout << " [FAIL]" << std::endl;
                return false;
            }
            std::cout << " [OK]" << std::endl;
        }
        
        return true;
    }
    
    void print() const {
        std::cout << "\n=== R1CS System ===" << std::endl;
        std::cout << "Variables: " << num_variables << std::endl;
        std::cout << "Constraints: " << num_constraints << std::endl;
        
        for (int i = 0; i < num_constraints; i++) {
            std::cout << "\nConstraint " << i << ":" << std::endl;
            std::cout << "  A: [";
            for (int j = 0; j < num_variables; j++) {
                std::cout << std::setw(4) << A[i][j];
                if (j < num_variables - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
            
            std::cout << "  B: [";
            for (int j = 0; j < num_variables; j++) {
                std::cout << std::setw(4) << B[i][j];
                if (j < num_variables - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
            
            std::cout << "  C: [";
            for (int j = 0; j < num_variables; j++) {
                std::cout << std::setw(4) << C[i][j];
                if (j < num_variables - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
        }
    }
};

#endif // R1CS_H
