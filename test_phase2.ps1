#!/usr/bin/env powershell
# Phase 2 Sobel Filter Comprehensive Testing
# Author: BK Park

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "    PHASE 2 SOBEL FILTER TESTING" -ForegroundColor Cyan  
Write-Host "========================================" -ForegroundColor Cyan

$ErrorActionPreference = "Stop"

try {
    Set-Location build
    
    Write-Host "`n[1/6] Building project..." -ForegroundColor Yellow
    cmake --build . --config Release
    if ($LASTEXITCODE -ne 0) { throw "Build failed" }

    Write-Host "`n[2/6] Generating test data..." -ForegroundColor Yellow
    .\Release\test_data_generator.exe
    if (-not (Test-Path "test_input_640x640.raw")) { throw "Test data generation failed" }

    Write-Host "`n[3/6] Running Sobel filter..." -ForegroundColor Yellow
    .\Release\sobel_filter.exe test_input_640x640.raw sobel_phase2_output.raw
    if ($LASTEXITCODE -ne 0) { throw "Sobel filter execution failed" }

    Write-Host "`n[4/6] File integrity validation..." -ForegroundColor Yellow
    $inputSize = (Get-Item "test_input_640x640.raw").Length
    $outputSize = (Get-Item "sobel_phase2_output.raw").Length

    Write-Host "Input size: $inputSize bytes (RGB)"
    Write-Host "Output size: $outputSize bytes (Grayscale)"

    if ($inputSize -eq 1228800 -and $outputSize -eq 409600) {
        Write-Host "PASS: File sizes correct" -ForegroundColor Green
    } else {
        Write-Host "FAIL: File sizes incorrect" -ForegroundColor Red
    }

    Write-Host "`n[5/6] Edge detection analysis..." -ForegroundColor Yellow
    $outputBytes = [System.IO.File]::ReadAllBytes("S:\project\5x5_Sobel_Filter\build\sobel_phase2_output.raw")
    $stats = @{
        Min = ($outputBytes | Measure-Object -Minimum).Minimum
        Max = ($outputBytes | Measure-Object -Maximum).Maximum
        Mean = ($outputBytes | Measure-Object -Average).Average
    }

    Write-Host "Min intensity: $($stats.Min)"
    Write-Host "Max intensity: $($stats.Max)"
    Write-Host "Mean intensity: $([math]::Round($stats.Mean, 2))"
    Write-Host "Dynamic range: $($stats.Max - $stats.Min)"

    $edgePixels = ($outputBytes | Where-Object { $_ -gt 50 }).Count
    $edgePercentage = [math]::Round(($edgePixels / $outputBytes.Count) * 100, 2)

    Write-Host "Edge pixels (>50): $edgePixels"
    Write-Host "Edge percentage: $edgePercentage%"

    Write-Host "`n[6/6] Error handling test..." -ForegroundColor Yellow
    & .\Release\sobel_filter.exe nonexistent.raw test_error.raw 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Host "WARNING: Error handling may need improvement" -ForegroundColor Yellow
    } else {
        Write-Host "PASS: Error handling working correctly" -ForegroundColor Green
    }

    Write-Host "`n========================================"
    if ($stats.Max -gt $stats.Min -and $edgePixels -gt 1000) {
        Write-Host "SUCCESS: Sobel edge detection working!" -ForegroundColor Green
        Write-Host "- Dynamic range detected: $($stats.Max - $stats.Min)"
        Write-Host "- Edge pixels found: $edgePixels"
        Write-Host "- Phase 2 implementation verified"
    } else {
        Write-Host "WARNING: Edge detection may not be working properly" -ForegroundColor Yellow
    }
    Write-Host "========================================"

} catch {
    Write-Host "ERROR: $($_.Exception.Message)" -ForegroundColor Red
    Set-Location ..
    exit 1
} finally {
    Set-Location ..
}
