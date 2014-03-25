/*
  -----Motif Class Header-----
  Auston Sterling
  austonst@gmail.com

  The header for the Motif class, representing a short sequence of notes in a piece.
  This would be analogous to a single word.
*/

#ifndef _motif_h_
#define _motif_h_

#include "midi/midi.h"

#include <vector>
#include <random>

//Helper struct to store non-concrete note information
//Time units could be anything, should be specified when used
struct AbstractNoteTime
{
  int8_t note;
  uint32_t begin;
  uint32_t duration;
};

//Helper struct for AbstractMotif generation
struct MotifGenSettings
{
  //The strictness of the theme
  uint8_t strictness;
  
  //The length of the motif in whole notes
  float length;

  //A pointer to a random number generator to be used in generation
  std::mt19937* gen;
};

//Helper struct for ConcreteMotif generation from an AbstractMotif
struct MotifConcreteSettings
{
  //The strictness of the theme
  uint8_t strictness;
  
  //The key of the concrete motif
  midi::Note key;

  //The type of key (0=major, 1=harmonic 2=natural minor)
  uint8_t keyType;

  //The points that can be spent on mutations
  uint32_t mutations;

  //The instrument that will play this motif
  midi::Instrument instrument;

  //The conversion between abstract and concrete time
  uint32_t ticksPerQuarter;

  //A note (in abstract format) to forcibly start the motif around
  bool forceStartNote;
  int8_t startNote;

  //A pointer to a Mersenne Twister to be used in generation
  std::mt19937* gen;
};

//An abstract motif, which contains the main information about a motif but lacks
//some details
class AbstractMotif
{
 public:
  //Constructors
  AbstractMotif() {length_=0;}
  AbstractMotif(const MotifGenSettings& set);

  //General use functions
  void generate(const MotifGenSettings& set);
  size_t numNotes() {return notes_.size();}
  void addToNote(uint32_t note, int8_t change) {notes_[note].note += change;}

  //Accessors
  AbstractNoteTime note(int n) const {return notes_[n];}
  float length() const {return length_;}
  size_t numNotes() const {return notes_.size();}
  
 private:
  //This is a collection of notes in an unspecified scale
  //0 being the lowest note and 7 being the highest.
  //Time units are in 32nd notes.
  std::vector<AbstractNoteTime> notes_;

  //The length of the motif in whole notes
  float length_;
};

//A concrete motif, which is effectively an instance of an abstract motif and
//can be played directly
class ConcreteMotif
{
 public:
  //Constructors
  ConcreteMotif(const AbstractMotif& abstr, const MotifConcreteSettings& set);

  //General use functions
  void generate(AbstractMotif abstr, MotifConcreteSettings set);
  void addToTrack(midi::NoteTrack& nt, uint32_t begin);
  uint32_t ticks() const;
  
 private:
  //A collection of notes, with time units being MIDI ticks
  std::vector<midi::NoteTime> notes_;

  //The settings, which need to be stored for ticks()
  MotifConcreteSettings set_;
};

#endif
