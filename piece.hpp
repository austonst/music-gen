/*
  -----Piece Class Header-----
  Auston Sterling
  austonst@gmail.com

  The header file for the Piece class, representing an entire musical song.
*/

#ifndef _piece_h_
#define _piece_h_

#include "theme.hpp"

#include <string>

struct PieceSettings
{
  //The overall (approximate) length of the piece in whole notes
  uint32_t length;

  //The strictness of the piece on a scale from 1-5
  //1 will produce very random pieces, 5 will produce standard music sounding pieces
  uint8_t strictness;

  //The instrument that will play the melody
  midi::Instrument instrumentMel;
};

class Piece
{
 public:
  //Constructors
  Piece(const PieceSettings& set);

  //General use functions
  void generate(PieceSettings set);
  void write(const std::string& filename) const;

 private:
  //The notes in the piece
  midi::NoteTrack notes_;
};

#endif
