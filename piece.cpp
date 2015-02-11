/*
  Copyright (c) 2014 Auston Sterling
  See LICENSE for copying permissions.
  
  -----Piece Class Implementation-----
  Auston Sterling
  austonst@gmail.com

  The implementation of the Piece class, representing an entire musical song.
*/

#include "piece.hpp"

#include <chrono>
#include <math.h>

//Default constructor, sets to minimum strictness
PieceSettings::PieceSettings() :
  length(0),
  instrumentMel(midi::INST_ACOUSTIC_GRAND_PIANO)
{
  setStrictness(1);
}

//Constructor to set up required fields and optionally strictness
//If no strictness specified, sets to minimum
PieceSettings::PieceSettings(float inLength, midi::Instrument inInst,
                             uint8_t strict) :
  length(inLength),
  instrumentMel(inInst)
{
  setStrictness(strict);
}

void PieceSettings::setStrictness(uint8_t strict)
{
  strictness = strict;

  maxMutations = 70 - strictness*10;
  numThemes = length/(6+strictness*2) + 0.5;
  
  if (strictness <= 1)
    {
      allowFractionalMotifs = true;
    }
  else if (strictness == 2)
    {
      allowFractionalMotifs = true;
    }
  else if (strictness == 3)
    {
      allowFractionalMotifs = false;
    }
  else if (strictness == 4)
    {
      allowFractionalMotifs = false;
    }
  else // >= 5
    {
      allowFractionalMotifs = false;
    }
}

//Generating constructor
Piece::Piece(const PieceSettings& set)
{
  generate(set);
}

//Generates a new piece from the given settings
void Piece::generate(PieceSettings set)
{
  //Create a Mersenne Twister to act as the RNG
  std::mt19937 gen;
  gen.seed(std::chrono::system_clock::now().time_since_epoch().count());
  
  //Create some global motifs
  //Number should be a function of length

  MotifGenSettings amSet(1, &gen, set.strictness);
  
  std::vector<AbstractMotif> globalMotifs;
  for (uint8_t i = 0; i < set.length/10; i++)
    {
      //allowFractionalMotifs true: length can be 1, 1.5 , or 2
      if (set.allowFractionalMotifs)
        {
          std::uniform_int_distribution<uint8_t> distMotifLen(0,2);
          amSet.length = (float(distMotifLen(gen))/2) + 1;
        }
      else //Otherwise, Length can be 1 or 2
        {
          std::uniform_int_distribution<uint8_t> distMotifLen(0,1);
          amSet.length = distMotifLen(gen) + 1;
        }
      
      globalMotifs.push_back(AbstractMotif(amSet));
    }

  //Choose some keys to base the piece in
  std::uniform_int_distribution<uint8_t> distKey(midi::Note("G3").midiVal(),
                                                 midi::Note("C5").midiVal());
  std::uniform_int_distribution<uint8_t> distKeyNum(2,5);
  std::vector<midi::Note> keys;
  for(uint8_t i = 0; i < distKeyNum(gen); i++)
    {
      keys.push_back(midi::Note(distKey(gen)));
    }

  //Choose a type of key for the piece to be based in
  std::uniform_int_distribution<uint8_t> distKeyType(0,2);
  uint8_t keyType = distKeyType(gen);

  //Generate a bunch of abstract themes with varying length and concreteness
  std::vector<AbstractTheme> abstrThemes;
  ThemeGenSettings atSet(0, globalMotifs, 0, &gen, set.strictness);
  std::uniform_int_distribution<uint8_t> distThemeLen(3,6);
  std::uniform_real_distribution<float> distConcrete(0,1);
  for (uint16_t i = 0; i < set.numThemes; i++)
    {
      atSet.length = distThemeLen(gen);
      atSet.concreteness = distConcrete(gen);
      abstrThemes.push_back(AbstractTheme(atSet));
    }

  //Now concretize it!
  std::vector<ConcreteTheme> concThemes;
  uint32_t length = 0;
  ThemeConcreteSettings ctSet(0, keyType, set.maxMutations, set.instrumentMel,
                              1500 /*No justifiation*/, &gen, set.strictness);

  std::uniform_int_distribution<uint8_t> distAbsTheme(0, set.numThemes-1);
  std::uniform_int_distribution<uint8_t> distSelectKey(0, keys.size()-1);
  while (length < set.length)
    {
      ctSet.key = keys[distSelectKey(gen)];
      ConcreteTheme ct(abstrThemes[distAbsTheme(gen)], ctSet);
      length += ct.ticks();
      concThemes.push_back(ct);
    }

  //Put them all in the NoteTrack
  notes_.clear();
  uint32_t totalTicks = 0;
  for (size_t i = 0; i < concThemes.size(); i++)
    {
      concThemes[i].addToTrack(notes_, totalTicks);
      totalTicks += concThemes[i].ticks();
    }
}

//Writes the piece to the specified MIDI file
void Piece::write(const std::string& filename) const
{
  midi::MIDI_Type0 mid(notes_, midi::TimeDivision(1500));
  mid.write(filename);
}
