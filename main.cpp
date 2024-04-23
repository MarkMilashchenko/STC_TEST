#include <iostream>
#include <chrono>
#include "SimpleXlsx/Xlsx/Chart.h"
#include "SimpleXlsx/Xlsx/Chartsheet.h"
#include "SimpleXlsx/Xlsx/Workbook.h"
#include "MyTests.h"
#include "DataGenerator.h"
#include "SFM.h"

int main(int argc, char* argv[])
{
    // StartTests(1000ULL);

    const int SECTION_SIZES[] = {4, 8, 16, 32, 64, 128};

    size_t DATASET_SIZE = 1000000ULL;
    if (argc > 1) {
        char* p = nullptr;
        const size_t inputSize = strtoull(argv[1], &p, 10);
        if (*p == 0) {
            DATASET_SIZE = inputSize;
        }
    }
    const auto inputF = GenerateData<float>(DATASET_SIZE);
    const auto inputD = GenerateData<double>(DATASET_SIZE);
    
    using namespace SimpleXlsx;
    std::vector<CellDataUInt> speedF; speedF.reserve(6);
    std::vector<CellDataUInt> speedD; speedD.reserve(6);

    using ClockPrefix = std::chrono::steady_clock;
    ClockPrefix::time_point startT, endT;
    std::chrono::duration<float> elapsed_seconds;
    startT = ClockPrefix::now();

    auto ProcessData = [&](const auto& inputData, int sectionSize, std::vector<CellDataUInt>& speedContainer) -> void {
        CalculateSFM(inputData, sectionSize);
        endT = ClockPrefix::now();
        elapsed_seconds = endT - startT;
        speedContainer.push_back((double)DATASET_SIZE / elapsed_seconds.count());
		startT = endT;
    };

    for (int sectionSize : SECTION_SIZES)
    {
        ProcessData(inputF, sectionSize, speedF);
        ProcessData(inputD, sectionSize, speedD);
    }

    CWorkbook book;
    CWorksheet& data = book.AddSheet("Data");

    data.BeginRow();
    data.AddCell();
    data.AddCell("float");
    data.AddCell("double");
    data.EndRow();

    for (size_t i = 0ULL; i < 6ULL; ++i)
    {
        data.BeginRow();
        data.AddCell(SECTION_SIZES[i]);
        data.AddCell(speedF.at(i));
        data.AddCell(speedD.at(i));
        data.EndRow();
    }

    data.BeginRow();
    data.AddCell("Число ядер: ");
    data.AddCell(std::thread::hardware_concurrency());
    data.EndRow();
    data.BeginRow();
    data.AddCell("Число отсчетов: ");
    data.AddCell(DATASET_SIZE);
    data.EndRow();

    CChart& OnSheetChart = book.AddChart(data, DrawingPoint(3, 3), DrawingPoint(19, 26));

    CChart::Series ser;
    ser.catSheet = &data;
    ser.valAxisFrom = CellCoord(2, 1);
    ser.valAxisTo = CellCoord(7, 1);
    ser.catAxisFrom = CellCoord(2, 0);
    ser.catAxisTo = CellCoord(7, 0);
    ser.valSheet = &data;
    ser.title = "Float";
    ser.JoinType = CChart::Series::joinLine;

    CChart::Series ser2 = ser;
    ser2.valAxisFrom = CellCoord(2, 2);
    ser2.valAxisTo = CellCoord(7, 2);
    ser2.title = "Double";

    OnSheetChart.AddSeries(ser);
    OnSheetChart.AddSeries(ser2);
    OnSheetChart.SetLegendPos(CChart::POS_RIGHT_ASIDE);

    book.Save("result.xlsx");

    return 0; 
}