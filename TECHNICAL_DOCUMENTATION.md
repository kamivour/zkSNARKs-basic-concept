# zkSNARKs Technical Documentation

## Architecture Overview

This implementation demonstrates a complete zkSNARK protocol from the ground up:

### 1. Field Arithmetic (`field.h`)
- Implements finite field arithmetic over prime p = 2^31 - 1 (Mersenne prime)
- Operations: addition, subtraction, multiplication, division (via modular inverse)
- Extended Euclidean Algorithm for computing modular inverses
- Exponentiation using fast modular exponentiation

### 2. Elliptic Curve (`elliptic_curve.h`)
- Weierstrass form: y² = x³ + ax + b
- Point addition and doubling
- Scalar multiplication using double-and-add algorithm
- Simplified curve (not cryptographically secure, for education only)

### 3. R1CS - Rank-1 Constraint System (`r1cs.h`)
- Represents computation as constraints: (A·s) * (B·s) = (C·s)
- Each constraint is a quadratic equation over field elements
- Witness vector s contains all variables (including intermediates)
- Verification checks all constraints are satisfied

### 4. QAP - Quadratic Arithmetic Program (`qap.h`)
- Converts R1CS to polynomials using Lagrange interpolation
- Each variable becomes 3 polynomials (A, B, C)
- Target polynomial Z(x) vanishes at all constraint evaluation points
- Key property: If R1CS is satisfied, then A(x)·B(x) - C(x) is divisible by Z(x)

### 5. zkSNARK Protocol (`zksnark.h`)

#### Setup Phase
- Generates toxic waste: random scalars τ, α, β, γ, δ
- Creates proving key: evaluations of polynomials at τ encrypted as EC points
- Creates verification key: subset needed for verification
- **CRITICAL**: Toxic waste must be destroyed after setup!

#### Prove Phase
- Prover has: witness (including secret values)
- Computes polynomial evaluations using witness
- Adds random blinding factors for zero-knowledge property
- Creates proof: three elliptic curve points (A, B, C)

#### Verify Phase
- Verifier has: public inputs only (not secret inputs)
- Checks proof using pairing equations (simplified in this implementation)
- Verifier learns NOTHING about secret inputs
- Verification is fast (constant time, independent of computation size)

## Example: x³ + x + 5 = 35

### Problem
Prove you know x such that x³ + x + 5 = 35, without revealing x.

### Solution
x = 3: (27 + 3 + 5 = 35) ✓

### Flattening to R1CS

The computation requires intermediate variables:

```
Variables: [one, x, out, v1, v2]
           [  1, 3,  35,  9, 27]

Constraint 0: x * x = v1
  (3) * (3) = 9 ✓

Constraint 1: v1 * x = v2
  (9) * (3) = 27 ✓

Constraint 2: (v2 + x + 5) * 1 = out
  (27 + 3 + 5) * 1 = 35 ✓
```

### R1CS Matrix Representation

```
Constraint 0: x * x = v1
A = [0, 1, 0, 0, 0]  → selects x
B = [0, 1, 0, 0, 0]  → selects x
C = [0, 0, 0, 1, 0]  → selects v1

Constraint 1: v1 * x = v2
A = [0, 0, 0, 1, 0]  → selects v1
B = [0, 1, 0, 0, 0]  → selects x
C = [0, 0, 0, 0, 1]  → selects v2

Constraint 2: (5·one + 1·x + 1·v2) * 1 = out
A = [5, 1, 0, 0, 1]  → computes 5 + x + v2
B = [1, 0, 0, 0, 0]  → selects 1
C = [0, 0, 1, 0, 0]  → selects out
```

### QAP Conversion

Each variable gets 3 polynomials that pass through the constraint values:

```
For variable x:
- A-values at constraints: [0, 0, 1] → interpolate to polynomial A₁(x)
- B-values at constraints: [1, 1, 0] → interpolate to polynomial B₁(x)
- C-values at constraints: [0, 0, 0] → interpolate to polynomial C₁(x)

Evaluation points: x = 1, 2, 3 (one per constraint)
```

The key property: 
```
A(τ) · B(τ) - C(τ) = H(τ) · Z(τ)
```

Where:
- A(x) = Σᵢ wᵢ·Aᵢ(x)
- B(x) = Σᵢ wᵢ·Bᵢ(x)
- C(x) = Σᵢ wᵢ·Cᵢ(x)
- Z(x) = (x-1)(x-2)(x-3)
- wᵢ = witness values

### Security Properties

1. **Completeness**: If prover knows valid x, verification succeeds
2. **Soundness**: If prover doesn't know valid x, verification fails (with high probability)
3. **Zero-Knowledge**: Verifier learns nothing about x except that the statement is true
4. **Succinctness**: Proof is small (3 EC points) and verification is fast

## Limitations of This Implementation

This is educational code with intentional simplifications:

1. **Small Field**: Uses 31-bit prime instead of 254-bit BN curve
2. **No Pairings**: Real zkSNARKs use bilinear pairings (e(A,B) = e(C,D))
3. **No CRS**: Missing Common Reference String ceremony
4. **Weak Curve**: Elliptic curve is not cryptographically secure
5. **Limited ZK**: Blinding factors are simplified
6. **No Optimizations**: Real systems use FFT for polynomial operations

## For Production Use, You Need:

1. **Proper Curves**: BN254, BLS12-381 with pairing support
2. **Libraries**: libsnark, bellman, arkworks
3. **Trusted Setup**: Multi-party computation (MPC) ceremony
4. **Optimizations**: FFT, multi-exponentiation, batch verification
5. **Security Audit**: Professional cryptographic review

## Mathematical Foundations

### Schwartz-Zippel Lemma
If polynomial P(x) has degree d and P(r) = 0 for random r in large field,
then P is zero polynomial with high probability (1 - d/|F|).

This is why QAP works: if R1CS is satisfied, then A·B - C is divisible by Z,
so checking at random τ is sufficient.

### Pairing-Based Cryptography
Real zkSNARKs use bilinear pairings:
```
e: G₁ × G₂ → Gₜ
e(aP, bQ) = e(P, Q)^(ab)
```

This allows verification without revealing τ or witness values.

### Knowledge of Exponent Assumption (KEA)
If you can compute (C, C^τ) you must "know" the exponent.
This prevents cheating in proof construction.

## References

1. **Pinocchio Protocol**: Parno, Howell, Gentry, Raykova (2013)
2. **Groth16**: Most efficient zkSNARK (2016)
3. **PLONK**: Universal setup variant (2019)
4. **STARKs**: Transparent alternative (no trusted setup)

## Compilation and Execution

```powershell
# Compile
g++ -std=c++17 main.cpp -o zksnark.exe

# Run
.\zksnark.exe
```

Expected output:
- R1CS constraint verification
- QAP polynomial creation
- Key generation details
- Proof creation with witness
- Verification success

The verifier confirms the prover knows x such that x³ + x + 5 = 35,
without learning x = 3!
