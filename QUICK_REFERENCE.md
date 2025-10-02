# zkSNARKs Quick Reference Guide

## 🚀 Quick Commands

### Compile Main Example
```powershell
g++ -std=c++17 main.cpp -o zksnark.exe
```

### Compile Simple Example
```powershell
g++ -std=c++17 simple_example.cpp -o simple.exe
```

### Run Examples
```powershell
.\zksnark.exe    # Full example: x³ + x + 5 = 35
.\simple.exe     # Simple example: x² = 9
```

### Run Build Script
```powershell
.\build_and_run.ps1
```

## 📚 Core Concepts Cheat Sheet

### R1CS (Rank-1 Constraint System)
- **Format**: (A·s) * (B·s) = (C·s)
- **Components**: A, B, C matrices; s = witness vector
- **Purpose**: Flatten computation into quadratic constraints

### QAP (Quadratic Arithmetic Program)
- **Format**: A(x) * B(x) = C(x) + H(x) * Z(x)
- **Components**: Polynomials for each variable
- **Purpose**: Convert constraints to polynomial form

### zkSNARK Protocol
1. **Setup**: Generate proving & verification keys
2. **Prove**: Create proof from witness
3. **Verify**: Check proof with public inputs only

## 🔑 Key Properties

| Property | Meaning |
|----------|---------|
| **Completeness** | Valid witness → proof accepts |
| **Soundness** | Invalid witness → proof rejects |
| **Zero-Knowledge** | Verifier learns nothing about secrets |
| **Succinctness** | Proof is small (O(1) size) |
| **Non-Interactive** | One-shot proof, no back-and-forth |

## 📐 Mathematical Operations

### Field Operations
```cpp
FieldElement a(5), b(3);
FieldElement sum = a + b;           // Addition
FieldElement diff = a - b;          // Subtraction
FieldElement prod = a * b;          // Multiplication
FieldElement quot = a / b;          // Division (via inverse)
FieldElement inv = a.inverse();     // Modular inverse
FieldElement pow = a.power(10);     // Exponentiation
```

### Elliptic Curve Operations
```cpp
ECPoint P(x1, y1), Q(x2, y2);
ECPoint R = P + Q;                  // Point addition
ECPoint S = P * 5;                  // Scalar multiplication
bool inf = P.isInfinity();          // Check if point at infinity
```

### R1CS Operations
```cpp
R1CS r1cs(num_vars, num_constraints);
r1cs.setConstraint(idx, a, b, c);   // Set constraint
bool valid = r1cs.verify(witness);  // Verify witness
r1cs.print();                       // Print constraint system
```

### QAP Operations
```cpp
QAP qap = QAP::fromR1CS(r1cs);      // Convert R1CS to QAP
Polynomial A, B, C;
qap.computePolynomials(witness, A, B, C);  // Compute polys
FieldElement val = poly.evaluate(x);       // Evaluate polynomial
```

### zkSNARK Operations
```cpp
ProvingKey pk;
VerificationKey vk;
zkSNARK::setup(qap, r1cs, pk, vk, num_public);  // Setup
Proof proof = zkSNARK::prove(qap, pk, witness, public_inputs);  // Prove
bool valid = zkSNARK::verify(vk, proof, public_inputs);  // Verify
```

## 🎯 Example Walkthroughs

### Simple Example: x² = 9

**Witness**: [1, 3, 9]  
**Constraint**: x * x = out

```
Variables:
  [0] one = 1
  [1] x = 3 (SECRET)
  [2] out = 9 (PUBLIC)

Constraint 0: x * x = out
  A = [0, 1, 0] → select x
  B = [0, 1, 0] → select x
  C = [0, 0, 1] → select out
  Verify: 3 * 3 = 9 ✓
```

### Full Example: x³ + x + 5 = 35

**Witness**: [1, 3, 35, 9, 27]  
**Constraints**: 3 constraints to compute x³

```
Variables:
  [0] one = 1
  [1] x = 3 (SECRET)
  [2] out = 35 (PUBLIC)
  [3] v₁ = 9 (intermediate: x²)
  [4] v₂ = 27 (intermediate: x³)

Constraint 0: x * x = v₁
  Verify: 3 * 3 = 9 ✓

Constraint 1: v₁ * x = v₂
  Verify: 9 * 3 = 27 ✓

Constraint 2: (v₂ + x + 5) * 1 = out
  Verify: (27 + 3 + 5) * 1 = 35 ✓
```

## 🔧 Common Tasks

### Create a Custom Computation

1. **Define variables**: List all values including intermediates
2. **Flatten operations**: Break into basic multiplications
3. **Create R1CS**: Express each operation as constraint
4. **Build witness**: Provide concrete values
5. **Convert to QAP**: Use `QAP::fromR1CS()`
6. **Setup keys**: Call `zkSNARK::setup()`
7. **Generate proof**: Call `zkSNARK::prove()`
8. **Verify proof**: Call `zkSNARK::verify()`

### Debug a Failing Constraint

```cpp
// Print R1CS to see constraint details
r1cs.print();

// Manually verify each constraint
for (int i = 0; i < num_constraints; i++) {
    FieldElement a_val(0), b_val(0), c_val(0);
    // Compute A·witness, B·witness, C·witness
    // Check if (A·witness) * (B·witness) = C·witness
}

// Check witness values
cout << "Witness: ";
for (auto& w : witness) cout << w << " ";
```

## 📊 Performance Characteristics

| Component | Complexity | Notes |
|-----------|-----------|-------|
| R1CS Creation | O(n) | n = number of constraints |
| QAP Conversion | O(n²) | Lagrange interpolation |
| Setup | O(n) | Generate keys |
| Prove | O(n) | Compute proof |
| Verify | **O(1)** | Constant time! |
| Proof Size | **O(1)** | 3 EC points |

## ⚠️ Common Pitfalls

### 1. Incorrect Witness Size
```cpp
// ❌ Wrong: Witness size doesn't match R1CS variables
witness = {1, 3, 35};  // Only 3 values
R1CS r1cs(5, 3);       // Expects 5 variables

// ✓ Correct: Match witness size to number of variables
witness = {1, 3, 35, 9, 27};  // 5 values
R1CS r1cs(5, 3);              // 5 variables
```

### 2. Constraint Indices
```cpp
// ❌ Wrong: Vector indices out of order
A = {1, 0, 0};  // 3 elements
B = {0, 1, 0, 0, 0};  // 5 elements - MISMATCH!

// ✓ Correct: All vectors same size as number of variables
A = {0, 1, 0, 0, 0};  // 5 elements
B = {0, 1, 0, 0, 0};  // 5 elements
C = {0, 0, 1, 0, 0};  // 5 elements
```

### 3. Public vs Private Inputs
```cpp
// Prover has ALL values (public + private)
witness = {1, 3, 35, 9, 27};  // Including secret x=3

// Verifier has ONLY public values
public_inputs = {35};  // Just the output
```

## 🎓 Learning Resources

### Start Here
1. Run `simple.exe` - understand basic flow
2. Read `TECHNICAL_DOCUMENTATION.md` - learn theory
3. Run `zksnark.exe` - see detailed example
4. Study source code - understand implementation

### Deep Dive
- **Vitalik's Blog**: Excellent zkSNARK tutorials
- **"Why and How zk-SNARK Works"**: By Maksym Petkus
- **Academic Papers**: Pinocchio, Groth16, PLONK

### Experiment
- Modify witness values
- Add new constraints
- Create different computations
- Add timing measurements

## 🔍 Debugging Commands

```cpp
// Print field element
cout << "Value: " << field_elem << endl;

// Print EC point
cout << "Point: " << ec_point << endl;

// Print R1CS
r1cs.print();

// Verify R1CS
bool valid = r1cs.verify(witness);
cout << "R1CS Valid: " << (valid ? "YES" : "NO") << endl;

// Print polynomial
poly.print();

// Evaluate polynomial
FieldElement val = poly.evaluate(FieldElement(5));
cout << "P(5) = " << val << endl;
```

## 💡 Tips & Tricks

1. **Always verify R1CS first** before converting to QAP
2. **Print intermediate values** for debugging
3. **Start with simple examples** (x² = 9) before complex ones
4. **Use consistent variable ordering** in witness and constraints
5. **Remember**: Variable 0 should always be constant 1

## 📖 Glossary

- **Witness**: Complete set of values (public + private)
- **Constraint**: Quadratic equation over witness
- **R1CS**: System of quadratic constraints
- **QAP**: Polynomial representation of R1CS
- **Toxic Waste**: Random values from setup (must destroy!)
- **Proving Key**: Keys for proof generation
- **Verification Key**: Keys for proof checking
- **Proof**: Three elliptic curve points
- **Pairing**: Special function for verification

## 🎯 Next Steps

1. ✅ Understand the examples
2. ✅ Read the documentation
3. ✅ Modify the code
4. ✅ Create your own computation
5. ✅ Study production libraries (libsnark, bellman)
6. ✅ Read academic papers
7. ✅ Build something cool! 🚀

---

**Remember**: This is educational code. For production, use proper libraries! 🔒
