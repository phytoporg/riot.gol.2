#include <gtest/gtest.h>

#include <lib/Cell.h>
#include <lib/GOLGrid.h>

#include <algorithm>

//
// Clockwise neighbor addresses.
//
const static std::vector<gol::CellAddress> NeighborAddresses = {
    std::make_pair<int64_t, int64_t>(-1, -1),
    std::make_pair<int64_t, int64_t>( 0, -1),
    std::make_pair<int64_t, int64_t>( 1, -1),
    std::make_pair<int64_t, int64_t>( 1,  0),
    std::make_pair<int64_t, int64_t>( 1,  1),
    std::make_pair<int64_t, int64_t>( 0,  1),
    std::make_pair<int64_t, int64_t>(-1,  1),
    std::make_pair<int64_t, int64_t>(-1,  0)
};

//
// Generate all possible combinations of n neighbors, assuming a cell at (0, 0).
//
static void GenerateNeighborCombinations_BacktrackHelper(
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

static std::vector<std::vector<gol::CellAddress>>
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
class CellAliveToDeadFixture :
    public testing::TestWithParam<size_t> {
};

TEST_P(CellAliveToDeadFixture, CellAliveToDead_Param)
{
    const size_t NumNeighbors{GetParam()};
    auto liveCellCollections = GenerateNeighborCombinations(NumNeighbors);

    for (auto& liveCells : liveCellCollections)
    {
        const gol::CellAddress CenterCellAddr{
            std::make_pair<int64_t, int64_t>(0, 0)};
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
                        testing::Values(0, 1, 4, 5, 6, 7, 8));

//
// Alive -> Dead tests for individual cells
//
class CellAliveToAliveFixture :
    public testing::TestWithParam<size_t> {
};

TEST_P(CellAliveToAliveFixture, CellAliveToAlive_Param)
{
    const size_t NumNeighbors{GetParam()};
    auto liveCellCollections = GenerateNeighborCombinations(NumNeighbors);

    for (auto& liveCells : liveCellCollections)
    {
        const gol::CellAddress CenterCellAddr{
            std::make_pair<int64_t, int64_t>(0, 0)};
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
        ASSERT_TRUE(it->State.Alive);
    }
}

INSTANTIATE_TEST_CASE_P(CellAliveToAlive_Param,
                        CellAliveToAliveFixture,
                        testing::Values(2, 3));

//
// Dead -> Alive tests for individual cells
//
class CellDeadToAliveFixture :
    public testing::TestWithParam<size_t> {
};

TEST_P(CellDeadToAliveFixture, CellDeadToAlive_Param)
{
    const size_t NumNeighbors{GetParam()};
    auto liveCellCollections = GenerateNeighborCombinations(NumNeighbors);

    for (const auto& LiveCells : liveCellCollections)
    {
        gol::GOLGrid grid(LiveCells);
        grid.AdvanceGeneration();
        
        const gol::CellAddress CenterCellAddr{
            std::make_pair<int64_t, int64_t>(0, 0)};
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
        ASSERT_TRUE(it->State.Alive);
    }
}

INSTANTIATE_TEST_CASE_P(CellDeadToAlive_Param,
                        CellDeadToAliveFixture,
                        testing::Values(3));

//
// Dead -> Dead tests for individual cells
//
class CellDeadToDeadFixture :
    public testing::TestWithParam<size_t> {
};

TEST_P(CellDeadToDeadFixture, CellDeadToDead_Param)
{
    const size_t NumNeighbors{GetParam()};
    auto liveCellCollections = GenerateNeighborCombinations(NumNeighbors);

    for (auto& liveCells : liveCellCollections)
    {
        for (const auto& cell : liveCells)
        {
            using namespace std;
            auto it = find(begin(NeighborAddresses), end(NeighborAddresses), cell);
        }

        gol::GOLGrid grid(liveCells);
        grid.AdvanceGeneration();
        
        const gol::CellAddress CenterCellAddr{
            std::make_pair<int64_t, int64_t>(0, 0)};
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
                        testing::Values(1, 2, 4, 5, 6, 7, 8));

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
