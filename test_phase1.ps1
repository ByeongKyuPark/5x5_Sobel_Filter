#!/usr/bin/env powershell
# Phase 1 Test Suite - Simple Validation
# Author: BK Park

Write-Host "=========================================="
Write-Host "     Sobel Filter Phase 1 Test Suite"
Write-Host "=========================================="

$tests = 0
$passed = 0

Write-Host "`n[Test 1] Build System Verification"
if (Test-Path "build\Release\sobel_filter.exe") {
    Write-Host "PASS: Main executable exists" -ForegroundColor Green
    $passed++
} else {
    Write-Host "FAIL: Main executable missing" -ForegroundColor Red
}
$tests++

if (Test-Path "build\Release\test_data_generator.exe") {
    Write-Host "PASS: Test data generator exists" -ForegroundColor Green
    $passed++
} else {
    Write-Host "FAIL: Test data generator missing" -ForegroundColor Red
}
$tests++

Write-Host "`n[Test 2] Help Message Test"
Set-Location build
$helpResult = & ".\Release\sobel_filter.exe" 2>&1
if ($helpResult -match "Usage:" -and $LASTEXITCODE -eq 1) {
    Write-Host "PASS: Help message works correctly" -ForegroundColor Green
    $passed++
} else {
    Write-Host "FAIL: Help message not working" -ForegroundColor Red
}
$tests++

Write-Host "`n[Test 3] Test Data Generation"
$genResult = & ".\Release\test_data_generator.exe" 2>&1
if (Test-Path "test_input_640x640.raw") {
    $size = (Get-Item "test_input_640x640.raw").Length
    if ($size -eq 1228800) {
        Write-Host "PASS: Test data generated with correct size ($size bytes)" -ForegroundColor Green
        $passed++
    } else {
        Write-Host "FAIL: Test data has wrong size ($size bytes)" -ForegroundColor Red
    }
} else {
    Write-Host "FAIL: Test data not generated" -ForegroundColor Red
}
$tests++

Write-Host "`n[Test 4] Image Processing"
$processResult = & ".\Release\sobel_filter.exe" "test_input_640x640.raw" "test_output.raw" 2>&1
if ($LASTEXITCODE -eq 0 -and (Test-Path "test_output.raw")) {
    $outSize = (Get-Item "test_output.raw").Length
    if ($outSize -eq 409600) {
        Write-Host "PASS: Image processing with correct output size ($outSize bytes)" -ForegroundColor Green
        $passed++
    } else {
        Write-Host "FAIL: Wrong output size ($outSize bytes)" -ForegroundColor Red
    }
} else {
    Write-Host "FAIL: Image processing failed" -ForegroundColor Red
}
$tests++

Write-Host "`n[Test 5] Error Handling - File Not Found"
$errorResult = & ".\Release\sobel_filter.exe" "nonexistent.raw" "output.raw" 2>&1
if ($LASTEXITCODE -eq 1 -and ($errorResult -match "File not found")) {
    Write-Host "PASS: File not found error handled correctly" -ForegroundColor Green
    $passed++
} else {
    Write-Host "FAIL: File not found error not handled" -ForegroundColor Red
}
$tests++

Write-Host "`n=========================================="
Write-Host "Results: $passed/$tests tests passed"

if ($passed -eq $tests) {
    Write-Host "ALL TESTS PASSED! Phase 1 ready for commit." -ForegroundColor Green
    Write-Host "`nImplemented Features:"
    Write-Host "- Core image data structures"
    Write-Host "- File I/O with error handling"
    Write-Host "- RGB to grayscale conversion"
    Write-Host "- Memory management"
    Write-Host "- Build system"
    Write-Host "`nNext: Phase 2 - Sobel filter implementation"
    Set-Location ..
    exit 0
} else {
    Write-Host "Some tests failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}
