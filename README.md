
# DucaPowr - Chess engine

## Usage

### Build

```bash
make build
```

### Run

```bash
chmod +x ./run.sh
./run.sh
```

## Project Structure

### Xboard Handler Logic

The `xBoardHandler` class is an interface between the Xboard API and the main engine. One of the roles it plays is parsing the given commands, converting them to an internal format and replying back.

Another role of this handler is to feed the processed commands to the engine API. Also, it awaits a generated move to send back a command.

<details>
<summary>Note to the programmer</summary>
We have not implemented move legality checks. 
</details>

### Engine Logic

The main class of this program is `Engine`. It stores internally the board state and a move generator.

Receiving a move from XBoard is done through the `userMove()` method. It updates the board state at every move.

A move is chosen from a pool of generated moves randomly, for now. We will add proper selection logic in the future.

### Board Logic

For this project we chose to use [bitboard](https://www.chessprogramming.org/Bitboards) representation, with [little endian rank-file mapping](https://www.chessprogramming.org/Square_Mapping_Considerations#Little-Endian_File-Rank_Mapping). The reason behind this choice is that they provide the right balance between time and space complexity.

We use 13 bitboards, one for each piece of each color plus one trash piece (required to optimize the code).

### Generation Logic

<details>
<summary>Note</summary>
Until now, we only generated moves for pawns.
</details>

At each turn we check which side has to move and call one of the corresponding methods:

- `whitePawnMoves();`
- `whitePawnAttacks();`
- `blackPawnMoves();`
- `blackPawnAttacks();`

Pawns are non-sliding pieces. Therefore, their generation is easy to implement using bitwise operations. For example, shifting the bitboard by 8, gives us a bitboard in which all the pieces have moved one square forward. (More about pawn pushes [here](https://www.chessprogramming.org/Pawn_Pushes_(Bitboards)).)

### Further details

To find more about the internals of the engine check the code comments and `Internals.md` file.

## Epilogue

At the end of the game, the bot brings to the user's attention an insightful quote.

## Resources
1. [3-check chess](https://www.chess.com/terms/3-check-chess)
2. [3-check traps](https://www.chess.com/article/view/the-3-check-traps-you-need-to-know)
3. [3-check openings](https://www.chess.com/forum/view/chess960-chess-variants/3-checks-best-openings-1)
4. [chessprogramming: a very resourceful site for chess engines](https://www.chessprogramming.org/Main_Page)
5. [A chess engine that can play 3-check chess](https://github.com/ianfab/Fairy-Stockfish)
6. [Stockfish: a very powerful chess engine](https://github.com/ddugovic/Stockfish)
7. [How do computers play three-check chess?](https://chess.stackexchange.com/questions/18903/how-do-computers-play-three-check-chess)
8. [How Stockfish works](http://rin.io/chess-engine/)
9. [Bitboard](https://www.chessprogramming.org/Bitboards)

