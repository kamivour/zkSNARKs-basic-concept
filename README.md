# zkSNARKs Implementation from Scratch

A simplified educational implementation of zkSNARKs (Zero-Knowledge Succinct Non-Interactive Argument of Knowledge) in C++ without external libraries.

## Overview

This project implements the core components of zkSNARKs:

1. **Finite Field Arithmetic** (`field.h`) - Operations over a prime field
2. **Elliptic Curve** (`elliptic_curve.h`) - Points and operations on elliptic curves
3. **R1CS** (`r1cs.h`) - Rank-1 Constraint System representation
4. **QAP** (`qap.h`) - Quadratic Arithmetic Program with Lagrange interpolation
5. **zkSNARK Protocol** (`zksnark.h`) - Setup, Prove, and Verify phases

## Example Problem

The demo proves knowledge of `x` such that:
```
x³ + x + 5 = 35
```

Solution: `x = 3` (27 + 3 + 5 = 35)

The proof convinces a verifier that the prover knows `x` WITHOUT revealing what `x` is!

## R1CS Constraints

The computation is flattened into 3 constraints:

```
Variables: [1, x, out, v1, v2]
Constraint 0: x * x = v1        (v1 = x²)
Constraint 1: v1 * x = v2       (v2 = x³)
Constraint 2: (v2 + x + 5) * 1 = out
```

## Building and Running

### Windows (MSVC)
```powershell
cl /EHsc /std:c++17 main.cpp /Fe:zksnark.exe
.\zksnark.exe
```

### Windows (MinGW)
```powershell
g++ -std=c++17 main.cpp -o zksnark.exe
.\zksnark.exe
```

### Linux/Mac
```bash
g++ -std=c++17 main.cpp -o zksnark
./zksnark
```

## Program Flow

1. **R1CS Creation**: Flatten the computation into constraints
2. **R1CS Verification**: Check witness satisfies constraints
3. **QAP Conversion**: Convert constraints to polynomials using Lagrange interpolation
4. **Setup**: Generate proving and verification keys (with toxic waste)
5. **Prove**: Prover creates a proof using the witness and proving key
6. **Verify**: Verifier checks the proof using only public inputs and verification key

## Output

The program provides detailed logs for each phase:

- R1CS constraint values
- QAP polynomial creation
- Key generation parameters
- Proof generation steps
- Verification checks

## Simplifications

This is an educational implementation with simplifications:

- Uses a small prime field (2³¹ - 1) instead of BN254 curve
- Simplified elliptic curve pairing (not full bilinear pairing)
- No CRS (Common Reference String) ceremony
- No zero-knowledge blinding factors in full form
- Simplified polynomial operations

## Security Warning

⚠️ **DO NOT USE IN PRODUCTION** ⚠️

This is for educational purposes only. A production zkSNARK system requires:

- Cryptographically secure elliptic curves (BN254, BLS12-381)
- Proper pairing operations
- Secure multi-party computation for setup
- Proper random number generation
- Rigorous security audits

## References

- [Why and How zk-SNARK Works](https://arxiv.org/abs/1906.07221)
- [Vitalik's zkSNARK Tutorial](https://medium.com/@VitalikButerin/quadratic-arithmetic-programs-from-zero-to-hero-f6d558cea649)
- [Pinocchio Protocol](https://eprint.iacr.org/2013/279.pdf)

## License

Educational use only.
