// Simple example: Prove you know x such that x^2 = 9, without revealing x = 3

#include <iostream>
#include "../src/field.h"
#include "../src/elliptic_curve.h"
#include "../src/r1cs.h"
#include "../src/qap.h"
#include "../src/zksnark.h"

int main() {
    std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   Simple zkSNARK Example: x² = 9              ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
    
    try {
        // ============================================================
        // STEP 1: Define the computation as R1CS
        // ============================================================
        // We want to prove: x * x = 9
        // Variables: [1, x, out] where out = 9
        // Single constraint: x * x = out
        
        std::cout << "\n[Step 1] Creating R1CS for x² = 9" << std::endl;
        std::cout << "─────────────────────────────────" << std::endl;
        
        int num_variables = 3;  // [one, x, out]
        int num_constraints = 1; // Just one constraint: x * x = out
        R1CS r1cs(num_variables, num_constraints);
        
        // Secret: x = 3
        FieldElement x(3);
        FieldElement out(9);
        
        std::cout << "Secret input: x = " << x << std::endl;
        std::cout << "Public output: " << out << std::endl;
        
        // Witness = [1, 3, 9]
        std::vector<FieldElement> witness = {
            FieldElement(1),   // index 0: constant 1
            x,                 // index 1: x (secret!)
            out                // index 2: out (public)
        };
        
        // Constraint: x * x = out
        // A = [0, 1, 0] → selects witness[1] = x
        // B = [0, 1, 0] → selects witness[1] = x
        // C = [0, 0, 1] → selects witness[2] = out
        std::vector<FieldElement> a = {FieldElement(0), FieldElement(1), FieldElement(0)};
        std::vector<FieldElement> b = {FieldElement(0), FieldElement(1), FieldElement(0)};
        std::vector<FieldElement> c = {FieldElement(0), FieldElement(0), FieldElement(1)};
        
        r1cs.setConstraint(0, a, b, c);
        
        std::cout << "\nConstraint 0: x * x = out" << std::endl;
        std::cout << "  A·witness = 0*1 + 1*3 + 0*9 = 3" << std::endl;
        std::cout << "  B·witness = 0*1 + 1*3 + 0*9 = 3" << std::endl;
        std::cout << "  C·witness = 0*1 + 0*3 + 1*9 = 9" << std::endl;
        std::cout << "  Check: 3 * 3 = 9 ✓" << std::endl;
        
        // Verify witness satisfies R1CS
        bool r1cs_valid = r1cs.verify(witness);
        if (!r1cs_valid) {
            std::cout << "\n✗ R1CS verification failed!" << std::endl;
            return 1;
        }
        std::cout << "\n✓ R1CS is satisfied by witness" << std::endl;
        
        // ============================================================
        // STEP 2: Convert R1CS to QAP
        // ============================================================
        std::cout << "\n[Step 2] Converting R1CS to QAP" << std::endl;
        std::cout << "─────────────────────────────────" << std::endl;
        
        QAP qap = QAP::fromR1CS(r1cs);
        
        std::cout << "\nQAP polynomials created:" << std::endl;
        std::cout << "  Each of 3 variables → 3 polynomials (A, B, C)" << std::endl;
        std::cout << "  Total: 9 polynomials" << std::endl;
        std::cout << "  Target polynomial Z(x) = (x - 1)" << std::endl;
        
        // Compute A(x), B(x), C(x) using witness
        Polynomial A_poly, B_poly, C_poly;
        qap.computePolynomials(witness, A_poly, B_poly, C_poly);
        
        std::cout << "\nComputed from witness:" << std::endl;
        std::cout << "  A(x) = Σᵢ witnessᵢ·Aᵢ(x) = ";
        A_poly.print();
        std::cout << std::endl;
        
        std::cout << "  B(x) = Σᵢ witnessᵢ·Bᵢ(x) = ";
        B_poly.print();
        std::cout << std::endl;
        
        std::cout << "  C(x) = Σᵢ witnessᵢ·Cᵢ(x) = ";
        C_poly.print();
        std::cout << std::endl;
        
        // Key property: At evaluation point x=1 (constraint 0)
        FieldElement eval_point(1);
        FieldElement a_val = A_poly.evaluate(eval_point);
        FieldElement b_val = B_poly.evaluate(eval_point);
        FieldElement c_val = C_poly.evaluate(eval_point);
        
        std::cout << "\nAt evaluation point x=1:" << std::endl;
        std::cout << "  A(1) = " << a_val << std::endl;
        std::cout << "  B(1) = " << b_val << std::endl;
        std::cout << "  C(1) = " << c_val << std::endl;
        std::cout << "  A(1)·B(1) = " << (a_val * b_val) << " = C(1) ✓" << std::endl;
        
        // ============================================================
        // STEP 3: Trusted Setup - Generate Keys
        // ============================================================
        std::cout << "\n[Step 3] Trusted Setup" << std::endl;
        std::cout << "─────────────────────────────────" << std::endl;
        
        ProvingKey pk;
        VerificationKey vk;
        int num_public_inputs = 1; // Just the output
        
        std::cout << "\nGenerating proving and verification keys..." << std::endl;
        std::cout << "(In real systems, this uses secure MPC ceremony)" << std::endl;
        
        zkSNARK::setup(qap, r1cs, pk, vk, num_public_inputs);
        
        std::cout << "\n✓ Keys generated successfully" << std::endl;
        std::cout << "  Proving key: " << pk.A_query.size() << " EC points" << std::endl;
        std::cout << "  Verification key: " << vk.IC.size() << " IC points" << std::endl;
        
        // ============================================================
        // STEP 4: Prover Creates Proof
        // ============================================================
        std::cout << "\n[Step 4] Prover Creates Proof" << std::endl;
        std::cout << "─────────────────────────────────" << std::endl;
        
        std::vector<FieldElement> public_inputs = {out};
        
        std::cout << "\nProver's knowledge:" << std::endl;
        std::cout << "  ✓ Secret input: x = " << x << std::endl;
        std::cout << "  ✓ Public output: " << out << std::endl;
        std::cout << "  ✓ Full witness: [1, " << x << ", " << out << "]" << std::endl;
        
        std::cout << "\nGenerating proof..." << std::endl;
        Proof proof = zkSNARK::prove(qap, pk, witness, public_inputs);
        
        std::cout << "\n✓ Proof generated:" << std::endl;
        std::cout << "  Size: 3 elliptic curve points" << std::endl;
        std::cout << "  Proof.A = " << proof.A << std::endl;
        std::cout << "  Proof.B = " << proof.B << std::endl;
        std::cout << "  Proof.C = " << proof.C << std::endl;
        
        // ============================================================
        // STEP 5: Verifier Checks Proof
        // ============================================================
        std::cout << "\n[Step 5] Verifier Checks Proof" << std::endl;
        std::cout << "─────────────────────────────────" << std::endl;
        
        std::cout << "\nVerifier's knowledge:" << std::endl;
        std::cout << "  ✓ Public output: " << out << std::endl;
        std::cout << "  ✗ Secret input: UNKNOWN (this is the point!)" << std::endl;
        std::cout << "  ✓ Proof from prover: 3 EC points" << std::endl;
        std::cout << "  ✓ Verification key" << std::endl;
        
        std::cout << "\nVerifying proof..." << std::endl;
        bool proof_valid = zkSNARK::verify(vk, proof, public_inputs);
        
        // ============================================================
        // RESULT
        // ============================================================
        std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    RESULT                      ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
        
        if (proof_valid) {
            std::cout << "\n🎉 SUCCESS! Proof verified! 🎉" << std::endl;
            std::cout << "\n═══ What Just Happened? ═══" << std::endl;
            std::cout << "\n1. The PROVER knows x = 3" << std::endl;
            std::cout << "2. The PROVER proved that x² = 9" << std::endl;
            std::cout << "3. The VERIFIER confirmed the proof is valid" << std::endl;
            std::cout << "4. The VERIFIER learned NOTHING about x!" << std::endl;
            std::cout << "\n═══ Zero-Knowledge Property ═══" << std::endl;
            std::cout << "The verifier is 100% convinced that:" << std::endl;
            std::cout << "  ✓ The prover knows some x" << std::endl;
            std::cout << "  ✓ That x satisfies x² = 9" << std::endl;
            std::cout << "\nBut the verifier learned:" << std::endl;
            std::cout << "  ✗ NOTHING about what x actually is!" << std::endl;
            std::cout << "  ✗ Not even whether x = 3 or x = -3" << std::endl;
            std::cout << "\nThis is the magic of Zero-Knowledge Proofs! ✨" << std::endl;
            
        } else {
            std::cout << "\n✗ FAILED! Proof verification failed!" << std::endl;
        }
        
        std::cout << "\n════════════════════════════════════════════════" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
