/*
  -----Motif Class Header-----
  Auston Sterling
  austonst@gmail.com

  The header for the Motif class, representing a short sequence of notes in a piece.
  This would be analogous to a single word.
*/

#ifndef _motif_h_
#define _motif_h_

#include "midi/midi.hpp"

#include <vector>
#include <random>

//Helper struct to store non-concrete note information
//Time units could be anything, should be specified when used
struct AbstractNoteTime
{
  std::int8_t note;
  std::uint32_t begin;
  std::uint32_t duration;
};

//Helper struct for AbstractMotif generation
struct MotifGenSettings
{
  //Default constructor, sets to minimum strictness
  MotifGenSettings();

  //Constructor to set up required fields and optionally strictness
  //If no strictness specified, sets to minimum
  MotifGenSettings(float inLength, std::mt19937* inGen, std::uint8_t strict = 1);
  
  //Sets up values corresponding to a certain strictness
  //Does not set length or gen!
  void setStrictness(std::uint8_t strict);

  //--- Strictness Independent Variables ---
  //The length of the motif in whole notes
  float length;

  //A pointer to a random number generator to be used in generation
  std::mt19937* gen;

  //--- Strictness Dependent Variables ---
  //If setStrictness used to generate this, this stores the given value
  std::uint8_t strictness;
  
  //A value of X here will make notes align their start time to a
  //multiple of notelength/X. A value of 0 means to not try to align.
  float noteAlign;

  //Force the first note of the motif to be 0
  bool forceFirstNote0;
};

//Helper struct for ConcreteMotif generation from an AbstractMotif
struct MotifConcreteSettings
{
  //Default constructor, sets to minimum strictness
  MotifConcreteSettings();

  //Constructor to set up required fields and optionally strictness
  //If no strictness specified, sets to minimum
  MotifConcreteSettings(midi::Note inKey, std::uint8_t inType, std::uint32_t inMut,
                        midi::Instrument inInst, std::uint32_t inTPQ,
                        bool inForceStart, std::int8_t inStart, std::mt19937* inGen,
                        std::uint8_t strict = 1);
  
  //Sets up values corresponding to a certain strictness
  //Currently has no effect
  void setStrictness(std::uint8_t strictness);

  //--- Strictness Independent Variables ---
  //The key of the concrete motif
  midi::Note key;

  //The type of key (0=major, 1=harmonic 2=natural minor)
  std::uint8_t keyType;

  //The points that can be spent on mutations
  std::uint32_t mutations;

  //The instrument that will play this motif
  midi::Instrument instrument;

  //The conversion between abstract and concrete time
  std::uint32_t ticksPerQuarter;

  //A note (in abstract format) to forcibly start the motif around
  bool forceStartNote;
  std::int8_t startNote;

  //A pointer to a Mersenne Twister to be used in generation
  std::mt19937* gen;

  //--- Strictness Dependent Variables ---
  //If setStrictness used to generate this, this stores the given value
  std::uint8_t strictness;
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
  std::size_t numNotes() {return notes_.size();}
  void addToNote(std::uint32_t note, std::int8_t change)
  {
    notes_[note].note += change;
  }

  //Accessors
  AbstractNoteTime note(int n) const {return notes_[n];}
  float length() const {return length_;}
  std::size_t numNotes() const {return notes_.size();}
  
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
  void addToTrack(midi::NoteTrack& nt, std::uint32_t begin);
  std::uint32_t ticks() const;
  
 private:
  //A collection of notes, with time units being MIDI ticks
  std::vector<midi::NoteTime> notes_;

  //The settings, which need to be stored for ticks()
  MotifConcreteSettings set_;
};

#endif
