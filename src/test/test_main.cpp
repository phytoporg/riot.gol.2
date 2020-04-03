#include <gtest/gtest.h>

#include <lib/Cell.h>
#include <lib/GOLGrid.h>

#include <algorithm>

//
// Clockwise neighbor addresses.
//
const static std::vector<gol::CellAddress> NeighborAddresses = {
    {-1, -1},
    { 0, -1},
    { 1, -1},
    { 1,  0},
    { 1,  1},
    { 0,  1},
    {-1,  1},
    {-1,  0}
};

static 
void GenerateNeighborCombinations_BacktrackHelper(
    std::vector<std::vector<gol::CellAddress>>& combinations,
    std::vector<gol::CellAddress>& candidates,
    size_t n,
    size_t i
)
{
    if (n == candidates.size()) 
    { 
        combinations.push_back(candidates); 
    }
    else
    {
        for (size_t j = i; j < NeighborAddresses.size(); ++j)
        {
            candidates.push_back(NeighborAddresses[j]);
            GenerateNeighborCombinations_BacktrackHelper(
                combinations,
                candidates,
                n,
                j + 1);
            candidates.pop_back();
        }
    }
}

//
// Generate all possible combinations of n neighbors, assuming a cell at (0, 0).
//
static 
std::vector<std::vector<gol::CellAddress>>
GenerateNeighborCombinations(size_t n)
{
    std::vector<std::vector<gol::CellAddress>> combinations;
    std::vector<gol::CellAddress> candidates;
    GenerateNeighborCombinations_BacktrackHelper(
        combinations,
        candidates,
        n,
        0
    );

    return combinations;
}

//
// Alive -> Dead tests for individual cells
//
class CellAliveToDeadFixture : public testing::TestWithParam<size_t> {};

TEST_P(CellAliveToDeadFixture, CellAliveToDead_Param)
{
    const size_t NumNeighbors{GetParam()};
    auto liveCellCollections = GenerateNeighborCombinations(NumNeighbors);

    for (auto& liveCells : liveCellCollections)
    {
        const gol::CellAddress CenterCellAddr(0, 0);
        liveCells.push_back(CenterCellAddr);

        gol::GOLGrid grid(liveCells);
        grid.AdvanceGeneration();
        
        const auto NextGenCells = grid.GetLiveCells();
        auto it = 
            std::find_if(
                std::begin(NextGenCells),
                std::end(NextGenCells),
                [&CenterCellAddr](const gol::Cell& cell)
                {
                    return CenterCellAddr == cell.Address;
                });
        ASSERT_EQ(it, std::end(NextGenCells));
    }
}

INSTANTIATE_TEST_CASE_P(CellAliveToDead_Param,
                        CellAliveToDeadFixture,
                        // Live cells die hard when surrounded by this many 
                        // neighbors
                        testing::Values(0, 1, 4, 5, 6, 7, 8));

//
// Alive -> Alive tests for individual cells
//
class CellAliveToAliveFixture : public testing::TestWithParam<size_t> {};

TEST_P(CellAliveToAliveFixture, CellAliveToAlive_Param)
{
    const size_t NumNeighbors{GetParam()};
    auto liveCellCollections = GenerateNeighborCombinations(NumNeighbors);

    for (auto& liveCells : liveCellCollections)
    {
        const gol::CellAddress CenterCellAddr(0, 0);
        liveCells.push_back(CenterCellAddr);

        gol::GOLGrid grid(liveCells);
        grid.AdvanceGeneration();
        
        const auto NextGenCells = grid.GetLiveCells();
        auto it = 
            std::find_if(
                std::begin(NextGenCells),
                std::end(NextGenCells),
                [&CenterCellAddr](const gol::Cell& cell)
                {
                    return CenterCellAddr == cell.Address;
                });
        ASSERT_NE(it, std::end(NextGenCells));
        ASSERT_TRUE(it->Alive);
    }
}

INSTANTIATE_TEST_CASE_P(CellAliveToAlive_Param,
                        CellAliveToAliveFixture,
                        // Live cells remain alive when surrounded by this many 
                        // neighbors
                        testing::Values(2, 3));

//
// Dead -> Alive tests for individual cells
//
class CellDeadToAliveFixture : public testing::TestWithParam<size_t> {};

TEST_P(CellDeadToAliveFixture, CellDeadToAlive_Param)
{
    const size_t NumNeighbors{GetParam()};
    auto liveCellCollections = GenerateNeighborCombinations(NumNeighbors);

    for (const auto& LiveCells : liveCellCollections)
    {
        gol::GOLGrid grid(LiveCells);
        grid.AdvanceGeneration();
        
        const gol::CellAddress CenterCellAddr(0, 0);
        const auto NextGenCells = grid.GetLiveCells();
        auto it = 
            std::find_if(
                std::begin(NextGenCells),
                std::end(NextGenCells),
                [&CenterCellAddr](const gol::Cell& cell)
                {
                    return CenterCellAddr == cell.Address;
                });
        ASSERT_NE(it, std::end(NextGenCells));
        ASSERT_TRUE(it->Alive);
    }
}

INSTANTIATE_TEST_CASE_P(CellDeadToAlive_Param,
                        CellDeadToAliveFixture,
                        // Dead cells come to life when surrounded by this many 
                        // neighbors
                        testing::Values(3));

//
// Dead -> Dead tests for individual cells
//
class CellDeadToDeadFixture : public testing::TestWithParam<size_t> { };

TEST_P(CellDeadToDeadFixture, CellDeadToDead_Param)
{
    const size_t NumNeighbors{GetParam()};
    auto liveCellCollections = GenerateNeighborCombinations(NumNeighbors);

    for (auto& liveCells : liveCellCollections)
    {
        gol::GOLGrid grid(liveCells);
        grid.AdvanceGeneration();
        
        const gol::CellAddress CenterCellAddr(0, 0);
        const auto NextGenCells = grid.GetLiveCells();
        auto it = 
            std::find_if(
                std::begin(NextGenCells),
                std::end(NextGenCells),
                [&CenterCellAddr](const gol::Cell& cell)
                {
                    return CenterCellAddr == cell.Address;
                });
        ASSERT_EQ(it, std::end(NextGenCells));
    }
}

INSTANTIATE_TEST_CASE_P(CellDeadToDead_Param,
                        CellDeadToDeadFixture,
                        // Dead cells stay dead when surrounded by this many 
                        // neighbors
                        testing::Values(1, 2, 4, 5, 6, 7, 8));

//
// Set up a blinker and ensure the state alternates across multiple generations.
//
TEST(MultiGenerationTests, BlinkerTest)
{
    using namespace gol;

    const std::vector<CellAddress> BlinkerStates[] =
    {
        { CellAddress(-1, 0), CellAddress(0, 0), CellAddress(1,  0) },
        { CellAddress( 0, 1), CellAddress(0, 0), CellAddress(0, -1) },
    };

    const size_t TotalGenerations{10};
    GOLGrid grid(BlinkerStates[0]);
    for (size_t i; i < TotalGenerations; ++i)
    {
        //
        // Assert that all live cells in the expected blinker state are alive
        // in the grid and vice versa.
        //
        const auto LiveCells = grid.GetLiveCells();
        for (const auto& searchCell : LiveCells)
        {
            auto it = std::find_if(
                std::begin(BlinkerStates[i]),
                std::end(BlinkerStates[i]),
                [&searchCell](const CellAddress& cellAddress)
                {
                    return searchCell.Address == cellAddress;
                });
            
            ASSERT_NE(it, std::end(BlinkerStates[i]));
        }

        for (const auto& searchAddr : BlinkerStates[i])
        {
            auto it = std::find_if(
                std::begin(LiveCells),
                std::end(LiveCells),
                [&searchAddr](const Cell& cell)
                {
                    return cell.Address == searchAddr;
                });
            
            ASSERT_NE(it, std::end(LiveCells));
        }

        grid.AdvanceGeneration();
    }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

