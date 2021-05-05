# DucaPowr - Chess engine

## Usage

### Build

```bash
make build
```

### Debug
```bash
make build DEBUG='-g -O0'
```

### Run
To run the executable
```bash
make run
```

To run xboard with Duca Engine
```bash
./run.sh
```

## Project Structure

### Xboard Handler Logic

The `xBoardHandler` class is an interface between the Xboard API and the main engine. One of the roles it plays is parsing the given commands, converting them to an internal format and replying back.

Another role of this handler is to feed the processed commands to the engine API. Also, it awaits a generated move to send back a command.

### Engine Logic

The main class of this program is `Engine`. It internally stores the board state and a move generator.

Receiving a move from XBoard is done through the `userMove()` method. It updates the board state at every move.

A move is chosen randomly from a pool of generated moves, for now. We will add proper selection logic in the future.

### Board Logic

For this project we chose to use [bitboard](https://www.chessprogramming.org/Bitboards) representation, with [little endian rank-file mapping](https://www.chessprogramming.org/Square_Mapping_Considerations#Little-Endian_File-Rank_Mapping). The reason behind this choice is that they provide the right balance between time and space complexity.

We use 13 bitboards, one for each piece of each color plus one trash piece (required to optimize the code).

### Generation Logic

At each turn we check which side has to move and call one of the corresponding methods
that generate moves for each piece.

Pawns are non-sliding pieces. Therefore, their generation is easy to implement using bitwise operations. For example, shifting the bitboard by 8, gives us a bitboard in which all the pieces have moved one square forward. (More about pawn pushes [here](https://www.chessprogramming.org/Pawn_Pushes_(Bitboards)).)

### Move encoding

XBoard sends the move to the handler in a Pure Coordinate String Format (eg. "e3f4"). The move is stored on 16 bits encoded as:

- bits 0-5 source square
- bits 6-11 destination square
- bits 12-13 promotion (0 - rook, 1 - knight, 2 - bishop, 3 - queen)
- bits 14-15 flags (0 - none, 1 - promotion, 2 - en passant, 3 - castling)

Stockfish uses a similar [encoding](https://github.com/official-stockfish/Stockfish/blob/b1c8840f104d4d36b3f9a420b8b26a5e94c6dc18/src/types.h#L112).

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

## Each member's duties
* Andrei IONESCU - bitboards research; movement generator implementation; internal move execution; internal move undo; knight moves;
* Dimitrie DAVID - team leader; create xBoardHandler and initial engine flow design and framework; made sure the coding style is correct; move legality checking; xBoardHandler updates;
* Matei BARBU - task distribution and schedule planner; research on different chess board representations and algorithms; implementation of internal move representation; king moves and castling;
* Razvan PRICOP - in-depth research of different chess aspects, algorithms and data structures; implemented board representation, pawn movement generation and other utils; queen, bishop and rook moves; perft checking;

** Everyone worked really hard to fix many bugs and to do many imporvements that we did not mention above.
