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
  //Default constructor, sets to minimum strictness
  PieceSettings();

  //Constructor to set up required fields and optionally strictness
  //If no strictness specified, sets to minimum
  PieceSettings(float inLength, midi::Instrument inInst, std::uint8_t strict = 1);
  
  //Sets up values corresponding to a certain strictness
  //Does not properly set length or instrument
  void setStrictness(std::uint8_t strict);

  //--- Strictness Independent Variables ---
  //The overall (approximate) length of the piece in whole notes
  std::uint32_t length;

  //The instrument that will play the melody
  midi::Instrument instrumentMel;

  //--- Strictness Dependent Variables ---
  //The strictness of the piece on a scale from 1-5
  //1 will produce very random pieces, 5 will produce standard music sounding pieces
  std::uint8_t strictness;

  //Allow for motifs of length 1.5 measures
  bool allowFractionalMotifs;

  //Maximum number of mutations per motif is a function of stiffness
  std::uint32_t maxMutations;

  //The number of themes to generate is a function of stiffness
  std::uint16_t numThemes;
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
