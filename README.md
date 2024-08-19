# ChessEngine

## Overview

This is a chess engine that communicates using [UCI](https://en.wikipedia.org/wiki/Universal_Chess_Interface). This means it can communicate with a variety of GUIs, ex: [Arena GUI](https://www.playwitharena.de).

This engine applies heurstics and alpha beta pruning to explore all possible moves and choose the best one, pruning off bad moves early into the search if possible. It uses [bitboards](https://en.wikipedia.org/wiki/Bitboard) to store the board position within a combination of 64 bit integers, allowing board operations to be applied through bitwise operators. This allows for very fast board operations and is a technique employed by many popular chess engines, including stockfish.


## Play against it!

This bot is playable on lichess periodically!
[Play now!](https://www.lichess.org/@/WinnerEngine)

Note this bot may offline, but you can also view past games it has played!
