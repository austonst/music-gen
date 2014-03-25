/*
  Copyright (c) 2014 Auston Sterling
  See LICENSE for copying permissions.

  -----Piece Test Program-----
  Auston Sterling
  austonst@gmail.com

  A program to test the generation of an entire piece of music.
*/

#include "piece.hpp"

int main()
{
  PieceSettings set;
  set.length = 20;
  set.strictness = 5;
  set.instrumentMel = 0;

  Piece p(set);
  p.write("testpiece.mid");
}
