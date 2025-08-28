# Sobel Filter - 5x5 Edge Detection

## 프로젝트 개요

640x640 RGB 이미지에 대한 5x5 Sobel 엣지 검출 필터 구현. C++17 표준을 따르며 유지보수성, 성능, 확장성을 고려한 설계.

## Phase 1: Infra

### 구현된 기능
- 이미지 데이터 구조: RGB 및 그레이스케일 픽셀 지원하는 범용 이미지 템플릿 클래스
- 파일 입출력 연산: 포괄적인 오류 처리를 포함한 raw binary 파일 읽기/쓰기
- RGB to 그레이스케일 변환: ITU-R BT.709 표준 가중치 적용
- 메모리 관리: RAII 준수 리소스 관리
- 오류 처리: C++17 호환성을 위한 사용자 정의 Result 타입

### 아키텍처 설계

#### 핵심 클래스
1. **`Image<PixelType>`**: 이미지 데이터를 위한 범용 템플릿 클래스
   - 다양한 픽셀 타입 지원 (RGB, 그레이스케일)
   - `getPixelSafe()`를 통한 안전한 경계 검사
   - 예외 안전 연산

2. **`ImageIO`**: 파일 연산을 위한 정적 유틸리티 클래스
   - 처리 전 파일 크기 검증
   - 오류 처리를 위한 사용자 정의 Result 타입
   - raw binary 형식 지원

3. **`RGBPixel`**: 변환 유틸리티를 포함한 RGB 픽셀 구조체
   - 채널당 8비트 저장
   - 표준 준수 그레이스케일 변환

### 빌드 환경

- **C++ 표준**: C++17
- **컴파일러**: MSVC 19.41+ / GCC 7+ / Clang 5+
- **빌드 시스템**: CMake 3.12+
- **플랫폼**: Windows(Primary), Linux/macOS 호환

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

**입력 요구사항:**
- 파일 형식: Raw Binary RGB
- 크기: 640x640 픽셀
- 사이즈: 1,228,800 바이트 (640 × 640 × 3)

**출력:**
- 파일 형식: Raw Binary 그레이스케일
- 사이즈: 409,600 바이트 (640 × 640 × 1)

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

## Technical 가정

1. **이미지 형식**: Raw 바이너리 RGB 파일 (헤더 없음)
2. **픽셀 순서**: 행 우선 순서 (y × width + x)
3. **색상 공간**: ITU-R BT.709 가중치를 사용한 sRGB
4. **경계 처리**: 컨볼루션을 위한 제로 패딩
5. **출력 범위**: 8비트 양자화 (0-255)

---
* BK Park  
* 2025년 8월 27일
* 1.0.0
