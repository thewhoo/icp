/**
 * Course ICP @ FIT VUT Brno, 2016
 * ICP 2016 Project - Othello
 *
 * @author Plaskon Pavol, xplask00@stud.fit.vutbr.cz
 * @author Postolka Matej, xposto02@stud.fit.vutbr.cz
 *
 * @brief A representation of Othello table.
 * @file table.h
 *
 * Unless otherwise stated, all code is licensed under a
 * GNU General Public License v2.0
 *
 */

#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <string>
#include <vector>

#include "boost/serialization/base_object.hpp"
#include "boost/serialization/utility.hpp"
#include "boost/serialization/vector.hpp"

/**
 * @brief A representation of Othello table.
 */
class Table
{
  public:
    /// Representation of the black stone on the table.
    static const std::string CLI_BLACK_STONE;
    /// Representation of the white stone on the table.
    static const std::string CLI_WHITE_STONE;

    /// @name Table constants.
    /// @{
    static const int MIN_ROWS = 4;
    static const int MIN_COLS = 4;
    static const int DEFAULT_ROWS = 8;
    static const int DEFAULT_COLS = 8;
    /// @}

    enum class Stone
    {
      FREE,
      BLACK,
      WHITE,
    };

    /// Definition of coordinates.
    using Coords = std::pair<int, int>;

  private:
    void fillCacheVector(const Coords& coords, Stone stone) const;
    void turnStonesByVector(int x, int y, const Coords& startCoords, Stone ownStone) const;
    void clearCache() const;

  private:

    struct _board
    {
        /// Rows count.
        int rows;
        /// Columns count.
        int cols;
        /// Count of black stones on the table.
        int blackStones;
        /// Count of white stones on the table.
        int whiteStones;
        /// Count total moves made.
        int moveCount;

        /// Matrix representing table state.
        std::vector<Stone> tableMatrix;

      private:
        /// @name BOOST serialization.
        /// @{
        friend std::ostream &operator<<(std::ostream &os, const struct _board &b);
        friend class boost::serialization::access;

        template<typename Archive>
        void serialize(Archive &ar, const unsigned int /* file version */)
        {
          ar & rows & cols &blackStones & whiteStones & moveCount & tableMatrix;
        }
        /// @}
    };

    struct _board board;

    mutable std::vector<Coords> stoneFlipCache;
    mutable Coords cachedCoords;
    mutable Stone cachedStone;

  public:
    using Board = struct _board;

  public:
    Table();
    Table(int initRows, int initCols);

    bool isMoveWithStonePossible(Stone stone) const;
    bool canPutStone(const Coords& coords, Stone stone) const;
    void putStone(const Coords& coords, Stone stone);

    const Board &getBoard() const;
    void setBoard(const Board &board);

    int getRowCount() const;
    int getColCount() const;
    int getBlackStonesCount() const;
    int getWhiteStonesCount() const;
    int getMoveCount() const;
    int getVecIndex(const Coords& coords) const;
    void recountScores();
    
    std::vector<Coords> getPossibleCoords(Stone stone);

    void print() const;
};

#endif
