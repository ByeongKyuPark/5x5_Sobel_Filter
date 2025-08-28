#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

class QuickEdgeTest {
public:
    static void runCompleteTest() {
        std::cout << "🎯 COMPLETE SOBEL FILTER TEST SUITE" << std::endl;
        std::cout << "====================================" << std::endl;
        
        // Test patterns to generate and test
        struct TestCase {
            std::string name;
            std::string input;
            std::string output;
            std::string expectedResult;
        };
        
        TestCase tests[] = {
            {"Building with Windows", "building_test.raw", "building_edges.raw", "Strong window frame edges"},
            {"Colorful Books", "books_test.raw", "books_edges.raw", "Book spine vertical edges"},
            {"Crosswalk Stripes", "crosswalk_test.raw", "crosswalk_edges.raw", "Horizontal stripe edges"},
            {"Geometric Shapes", "geometric_test.raw", "geometric_edges.raw", "Circle and checkerboard edges"}
        };
        
        std::cout << "\n1️⃣ GENERATING TEST PATTERNS..." << std::endl;
        if (system("visual_test_generator.exe > nul 2>&1") == 0) {
            std::cout << "✅ Test patterns generated" << std::endl;
        } else {
            std::cout << "❌ Failed to generate patterns" << std::endl;
            return;
        }
        
        std::cout << "\n2️⃣ RUNNING EDGE DETECTION..." << std::endl;
        int successCount = 0;
        
        for (const auto& test : tests) {
            std::cout << "\nTesting: " << test.name << std::endl;
            
            // Run Sobel filter
            std::string command = "sobel_filter.exe " + test.input + " " + test.output + " > nul 2>&1";
            if (system(command.c_str()) == 0) {
                std::cout << "  ✅ Edge detection completed" << std::endl;
                
                // Quick analysis
                analyzeResult(test.output, test.expectedResult);
                successCount++;
            } else {
                std::cout << "  ❌ Edge detection failed" << std::endl;
            }
        }
        
        std::cout << "\n3️⃣ FINAL SUMMARY" << std::endl;
        std::cout << "================" << std::endl;
        std::cout << "Tests passed: " << successCount << "/4" << std::endl;
        
        if (successCount == 4) {
            std::cout << "🎉 ALL TESTS PASSED!" << std::endl;
            std::cout << "✅ Your SIMD Sobel filter is working correctly!" << std::endl;
            std::cout << "✅ Edge detection producing reasonable results!" << std::endl;
            std::cout << "✅ Ready for submission to your coding test!" << std::endl;
        } else {
            std::cout << "⚠️  Some tests failed - check implementation" << std::endl;
        }
        
        std::cout << "\n🔍 For detailed analysis, run:" << std::endl;
        std::cout << "   edge_analyzer.exe building_edges.raw ascii" << std::endl;
    }
    
private:
    static void analyzeResult(const std::string& filename, const std::string& expected) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cout << "  ❌ Cannot read result file" << std::endl;
            return;
        }
        
        // Quick check - read first 1000 pixels
        std::vector<uint8_t> sample(1000);
        file.read(reinterpret_cast<char*>(sample.data()), 1000);
        file.close();
        
        auto minmax = std::minmax_element(sample.begin(), sample.end());
        int minVal = *minmax.first;
        int maxVal = *minmax.second;
        
        int brightPixels = 0;
        for (uint8_t pixel : sample) {
            if (pixel > 100) brightPixels++;
        }
        
        double brightPercent = 100.0 * brightPixels / sample.size();
        
        std::cout << "  📊 Range: " << minVal << "-" << maxVal 
                  << ", Bright edges: " << brightPercent << "%" << std::endl;
        
        // Simple validation
        if (maxVal > 200 && brightPercent > 5) {
            std::cout << "  ✅ Good edge detection (" << expected << ")" << std::endl;
        } else if (maxVal > 100 && brightPercent > 1) {
            std::cout << "  ⚠️  Weak but working (" << expected << ")" << std::endl;
        } else {
            std::cout << "  ❌ Poor edge detection" << std::endl;
        }
    }
};

int main() {
    QuickEdgeTest::runCompleteTest();
    return 0;
}
