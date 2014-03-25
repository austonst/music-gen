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

  MotifGenSettings amSet;
  amSet.strictness = set.strictness;
  amSet.gen = &gen;
  
  std::vector<AbstractMotif> globalMotifs;
  for (uint8_t i = 0; i < set.length/10; i++)
    {
      //Strictness 1-2: Length can be 1, 1.5 , or 2
      if (set.strictness <= 2)
        {
          std::uniform_int_distribution<uint8_t> distMotifLen(0,2);
          amSet.length = (float(distMotifLen(gen))/2) + 1;
        }
      else //Strictness 3-5: Length can be 1 or 2
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

  //Maximum number of mutations per motif is a function of strictness
  uint32_t maxMutations = 70 - set.strictness*10;

  //Generate a bunch of abstract themes with varying length and concreteness
  uint16_t numThemes = set.length/(6+set.strictness*2) + 0.5;
  std::vector<AbstractTheme> abstrThemes;
  ThemeGenSettings atSet;
  atSet.strictness = set.strictness;
  atSet.motifs = globalMotifs;
  atSet.gen = &gen;
  std::uniform_int_distribution<uint8_t> distThemeLen(3,6);
  std::uniform_real_distribution<float> distConcrete(0,1);
  for (uint16_t i = 0; i < numThemes; i++)
    {
      atSet.length = distThemeLen(gen);
      atSet.concreteness = distConcrete(gen);
      abstrThemes.push_back(AbstractTheme(atSet));
    }

  //Now concretize it!
  std::vector<ConcreteTheme> concThemes;
  uint32_t length = 0;
  ThemeConcreteSettings ctSet;
  ctSet.strictness = set.strictness;
  ctSet.keyType = keyType;
  ctSet.maxMutations = maxMutations;
  ctSet.instrument = set.instrumentMel;
  ctSet.ticksPerQuarter = 1500; //Just some number that works... Could randomize it
  ctSet.gen = &gen;
  set.length *= 6000;
  std::uniform_int_distribution<uint8_t> distAbsTheme(0, numThemes-1);
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
