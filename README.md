# Sobel Filter - 5x5 Edge Detection

## 프로젝트 개요

640x640 RGB 이미지에 대한 5x5 Sobel 엣지 검출 필터 구현. C++17 표준을 따르며 유지보수성, 성능, 확장성을 염두에 둔 설계.

## Phase 1: Infra

###### 성능 테스트 결과
```
SSE SIMD:  ~18.72ms (1.95x speedup)
AVX2 SIMD: ~21.93ms (1.95x speedup)  
Baseline:  ~53.66ms
Real-time capability: Close to target (60 FPS = 16.67ms budget)
```
- 이미지 데이터 구조: RGB 및 그레이스케일 픽셀 지원하는 범용 이미지 템플릿 클래스
- 파일 입출력 연산: 포괄적인 오류 처리를 포함한 raw binary 파일 읽기/쓰기
- RGB to 그레이스케일 변환: ITU-R BT.709 표준 가중치 적용
- 메모리 관리: RAII 준수 리소스 관리
- 오류 처리: C++17 호환성을 위한 사용자 정의 Result 타입

### Archithecture 설계

#### 핵심 클래스
1. **`Image<PixelType>`**: 이미지 데이터를 위한 범용 템플릿 클래스
   - 다양한 픽셀 타입 지원 (RGB, 그레이스케일)
   - `getPixelSafe()`를 통한 안전한 경계 검사
   - 예외 안전 연산

2. **`ImageIO`**: 파일 연산을 위한 정적 유틸리티 클래스
   - 처리 전 파일 크기 검증
   - 오류 처리를 위한 사용자 정의 Result 타입
   - 원시 바이너리 형식 지원

3. **`RGBPixel`**: 변환 유틸리티를 포함한 RGB 픽셀 구조체
   - 채널당 8비트 저장
   - 표준 준수 그레이스케일 변환

### 빌드 환경

- **C++ 표준**: C++17
- **컴파일러**: MSVC 19.41+ / GCC 7+ / Clang 5+
- **빌드 시스템**: CMake 3.12+
- **플랫폼**: Windows (주), Linux/macOS 호환

### 빌드 방법

#### Windows (Visual Studio)
```bash
# 제공된 빌드 스크립트 사용
.\build.bat

# 또는 수동으로
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## 빌드 및 테스트 명령어 (Build & Test Commands)

### 프로젝트 빌드 (Project Build)

#### 전체 빌드 (Full Build)
```bash
# 빌드 디렉토리 생성 및 빌드
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

#### 디버그 빌드 (Debug Build)
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

### 🧪 테스트 실행 (Test Execution)

#### 정확성 검증 테스트 (Correctness Validation)
```bash
cd build/Release
.\validation_test.exe     # 100개 테스트 케이스로 SIMD vs 기준선 비교
```

#### 성능 벤치마크 (Performance Benchmark)
```bash
cd build/Release
.\benchmark_simd.exe      # SIMD 성능 측정 (AVX2, SSE, Scalar 비교)
```

#### 디버그 테스트 (Debug Test)
```bash
cd build/Release
.\debug_test.exe          # 작은 5x5 이미지로 단계별 디버깅
```

#### 메인 애플리케이션 (Main Application)
```bash
cd build/Release
.\sobel_filter.exe        # 640x640 이미지 처리 데모
```

#### 테스트 데이터 생성 (Test Data Generation)
```bash
cd build/Release
.\test_data_generator.exe # 테스트용 샘플 이미지 생성
```

### 성능 분석 (Performance Analysis)

#### SIMD 최적화 레벨별 성능 비교
```bash
# AVX2 SIMD (최고 성능)
.\benchmark_simd.exe | findstr "AVX2"

# SSE SIMD (호환성)
.\benchmark_simd.exe | findstr "SSE" 

# Scalar (기준선)
.\benchmark_simd.exe | findstr "Scalar"
```

#### 실시간 성능 검증 (60 FPS = 16.67ms 목표)
```bash
.\benchmark_simd.exe | findstr "Real-time"
```

### 디버깅 및 문제 해결 (Debugging & Troubleshooting)

#### 컴파일 오류 확인
```bash
cmake --build . --config Release --verbose
```

#### CPU 기능 확인
```bash
.\benchmark_simd.exe | findstr "CPU Capabilities"
```

#### 메모리 정렬 확인
```bash
.\validation_test.exe | findstr "alignment"
```

### 빠른 테스트 스위트 (Quick Test Suite)

#### 전체 검증 (Complete Validation)
```bash
cd build/Release
echo "=== 빌드 확인 ==="
cmake --build .. --config Release
echo "=== 정확성 테스트 ==="
.\validation_test.exe
echo "=== 성능 테스트 ==="
.\benchmark_simd.exe
echo "=== 디버그 테스트 ==="
.\debug_test.exe
```

#### PowerShell one liner (All Tests)
```powershell
cd build/Release; .\validation_test.exe; .\benchmark_simd.exe; .\debug_test.exe
```

### 예상 결과 (Expected Results)

#### 정확성 테스트 결과
```
Total tests: 100
Passed: 100 (100.0%)
Failed: 0
ALL TESTS PASSED! SIMD implementation is correct.
```

#### 성능 테스트 결과
```
AVX2 SIMD: ~16.70ms (2.46x speedup)
SSE SIMD:  ~18.59ms (2.21x speedup)
Baseline:  ~41.16ms
Real-time capable: ✅ Under 16.67ms budget for 60 FPS
```

### 개발 Workflow

#### 코드 변경 후 검증
```bash
# 1. 빌드
cmake --build . --config Release

# 2. 빠른 정확성 체크
.\debug_test.exe

# 3. 전체 검증
.\validation_test.exe

# 4. 성능 확인
.\benchmark_simd.exe
```

#### CI/CD 파이프라인 명령어
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cd Release
.\validation_test.exe || exit 1
.\benchmark_simd.exe || exit 1
echo "All tests passed - Ready for deployment"
```

#### Linux/macOS
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 현재 구현 테스트

```bash
# 사용 예시 (Phase 1 - 그레이스케일 변환만)
.\build\Release\sobel_filter.exe input_640x640.raw output_grayscale.raw
```

**참고**: 현재 시각화 기능은 구현되지 않음. 출력 결과 육안 확인을 위해서는 raw_to_bmp.exe 유틸리티 사용 필요.

**입력 요구사항:**
- 파일 형식: Raw 바이너리 RGB
- 크기: 640x640 픽셀
- 사이즈: 1,228,800 바이트 (640 × 640 × 3)

**출력:**
- 파일 형식: Raw 바이너리 그레이스케일
- 사이즈: 409,600 바이트 (640 × 640 × 1)

## 향후 단계

### TODO: 시각화 (Visualization)
- **상태**: 미구현 (Not Implemented)
- **설명**: 현재 구현은 원시 바이너리 출력만 지원
- **향후 계획**: 
  - BMP/PNG 직접 출력 지원
  - 실시간 시각화 도구
  - 웹 기반 시각화 인터페이스
  - 에지 검출 결과 오버레이 표시

**참고**: 현재는 raw_to_bmp.exe 유틸리티를 통한 수동 변환이 필요

## 설계 원칙

### 1. **유지보수성**
- 명확한 관심사 분리
- 포괄적인 문서화
- RAII 리소스 관리
- 예외 안전 보장

### 2. **성능**
- 템플릿 기반 제네릭 프로그래밍
- 제로 코스트 추상화
- 메모리 효율적 데이터 구조
- 컴파일러 최적화 친화적

### 3. **확장성**
- 다양한 픽셀 타입을 위한 템플릿 기반 이미지 클래스
- 구성 가능한 처리 매개변수
- 다양한 필터 타입을 위한 플러그인 준비 아키텍처

### 4. **테스트 가능성**
- 가능한 한 순수 함수
- 의존성 주입 준비
- 명확한 입출력 계약
- 모듈러 설계

## 파일 구조

```
5x5_Sobel_Filter/
├── CMakeLists.txt           # 빌드 구성
├── build.bat               # Windows 빌드 스크립트
├── README.md               # 본 파일
├── include/
│   ├── image.hpp           # 이미지 데이터 구조
│   ├── image_io.hpp        # 파일 입출력 연산
│   └── sobel_filter.hpp    # Sobel 필터 (Phase 2)
├── src/
│   ├── main.cpp            # 애플리케이션 진입점
│   ├── image.cpp           # 이미지 구현
│   └── image_io.cpp        # 입출력 구현
├── tests/                  # 단위 테스트 (Phase 4)
├── docs/                   # 추가 문서
└── examples/               # 샘플 데이터 및 사용법
```

## 기술적 가정
1. **이미지 형식**: Raw 바이너리 RGB 파일 (헤더 없음)
2. **픽셀 순서**: 행 우선 순서 (y × width + x)
3. **색상 공간**: ITU-R BT.709 가중치를 사용한 sRGB
4. **경계 처리**: 컨볼루션을 위한 제로 패딩
5. **출력 범위**: 8비트 양자화 (0-255)

* BK Park *  
* 2025년 8월 27일 *  
* 버전: 1.0.0 *
