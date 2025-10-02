#include <iostream>
#include <vector>
#include "field.h"
#include "elliptic_curve.h"
#include "r1cs.h"
#include "qap.h"
#include "zksnark.h"

// Example: Prove knowledge of x such that x^3 + x + 5 = 35
// This translates to: x^3 + x + 5 = 35
// Let's use x = 3: 3^3 + 3 + 5 = 27 + 3 + 5 = 35 ✓
//
// We need to flatten this into R1CS constraints:
// Witness: [1, x, out, v1, v2]
// where:
//   v1 = x * x       (constraint 0)
//   v2 = v1 * x      (constraint 1) 
//   v2 + x + 5 = out (constraint 2)
//
// Variables layout:
// 0: one (constant 1)
// 1: x (input)
// 2: out (output)
// 3: v1 (x^2)
// 4: v2 (x^3)

void createCubicR1CS(R1CS& r1cs, std::vector<FieldElement>& witness, 
                     FieldElement x, FieldElement out) {
    std::cout << "\n=== Creating R1CS for x^3 + x + 5 = out ===" << std::endl;
    std::cout << "Input x = " << x << std::endl;
    std::cout << "Output = " << out << std::endl;
    
    // Calculate intermediate values
    FieldElement v1 = x * x;        // x^2
    FieldElement v2 = v1 * x;       // x^3
    
    std::cout << "v1 (x^2) = " << v1 << std::endl;
    std::cout << "v2 (x^3) = " << v2 << std::endl;
    
    // Witness: [1, x, out, v1, v2]
    witness = {
        FieldElement(1),  // 0: one
        x,                // 1: x
        out,              // 2: out
        v1,               // 3: v1 = x^2
        v2                // 4: v2 = x^3
    };
    
    std::cout << "\nWitness vector: [";
    for (size_t i = 0; i < witness.size(); i++) {
        std::cout << witness[i];
        if (i < witness.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    
    // Constraint 0: x * x = v1
    // A = [0, 1, 0, 0, 0] (selects x)
    // B = [0, 1, 0, 0, 0] (selects x)
    // C = [0, 0, 0, 1, 0] (selects v1)
    std::vector<FieldElement> a0 = {FieldElement(0), FieldElement(1), FieldElement(0), 
                                     FieldElement(0), FieldElement(0)};
    std::vector<FieldElement> b0 = {FieldElement(0), FieldElement(1), FieldElement(0), 
                                     FieldElement(0), FieldElement(0)};
    std::vector<FieldElement> c0 = {FieldElement(0), FieldElement(0), FieldElement(0), 
                                     FieldElement(1), FieldElement(0)};
    r1cs.setConstraint(0, a0, b0, c0);
    std::cout << "\nConstraint 0: x * x = v1" << std::endl;
    
    // Constraint 1: v1 * x = v2
    // A = [0, 0, 0, 1, 0] (selects v1)
    // B = [0, 1, 0, 0, 0] (selects x)
    // C = [0, 0, 0, 0, 1] (selects v2)
    std::vector<FieldElement> a1 = {FieldElement(0), FieldElement(0), FieldElement(0), 
                                     FieldElement(1), FieldElement(0)};
    std::vector<FieldElement> b1 = {FieldElement(0), FieldElement(1), FieldElement(0), 
                                     FieldElement(0), FieldElement(0)};
    std::vector<FieldElement> c1 = {FieldElement(0), FieldElement(0), FieldElement(0), 
                                     FieldElement(0), FieldElement(1)};
    r1cs.setConstraint(1, a1, b1, c1);
    std::cout << "Constraint 1: v1 * x = v2" << std::endl;
    
    // Constraint 2: (v2 + x + 5) * 1 = out
    // A = [5, 1, 0, 0, 1] (computes v2 + x + 5)
    // B = [1, 0, 0, 0, 0] (selects 1)
    // C = [0, 0, 1, 0, 0] (selects out)
    std::vector<FieldElement> a2 = {FieldElement(5), FieldElement(1), FieldElement(0), 
                                     FieldElement(0), FieldElement(1)};
    std::vector<FieldElement> b2 = {FieldElement(1), FieldElement(0), FieldElement(0), 
                                     FieldElement(0), FieldElement(0)};
    std::vector<FieldElement> c2 = {FieldElement(0), FieldElement(0), FieldElement(1), 
                                     FieldElement(0), FieldElement(0)};
    r1cs.setConstraint(2, a2, b2, c2);
    std::cout << "Constraint 2: (v2 + x + 5) * 1 = out" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  zkSNARKs Implementation from Scratch  " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nField Prime: " << FieldElement::getPrime() << std::endl;
    
    try {
        // Step 1: Create R1CS
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "STEP 1: Create R1CS" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        int num_variables = 5;
        int num_constraints = 3;
        R1CS r1cs(num_variables, num_constraints);
        
        FieldElement x(3);      // Secret input
        FieldElement out(35);   // Public output
        
        std::vector<FieldElement> witness;
        createCubicR1CS(r1cs, witness, x, out);
        
        r1cs.print();
        
        // Verify R1CS
        bool r1cs_valid = r1cs.verify(witness);
        std::cout << "\nR1CS Validation: " << (r1cs_valid ? "✓ PASSED" : "✗ FAILED") << std::endl;
        
        if (!r1cs_valid) {
            std::cout << "R1CS verification failed! Exiting." << std::endl;
            return 1;
        }
        
        // Step 2: Convert R1CS to QAP
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "STEP 2: Convert R1CS to QAP" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        QAP qap = QAP::fromR1CS(r1cs);
        
        std::cout << "\nQAP Created with " << qap.num_variables << " polynomials" << std::endl;
        
        // Verify QAP by evaluating at constraint points
        std::cout << "\n=== Verifying QAP Conversion ===" << std::endl;
        Polynomial A_poly, B_poly, C_poly;
        qap.computePolynomials(witness, A_poly, B_poly, C_poly);
        
        for (int i = 0; i < num_constraints; i++) {
            FieldElement eval_point(i + 1);
            FieldElement a_val = A_poly.evaluate(eval_point);
            FieldElement b_val = B_poly.evaluate(eval_point);
            FieldElement c_val = C_poly.evaluate(eval_point);
            FieldElement z_val = qap.Z.evaluate(eval_point);
            
            std::cout << "At x=" << eval_point << ": ";
            std::cout << "A=" << a_val << ", B=" << b_val << ", C=" << c_val;
            std::cout << ", Z=" << z_val << std::endl;
            
            // At constraint points, Z should be 0
            if (z_val != FieldElement(0)) {
                std::cout << "  Warning: Z should be 0 at constraint points" << std::endl;
            }
        }
        
        // Step 3: Setup - Generate keys
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "STEP 3: Setup Phase - Generate Keys" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        ProvingKey pk;
        VerificationKey vk;
        int num_public_inputs = 1; // Just the output
        
        zkSNARK::setup(qap, r1cs, pk, vk, num_public_inputs);
        
        std::cout << "\nProving key size: " << pk.A_query.size() << " elements" << std::endl;
        std::cout << "Verification key IC size: " << vk.IC.size() << " elements" << std::endl;
        
        // Step 4: Prove
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "STEP 4: Prover Generates Proof" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        std::vector<FieldElement> public_inputs = {out};
        std::cout << "\nProver knows secret x = " << x << std::endl;
        std::cout << "Public output = " << out << std::endl;
        
        Proof proof = zkSNARK::prove(qap, pk, witness, public_inputs);
        
        std::cout << "\n=== Proof Generated ===" << std::endl;
        std::cout << "Proof.A: " << proof.A << std::endl;
        std::cout << "Proof.B: " << proof.B << std::endl;
        std::cout << "Proof.C: " << proof.C << std::endl;
        
        // Step 5: Verify
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "STEP 5: Verifier Checks Proof" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        std::cout << "\nVerifier only knows: output = " << out << std::endl;
        std::cout << "Verifier does NOT know: x (secret input)" << std::endl;
        
        bool proof_valid = zkSNARK::verify(vk, proof, public_inputs);
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "FINAL RESULT" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        if (proof_valid) {
            std::cout << "\n✓✓✓ PROOF VERIFIED SUCCESSFULLY! ✓✓✓" << std::endl;
            std::cout << "\nThe verifier is convinced that the prover knows" << std::endl;
            std::cout << "a value x such that x^3 + x + 5 = 35" << std::endl;
            std::cout << "WITHOUT learning what x is!" << std::endl;
            std::cout << "(x = " << x << ", but verifier doesn't know this)" << std::endl;
        } else {
            std::cout << "\n✗✗✗ PROOF VERIFICATION FAILED! ✗✗✗" << std::endl;
        }
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "  zkSNARKs Demo Complete!" << std::endl;
        std::cout << "========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
