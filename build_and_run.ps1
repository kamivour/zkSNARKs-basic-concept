# zkSNARKs Build and Run Script
# PowerShell script for Windows

Write-Host "╔════════════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║   zkSNARKs from Scratch - Build Script        ║" -ForegroundColor Cyan
Write-Host "╚════════════════════════════════════════════════╝" -ForegroundColor Cyan
Write-Host ""

# Check if g++ is available
$gppExists = Get-Command g++ -ErrorAction SilentlyContinue
if (-not $gppExists) {
    Write-Host "❌ Error: g++ compiler not found!" -ForegroundColor Red
    Write-Host "Please install MinGW-w64 or MSVC" -ForegroundColor Yellow
    exit 1
}

Write-Host "✓ g++ compiler found" -ForegroundColor Green
Write-Host ""

# Build main example
Write-Host "[1/2] Building main example (x³ + x + 5 = 35)..." -ForegroundColor Cyan
g++ -std=c++17 main.cpp -o zksnark.exe 2>&1 | Out-Null
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ zksnark.exe compiled successfully" -ForegroundColor Green
} else {
    Write-Host "❌ Compilation failed for main.cpp" -ForegroundColor Red
    exit 1
}

# Build simple example
Write-Host "[2/2] Building simple example (x² = 9)..." -ForegroundColor Cyan
g++ -std=c++17 simple_example.cpp -o simple.exe 2>&1 | Out-Null
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ simple.exe compiled successfully" -ForegroundColor Green
} else {
    Write-Host "❌ Compilation failed for simple_example.cpp" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "╔════════════════════════════════════════════════╗" -ForegroundColor Green
Write-Host "║          Build Successful! 🎉                  ║" -ForegroundColor Green
Write-Host "╚════════════════════════════════════════════════╝" -ForegroundColor Green
Write-Host ""

# Ask user which example to run
Write-Host "Which example would you like to run?" -ForegroundColor Yellow
Write-Host "1. Simple example (x² = 9) - Quick demo" -ForegroundColor White
Write-Host "2. Full example (x³ + x + 5 = 35) - Detailed output" -ForegroundColor White
Write-Host "3. Both examples" -ForegroundColor White
Write-Host "4. Exit" -ForegroundColor White
Write-Host ""

$choice = Read-Host "Enter your choice (1-4)"

switch ($choice) {
    "1" {
        Write-Host ""
        Write-Host "Running simple example..." -ForegroundColor Cyan
        Write-Host ""
        .\simple.exe
    }
    "2" {
        Write-Host ""
        Write-Host "Running full example..." -ForegroundColor Cyan
        Write-Host ""
        .\zksnark.exe
    }
    "3" {
        Write-Host ""
        Write-Host "Running simple example first..." -ForegroundColor Cyan
        Write-Host ""
        .\simple.exe
        Write-Host ""
        Write-Host "Press any key to run full example..."
        $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
        Write-Host ""
        Write-Host "Running full example..." -ForegroundColor Cyan
        Write-Host ""
        .\zksnark.exe
    }
    "4" {
        Write-Host "Goodbye! 👋" -ForegroundColor Cyan
        exit 0
    }
    default {
        Write-Host "Invalid choice. Exiting." -ForegroundColor Red
        exit 1
    }
}

Write-Host ""
Write-Host "════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "Done! Thank you for exploring zkSNARKs! 🚀" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════" -ForegroundColor Cyan
