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
  PieceSettings set(20, midi::INST_ACOUSTIC_GRAND_PIANO, 5);

  Piece p(set);
  p.write("testpiece.mid");
}
