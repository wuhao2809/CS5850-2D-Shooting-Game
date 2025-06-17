# Get the directory where the script is located
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# Check if we're already in the CPP directory
if ((Get-Location).Path -notlike "*\CPP") {
    Set-Location $scriptDir
}

# Create build and bin directories if they don't exist
New-Item -ItemType Directory -Force -Path "build" | Out-Null
New-Item -ItemType Directory -Force -Path "bin" | Out-Null

# Build the project using CMake
Set-Location build
Write-Host "Running CMake..."
cmake -G "MinGW Makefiles" ..
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake failed with exit code $LASTEXITCODE"
    exit 1
}

Write-Host "Running make..."
mingw32-make
if ($LASTEXITCODE -ne 0) {
    Write-Host "Make failed with exit code $LASTEXITCODE"
    exit 1
}

Set-Location ..

# Run the game
Write-Host "Running game..."
.\bin\GameEngine.exe
if ($LASTEXITCODE -ne 0) {
    Write-Host "Game failed to run with exit code $LASTEXITCODE"
    exit 1
} 