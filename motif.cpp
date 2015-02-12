/*
  Copyright (c) 2014 Auston Sterling
  See LICENSE for copying permissions.
  
  -----Motif Class Implementation-----
  Auston Sterling
  austonst@gmail.com

  The motif class, representing a short sequence of notes in a piece.
  This would be analogous to a single word.
*/

#ifndef _motif_cpp_
#define _motif_cpp_

#include "motif.hpp"
#include "midi/scales.hpp"

#include <cmath>

//Default constructor, sets to minimum strictness
MotifGenSettings::MotifGenSettings() :
  length(0),
  gen(nullptr)
{
  setStrictness(0);
}

//Constructor to set up required fields and optionally strictness
MotifGenSettings::MotifGenSettings(float inLength, std::mt19937* inGen,
                                   std::uint8_t strict) :
  length(inLength),
  gen(inGen)
{
  setStrictness(strict);
}

void MotifGenSettings::setStrictness(std::uint8_t strict)
{
  strictness = strict;
  
  if (strictness <= 1)
    {
      noteAlign = 0;
      forceFirstNote0 = false;
      noteSelection = 0;
    }
  else if (strictness == 2)
    {
      noteAlign = 0;
      forceFirstNote0 = false;
      noteSelection = 1;
    }
  else if (strictness == 3)
    {
      noteAlign = 8;
      forceFirstNote0 = false;
      noteSelection = 1;
    }
  else if (strictness == 4)
    {
      noteAlign = 4;
      forceFirstNote0 = true;
      noteSelection = 1;
    }
  else // >= 5
    {
      noteAlign = 2;
      forceFirstNote0 = true;
      noteSelection = 1;
    }
}

//Default constructor, sets to minimum strictness
MotifConcreteSettings::MotifConcreteSettings() :
  key(0),
  keyType(0),
  mutations(0),
  instrument(midi::INST_ACOUSTIC_GRAND_PIANO),
  ticksPerQuarter(1500), //No justification for this
  forceStartNote(false),
  gen(nullptr)
{
  setStrictness(1);
}

//Constructor to set up required fields and optionally strictness
//If no strictness specified, sets to minimum
MotifConcreteSettings::MotifConcreteSettings(midi::Note inKey, std::uint8_t inType,
                                             std::uint32_t inMut, midi::Instrument inInst,
                                             std::uint32_t inTPQ, bool inForceStart,
                                             std::int8_t inStart, std::mt19937* inGen,
                                             std::uint8_t strict) :
  key(inKey),
  keyType(inType),
  mutations(inMut),
  instrument(inInst),
  ticksPerQuarter(inTPQ), //No justification for this
  forceStartNote(inForceStart),
  startNote(inStart),
  gen(inGen)
{
  setStrictness(strict);
}

void MotifConcreteSettings::setStrictness(std::uint8_t strict)
{
  strictness = strict;
  
  if (strictness <= 1)
    {
    }
  else if (strictness == 2)
    {
    }
  else if (strictness == 3)
    {
    }
  else if (strictness == 4)
    {
    }
  else // >= 5
    {
    }
}

//General use constructor
AbstractMotif::AbstractMotif(const MotifGenSettings& set)
{
  generate(set);
}

//Randomly generates an AbstractMotif given the settings
void AbstractMotif::generate(const MotifGenSettings& set)
{
  //Variables and initialization
  float pos = 0;
  std::normal_distribution<float> distLen(2,1);
  std::uniform_int_distribution<std::uint8_t> distNote(0,7);
  std::normal_distribution<float> distLenOffset(.7,.5);
  length_ = set.length;
  notes_.clear();
  std::int8_t lastNote = 0;

  //Generate notes until it's full
  while (pos < set.length-0.001)
    {
      //Choose the length of the next note, from a 32nd note to a whole note
      //So choose n in (2^n)th note from 0-5
      float offset = -1;
      while (offset < .2 || offset > 2) offset = distLenOffset(*(set.gen));
      std::int8_t rand = -1;
      while (rand < 0) rand = distLen(*(set.gen)) + offset;
      float noteLength = 1.0 / float(1 << rand);

      //If noteAlign set, notes should start on multiples of their note length
      if (set.noteAlign > 0.001)
        {
          while (fmod(pos,noteLength/set.noteAlign) > 0.001) noteLength /= 2;
        }

      //Notes should ALWAYS be aligned to whole notes when at measure bounds
      /*while (noteLength > 1 - fmod(pos,1.0))
        {
          noteLength /= 2;
        }*/
      
      //If this is longer than the time left, reduce until it does
      while (noteLength > set.length-pos)
        {
          noteLength /= 2;
        }

      //Add the corresponding AbstractNoteTime
      AbstractNoteTime ant;
      ant.begin = pos*32 + .001;
      ant.duration = noteLength*32 + .001;

      //Depending on forceFirstNote0, first note must be 0
      if (notes_.size() == 0 && set.forceFirstNote0)
        {
          ant.note = 0;
        }
      //If noteSelection is 0, note can be anywhere in the scale of the octave
      else if (set.noteSelection == 0)
        {
          ant.note = distNote(*(set.gen));
        }
      else //noteSelection=1: note is selected using normal distribution
        {
          std::normal_distribution<float> distNormNote(lastNote, 2);
          float normNote;
          do
            {
              normNote = distNormNote(*(set.gen)) + 0.5;
            }
          while (normNote < lastNote+.1 && normNote > lastNote-.1);
          ant.note = normNote;
          lastNote = ant.note + 0.5;
        }
      
      notes_.push_back(ant);

      //Move pos up
      pos += noteLength;
    }
}

//General use constructor
ConcreteMotif::ConcreteMotif(const AbstractMotif& abstr, const MotifConcreteSettings& set)
{
  generate(abstr, set);
}

/*
  --Mutations and costs--
  0) Bring one note up or down one semitone: 8 pts, direction limited
  1) UNUSED
  2) Increase or decrease key of Motif: 12 pts, direction limited
  3) Change key type: 10 pts, once per generation
  4) Increase or decrease Motif tempo: 12 pts, direction limited
  5) UNUSED
*/

//Randomly generates a ConcreteMotif given the settings
void ConcreteMotif::generate(AbstractMotif abstr, MotifConcreteSettings set)
{
  //Keep track of limited changes
  std::uint8_t numNotes = abstr.numNotes();
  std::vector<std::uint8_t> limit0(numNotes, 0); //0: Unmodified, 1: Up, 2: Down
  //std::vector<std::uint8_t> limit1(numNotes, 0); //0: Unmodified, 1: Modified
  std::uint8_t limit2 = 0; //0: Unmodified, 1: Up, 2: Down
  std::uint8_t limit3 = 0; //0: Unmodified, 1: Modified
  std::uint8_t limit4 = 0; //0: Unmodified, 1: Up, 2: Down
  std::vector<std::uint8_t> limit5(numNotes, 0); //0: Unmodified, 1: Long, 2: Short

  //Set up RNG
  std::uniform_int_distribution<std::uint8_t> distMut(0,0);
  std::uniform_int_distribution<std::uint8_t> distBool(0,1);
  std::uniform_int_distribution<std::uint32_t> distNote(0,numNotes-1);

  //Repeatedly apply mutations until mutation points are depleted or
  //a certain number of tries is exceeded without success
  const std::uint8_t MAX_FAILURES = 20;
  std::uint8_t currentFailures = 0;
  std::uint32_t pointsSpent = 0;
  std::uint32_t note;
  while (currentFailures < MAX_FAILURES && pointsSpent < set.mutations)
    {
      bool success = false;
      
      //Choose the mutation
      switch(distMut(*(set.gen)))
        {
        case 0:
          //Check remaining points
          if (set.mutations - pointsSpent < 8) break;
          
          //Select the note and choose a direction if it is not already chosen
          note = distNote(*(set.gen));
          if (limit0[note] == 0) limit0[note] = distBool(*(set.gen))+1;

          //Modify
          if (limit0[note] == 1) abstr.addToNote(note, 1);
          if (limit0[note] == 2) abstr.addToNote(note, -1);

          //Finish up
          pointsSpent += 8;
          success = true;
          break;
          
        case 1:
          //UNUSED
          break;
          
        case 2:
          //Check remaining points
          if (set.mutations - pointsSpent < 12) break;

          //Choose a direction if it is not already chosen
          if (limit2 == 0) limit2 = distBool(*(set.gen))+1;

          //Modify
          if (limit2 == 1) set.key = set.key + 1;
          if (limit2 == 2) set.key = set.key - 1;

          //Finish up
          pointsSpent += 12;
          success = true;
          break;
          
        case 3:
          //Check remaining points
          if (set.mutations - pointsSpent < 5) break;

          //Only do this once
          if (limit3 == 1) break;

          //Modify
          if (set.keyType == 0) set.keyType = distBool(*(set.gen)) + 1;
          else if (set.keyType == 1) set.keyType = 2 * distBool(*(set.gen));
          else set.keyType = distBool(*(set.gen));

          //Finish up
          pointsSpent += 10;
          success = true;
          break;
          
        case 4:
          //Check remaining points
          if (set.mutations - pointsSpent < 12) break;

          //Choose a direction if it is not already chosen
          if (limit4 == 0) limit4 = distBool(*(set.gen))+1;

          //Modify
          if (limit4 == 1) set.ticksPerQuarter *= .75;
          if (limit4 == 2) set.ticksPerQuarter = set.ticksPerQuarter * 1.33333 + 0.5;

          //Finish up
          pointsSpent += 12;
          success = true;
          
          break;
          
        case 5:
          //UNUSED

          break;
        }

      if (success)
        {
          currentFailures = 0;
        }
      else
        {
          currentFailures++;
        }
    }

  std::int8_t diffNote = 0;
  if (set.forceStartNote)
    {
      std::normal_distribution<float> distNormNote(0, 2);
      diffNote = set.startNote - abstr.note(0).note + distNormNote(*(set.gen));
    }

  //Convert all of the abstract notes to concrete notes
  notes_.clear();
  for (std::size_t i = 0; i < numNotes; i++)
    {
      midi::NoteTime nt;
      if (set.keyType == 0)
        {
          nt.note = midi::majorScale(set.key, abstr.note(i).note+diffNote);
        }
      else if (set.keyType == 1)
        {
          nt.note = midi::harMinorScale(set.key, abstr.note(i).note+diffNote);
        }
      else
        {
          nt.note = midi::natMinorScale(set.key, abstr.note(i).note+diffNote);
        }

      nt.begin = (float(abstr.note(i).begin)/8.0) * set.ticksPerQuarter;
      nt.duration = (float(abstr.note(i).duration)/8.0) * set.ticksPerQuarter;
      nt.instrument = set.instrument;
      notes_.push_back(nt);
    }

  //Store settings for ticks()
  set_ = set;
}

//Adds this concrete motif to a NoteTrack starting at begin
void ConcreteMotif::addToTrack(midi::NoteTrack& nt, std::uint32_t begin)
{
  for (std::size_t i = 0; i < notes_.size(); i++)
    {
      midi::NoteTime note = notes_[i];
      note.begin += begin;
      nt.add(note);
    }
}

//Returns the length of the motif in ticks
std::uint32_t ConcreteMotif::ticks() const
{
  std::uint32_t longest = 0;

  for (std::size_t i = 0; i < notes_.size(); i++)
    {
      if (notes_[i].begin + notes_[i].duration > longest)
        {
          longest = notes_[i].begin + notes_[i].duration;
        }
    }
  return longest;
}

#endif
