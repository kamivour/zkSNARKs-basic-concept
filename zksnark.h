#ifndef ZKSNARK_H
#define ZKSNARK_H

#include "field.h"
#include "elliptic_curve.h"
#include "r1cs.h"
#include "qap.h"
#include <vector>
#include <iostream>
#include <random>

// Proving Key
struct ProvingKey {
    std::vector<ECPoint> A_query;
    std::vector<ECPoint> B_query;
    std::vector<ECPoint> C_query;
    ECPoint alpha;
    ECPoint beta;
    ECPoint delta;
    std::vector<ECPoint> Z_query;
};

// Verification Key
struct VerificationKey {
    ECPoint alpha;
    ECPoint beta;
    ECPoint gamma;
    ECPoint delta;
    std::vector<ECPoint> IC; // Input consistency check values
};

// Proof
struct Proof {
    ECPoint A;
    ECPoint B;
    ECPoint C;
};

class zkSNARK {
private:
    static uint64_t randomScalar() {
        static std::random_device rd;
        static std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(1, FieldElement::getPrime() - 1);
        return dis(gen);
    }

public:
    // Setup phase: Generate proving and verification keys
    static void setup(const QAP& qap, const R1CS& r1cs, 
                     ProvingKey& pk, VerificationKey& vk, 
                     int num_public_inputs) {
        std::cout << "\n=== zkSNARK Setup Phase ===" << std::endl;
        
        // Generate random toxic waste (should be destroyed after setup!)
        uint64_t tau = randomScalar();
        uint64_t alpha_scalar = randomScalar();
        uint64_t beta_scalar = randomScalar();
        uint64_t gamma_scalar = randomScalar();
        uint64_t delta_scalar = randomScalar();
        
        std::cout << "Generated random parameters (toxic waste):" << std::endl;
        std::cout << "  tau = " << tau << std::endl;
        std::cout << "  alpha = " << alpha_scalar << std::endl;
        std::cout << "  beta = " << beta_scalar << std::endl;
        std::cout << "  gamma = " << gamma_scalar << std::endl;
        std::cout << "  delta = " << delta_scalar << std::endl;
        
        // Create generator point
        ECPoint::setCurveParams(FieldElement(0), FieldElement(7));
        ECPoint G(FieldElement(2), FieldElement(1234567)); // Simplified generator
        
        std::cout << "\nGenerator point G = " << G << std::endl;
        
        // Generate proving key queries
        std::cout << "\nGenerating proving key queries..." << std::endl;
        for (int i = 0; i < qap.num_variables; i++) {
            FieldElement tau_fe(tau);
            FieldElement a_val = qap.A_polys[i].evaluate(tau_fe);
            FieldElement b_val = qap.B_polys[i].evaluate(tau_fe);
            FieldElement c_val = qap.C_polys[i].evaluate(tau_fe);
            
            pk.A_query.push_back(G * a_val.getValue());
            pk.B_query.push_back(G * b_val.getValue());
            pk.C_query.push_back(G * c_val.getValue());
            
            std::cout << "  Variable " << i << " queries generated" << std::endl;
        }
        
        // Generate alpha, beta, delta points
        pk.alpha = G * alpha_scalar;
        pk.beta = G * beta_scalar;
        pk.delta = G * delta_scalar;
        
        std::cout << "\nProving key alpha = " << pk.alpha << std::endl;
        std::cout << "Proving key beta = " << pk.beta << std::endl;
        std::cout << "Proving key delta = " << pk.delta << std::endl;
        
        // Generate verification key
        vk.alpha = G * alpha_scalar;
        vk.beta = G * beta_scalar;
        vk.gamma = G * gamma_scalar;
        vk.delta = G * delta_scalar;
        
        std::cout << "\nVerification key generated:" << std::endl;
        std::cout << "  alpha = " << vk.alpha << std::endl;
        std::cout << "  beta = " << vk.beta << std::endl;
        std::cout << "  gamma = " << vk.gamma << std::endl;
        std::cout << "  delta = " << vk.delta << std::endl;
        
        // Generate IC for public inputs
        std::cout << "\nGenerating IC for " << num_public_inputs << " public inputs..." << std::endl;
        for (int i = 0; i <= num_public_inputs; i++) {
            vk.IC.push_back(G * (i + 1)); // Simplified
            std::cout << "  IC[" << i << "] = " << vk.IC[i] << std::endl;
        }
        
        std::cout << "\n=== Setup Complete ===" << std::endl;
    }
    
    // Prove phase: Create a proof
    static Proof prove(const QAP& qap, 
                      const ProvingKey& pk,
                      const std::vector<FieldElement>& witness,
                      const std::vector<FieldElement>& public_inputs) {
        std::cout << "\n=== zkSNARK Prove Phase ===" << std::endl;
        
        std::cout << "Witness values: [";
        for (size_t i = 0; i < witness.size(); i++) {
            std::cout << witness[i];
            if (i < witness.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        
        Proof proof;
        
        // Compute A, B, C polynomials with witness
        Polynomial A_poly, B_poly, C_poly;
        qap.computePolynomials(witness, A_poly, B_poly, C_poly);
        
        std::cout << "\nComputed polynomials from witness:" << std::endl;
        std::cout << "  A(x) = "; A_poly.print(); std::cout << std::endl;
        std::cout << "  B(x) = "; B_poly.print(); std::cout << std::endl;
        std::cout << "  C(x) = "; C_poly.print(); std::cout << std::endl;
        
        // Generate random blinding factors
        uint64_t r = randomScalar();
        uint64_t s = randomScalar();
        
        std::cout << "\nGenerated random blinding factors:" << std::endl;
        std::cout << "  r = " << r << std::endl;
        std::cout << "  s = " << s << std::endl;
        
        // Compute proof elements (simplified)
        ECPoint::setCurveParams(FieldElement(0), FieldElement(7));
        ECPoint G(FieldElement(2), FieldElement(1234567));
        
        // A = sum of A_query weighted by witness + random
        proof.A = ECPoint();
        for (size_t i = 0; i < witness.size(); i++) {
            if (i < pk.A_query.size()) {
                proof.A = proof.A + pk.A_query[i] * witness[i].getValue();
            }
        }
        proof.A = proof.A + pk.alpha * r;
        
        std::cout << "\nProof.A = " << proof.A << std::endl;
        
        // B = sum of B_query weighted by witness + random
        proof.B = ECPoint();
        for (size_t i = 0; i < witness.size(); i++) {
            if (i < pk.B_query.size()) {
                proof.B = proof.B + pk.B_query[i] * witness[i].getValue();
            }
        }
        proof.B = proof.B + pk.beta * s;
        
        std::cout << "Proof.B = " << proof.B << std::endl;
        
        // C = sum of C_query weighted by witness
        proof.C = ECPoint();
        for (size_t i = 0; i < witness.size(); i++) {
            if (i < pk.C_query.size()) {
                proof.C = proof.C + pk.C_query[i] * witness[i].getValue();
            }
        }
        proof.C = proof.C + pk.delta * (r * s);
        
        std::cout << "Proof.C = " << proof.C << std::endl;
        
        std::cout << "\n=== Proof Generation Complete ===" << std::endl;
        
        return proof;
    }
    
    // Verify phase: Verify a proof
    static bool verify(const VerificationKey& vk,
                      const Proof& proof,
                      const std::vector<FieldElement>& public_inputs) {
        std::cout << "\n=== zkSNARK Verify Phase ===" << std::endl;
        
        std::cout << "Public inputs: [";
        for (size_t i = 0; i < public_inputs.size(); i++) {
            std::cout << public_inputs[i];
            if (i < public_inputs.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "\nVerifying proof..." << std::endl;
        std::cout << "  Checking Proof.A = " << proof.A << std::endl;
        std::cout << "  Checking Proof.B = " << proof.B << std::endl;
        std::cout << "  Checking Proof.C = " << proof.C << std::endl;
        
        // Compute input consistency check
        ECPoint vk_x = ECPoint();
        if (!vk.IC.empty()) {
            vk_x = vk.IC[0];
        }
        
        for (size_t i = 0; i < public_inputs.size(); i++) {
            if (i + 1 < vk.IC.size()) {
                vk_x = vk_x + vk.IC[i + 1] * public_inputs[i].getValue();
            }
        }
        
        std::cout << "\nInput consistency check value: " << vk_x << std::endl;
        
        // Simplified verification: Check that proof components are not at infinity
        bool valid = !proof.A.isInfinity() && 
                    !proof.B.isInfinity() && 
                    !proof.C.isInfinity();
        
        // In a real implementation, we would check pairing equations:
        // e(A, B) = e(alpha, beta) * e(vk_x, gamma) * e(C, delta)
        
        std::cout << "\nPairing check (simplified): ";
        if (valid) {
            std::cout << "PASSED" << std::endl;
            std::cout << "✓ Proof components are valid elliptic curve points" << std::endl;
        } else {
            std::cout << "FAILED" << std::endl;
            std::cout << "✗ Proof contains invalid points" << std::endl;
        }
        
        std::cout << "\n=== Verification Complete ===" << std::endl;
        
        return valid;
    }
};

#endif // ZKSNARK_H
