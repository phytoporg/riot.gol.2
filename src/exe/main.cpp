//
// Entry point for gol2!
//
// Reads in a Life 1.06 file and run a user-specified number of generations of
// the Game of Life before producing the state to stdout in a Life 1.06-
// compatible format.
//

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>

#include <lib/Cell.h>
#include <lib/GOLGrid.h>

void PrintUsage(const std::string& progName)
{
    std::cerr << "Usage: " << progName << " <input_path> <num_iterations>"
              << std::endl;
}

const std::string LifeSignature("#Life 1.06");
void DumpCells(const std::vector<gol::Cell>& cells, std::ostream& out)
{
    out << LifeSignature << "\n";

    for (const auto& Cell : cells)
    {
        out << Cell.Address.first << " " << Cell.Address.second;

        if (&Cell != &cells.back())
        {
            out << "\n";
        }
    }

    out.flush();
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        PrintUsage(argv[0]);
        return -1;
    }

    const std::string InputPath(argv[1]);
    std::ifstream in(InputPath);
    if (!in)
    {
        std::cerr << "Could not load file: " << InputPath << std::endl;
    }

    uint32_t numIterations;
    try
    {
        numIterations = static_cast<uint32_t>(std::stoi(argv[2]));
    }
    catch (std::exception& /*e*/)
    {
        std::cout << "Invalid iterations parameter." << std::endl;
    }

    //
    // Check signature
    //
    std::string line;
    if(!(std::getline(in, line) && line == LifeSignature))
    {
        std::cerr << "Invalid GOL format!" << std::endl;
        return -1;
    }

    //
    // Read cells
    //
    int64_t x;
    int64_t y;
    std::vector<gol::CellAddress> initialCells;
    while(in >> x >> y) { initialCells.emplace_back(x, y); }

    if (initialCells.empty())
    {
        std::cerr << "Please specify at least one live cell in the input."
                  << std::endl;
        return -1;
    }

    gol::GOLGrid grid(initialCells);
    for (uint32_t i = 0; i < numIterations; ++i)
    {
        grid.AdvanceGeneration();
    }

    DumpCells(grid.GetLiveCells(), std::cout);

    return 0;
}

