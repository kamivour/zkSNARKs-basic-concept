# zkSNARKs Implementation from Scratch

A simplified educational implementation of zkSNARKs (Zero-Knowledge Succinct Non-Interactive Arguments of Knowledge) in C++ without external libraries.

## 📁 Project Structure

```
zkSNARKs_simplified/
│
├── src/                    # Core implementation (header files)
│   ├── field.h            # Finite field arithmetic
│   ├── elliptic_curve.h   # Elliptic curve operations
│   ├── r1cs.h             # Rank-1 Constraint System
│   ├── qap.h              # Quadratic Arithmetic Program
│   └── zksnark.h          # Main zkSNARK protocol
│
├── examples/              # Example programs
│   ├── main.cpp           # Full demo: x³ + x + 5 = 35
│   └── simple_example.cpp # Simple demo: x² = 9
│
├── docs/                  # Documentation
│   ├── README.md          # Detailed project documentation
│   ├── INDEX.md           # Complete file index
│   ├── QUICK_REFERENCE.md # Quick reference guide
│   ├── TECHNICAL_DOCUMENTATION.md  # Theory and math
│   ├── PROJECT_SUMMARY.md # Project overview
│   ├── CODE_OVERVIEW.cpp  # Architecture guide
│   └── VISUAL_DIAGRAMS.txt # ASCII diagrams
│
├── build_and_run.ps1      # Build automation script
├── .gitignore             # Git ignore file
└── README.md              # This file
```

## 🚀 Quick Start

### Compile Examples

```powershell
# Compile main example
g++ -std=c++17 examples/main.cpp -o zksnark.exe

# Compile simple example
g++ -std=c++17 examples/simple_example.cpp -o simple.exe
```

### Run Examples

```powershell
.\simple.exe       # Simple example: x² = 9
.\zksnark.exe      # Full example: x³ + x + 5 = 35
```

### Or Use Build Script

```powershell
.\build_and_run.ps1
```

## 📚 What This Project Contains

This project implements the core components of zkSNARKs:

1. **Finite Field Arithmetic** (`src/field.h`) - Operations over a prime field
2. **Elliptic Curve** (`src/elliptic_curve.h`) - Points and operations on elliptic curves
3. **R1CS** (`src/r1cs.h`) - Rank-1 Constraint System representation
4. **QAP** (`src/qap.h`) - Quadratic Arithmetic Program with Lagrange interpolation
5. **zkSNARK Protocol** (`src/zksnark.h`) - Setup, Prove, and Verify phases

## 🎯 Examples

### Example 1: x² = 9 (`examples/simple_example.cpp`)
**Problem**: Prove you know x such that x² = 9, without revealing x.

**Solution**: x = 3

The proof convinces a verifier that the prover knows `x` WITHOUT revealing what `x` is!

### Example 2: x³ + x + 5 = 35 (`examples/main.cpp`)
**Problem**: Prove you know x such that x³ + x + 5 = 35, without revealing x.

**Solution**: x = 3 (27 + 3 + 5 = 35)

This example shows the full zkSNARK pipeline with detailed logging at each step.

## 📖 Documentation

For detailed information, see the `docs/` folder:

- **[docs/README.md](docs/README.md)** - Comprehensive project documentation
- **[docs/INDEX.md](docs/INDEX.md)** - Complete navigation guide
- **[docs/QUICK_REFERENCE.md](docs/QUICK_REFERENCE.md)** - Commands and API reference
- **[docs/TECHNICAL_DOCUMENTATION.md](docs/TECHNICAL_DOCUMENTATION.md)** - Mathematical theory
- **[docs/PROJECT_SUMMARY.md](docs/PROJECT_SUMMARY.md)** - Project overview
- **[docs/VISUAL_DIAGRAMS.txt](docs/VISUAL_DIAGRAMS.txt)** - ASCII art diagrams

## 🎓 Learning Path

1. Read this README
2. Run `.\simple.exe` to see a basic example
3. Read `docs/QUICK_REFERENCE.md` for commands
4. Read `docs/TECHNICAL_DOCUMENTATION.md` for theory
5. Run `.\zksnark.exe` for detailed output
6. Study the source code in `src/`

## ⚠️ Educational Purpose Only

This is **NOT cryptographically secure** and should **NEVER be used in production**!

### Simplifications:
- Uses a small prime field (2³¹-1) instead of 254-bit curves
- Simplified elliptic curve (not cryptographically strong)
- No real bilinear pairings
- Simplified trusted setup
- No optimizations (FFT, batch verification, etc.)

### For Production:
Use established libraries like:
- **libsnark** (C++)
- **bellman** (Rust)
- **arkworks** (Rust)
- **circom + snarkjs** (JavaScript)

## 🔑 Key Concepts

- **Zero-Knowledge**: Verifier learns nothing except the statement is true
- **Succinctness**: Proof is small (O(1) size)
- **Non-Interactive**: One-shot proof, no back-and-forth
- **Soundness**: Invalid witness → proof fails
- **Completeness**: Valid witness → proof succeeds

## 📚 References

- **Pinocchio Protocol**: Parno et al. (2013)
- **Groth16**: Groth (2016) - Most efficient zkSNARK
- **PLONK**: Gabizon et al. (2019) - Universal setup
- **Vitalik's zkSNARK Tutorial Series**
- **"Why and How zk-SNARK Works"** by Maksym Petkus

## 🙏 Acknowledgments

Based on groundbreaking research by:
- Oded Goldreich, Silvio Micali, Charles Rackoff (GMR)
- Craig Gentry, Jens Groth, and many others
- The entire cryptography community

## 📜 License

Educational use only.

---

**Happy Learning! 🚀** For questions, start with the documentation in the `docs/` folder!
