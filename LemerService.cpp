#include "LemerService.h"
#include "LemerGenerator.h"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>

std::wstring LemerService::RunLab(uint64_t seed, int count)
{
    std::wstringstream output;
    std::wofstream file("results.txt");

    LemerGenerator generator(seed);
    auto sequence = generator.generateSequence(count);

    output << L"=== Послідовність (нормалізована) ===\n";
    file << L"=== Послідовність (нормалізована) ===\n";

    for (size_t i = 0; i < sequence.size(); ++i)
    {
        double normalized = (double)sequence[i] / 32767.0;

        output << std::fixed << std::setprecision(6) << normalized << L"  ";
        file << std::fixed << std::setprecision(6) << normalized << L"  ";

        if ((i + 1) % 5 == 0)
        {
            output << L"\n";
            file << L"\n";
        }
    }

    // Період
    LemerGenerator periodGen(seed);
    uint64_t start = seed;
    uint64_t x = periodGen.next();
    size_t period = 1;

    while (x != start && period <= 1000000) {
        x = periodGen.next();
        period++;
    }

    output << L"\n\nПеріод генератора: " << period << L"\n";
    file << L"\n\nПеріод генератора: " << period << L"\n";

    // Pi (Чезаро)
    LemerGenerator piGen(seed);
    size_t pairs = 10000;
    size_t coprime = 0;

    for (size_t i = 0; i < pairs; ++i) {
        uint64_t a = piGen.next();
        uint64_t b = piGen.next();

        while (b != 0) {
            uint64_t t = b;
            b = a % b;
            a = t;
        }
        if (a == 1) coprime++;
    }

    double piEstimate = sqrt(6.0 * pairs / coprime);

    output << L"\nОцінка π (Чезаро): "
        << std::fixed << std::setprecision(6)
        << piEstimate << L"\n";

    file << L"\nОцінка π (Чезаро): "
        << std::fixed << std::setprecision(6)
        << piEstimate << L"\n";

    file.close();
    return output.str();
}