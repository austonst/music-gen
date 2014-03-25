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
#include "midi/scales.h"

#include <math.h>

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
  std::uniform_int_distribution<uint8_t> distNote(0,7);
  std::normal_distribution<float> distLenOffset(.7,.5);
  length_ = set.length;
  notes_.clear();
  int8_t lastNote = 0;

  //Generate notes until it's full
  while (pos < set.length-0.001)
    {
      //Choose the length of the next note, from a 32nd note to a whole note
      //So choose n in (2^n)th note from 0-5
      float offset = -1;
      while (offset < .2 || offset > 2) offset = distLenOffset(*(set.gen));
      int8_t rand = -1;
      while (rand < 0) rand = distLen(*(set.gen)) + offset;
      float noteLength = 1.0 / float(1 << rand);

      //Strictness 3-5: Notes should start on multiples of their note length
      if (set.strictness == 5)
        {
          while (fmod(pos,noteLength/2) > 0.001) noteLength /= 2;
        }
      else if (set.strictness == 4)
        {
          while (fmod(pos,noteLength/4) > 0.001) noteLength /= 2;
        }
      else if (set.strictness == 3)
        {
          while (fmod(pos,noteLength/8) > 0.001) noteLength /= 2;
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

      //Strictness 4-5: First note must be 0
      if (notes_.size() == 0 && set.strictness >= 4)
        {
          ant.note = 0;
        }
      //Strictness 1: Note can be anywhere in the scale of the octave
      else if (set.strictness == 1)
        {
          ant.note = distNote(*(set.gen));
        }
      else //Strictness 2-5: Note is selected from a normal dist centered on last note
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
  uint8_t numNotes = abstr.numNotes();
  std::vector<uint8_t> limit0(numNotes, 0); //0: Unmodified, 1: Up, 2: Down
  //std::vector<uint8_t> limit1(numNotes, 0); //0: Unmodified, 1: Modified
  uint8_t limit2 = 0; //0: Unmodified, 1: Up, 2: Down
  uint8_t limit3 = 0; //0: Unmodified, 1: Modified
  uint8_t limit4 = 0; //0: Unmodified, 1: Up, 2: Down
  std::vector<uint8_t> limit5(numNotes, 0); //0: Unmodified, 1: Long, 2: Short

  //Set up RNG
  std::uniform_int_distribution<uint8_t> distMut(0,0);
  std::uniform_int_distribution<uint8_t> distBool(0,1);
  std::uniform_int_distribution<uint32_t> distNote(0,numNotes-1);

  //Repeatedly apply mutations until mutation points are depleted or
  //a certain number of tries is exceeded without success
  const uint8_t MAX_FAILURES = 20;
  uint8_t currentFailures = 0;
  uint32_t pointsSpent = 0;
  uint32_t note;
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

  int8_t diffNote = 0;
  if (set.forceStartNote)
    {
      std::normal_distribution<float> distNormNote(0, 2);
      diffNote = set.startNote - abstr.note(0).note + distNormNote(*(set.gen));
    }

  //Convert all of the abstract notes to concrete notes
  notes_.clear();
  for (size_t i = 0; i < numNotes; i++)
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
void ConcreteMotif::addToTrack(midi::NoteTrack& nt, uint32_t begin)
{
  for (size_t i = 0; i < notes_.size(); i++)
    {
      midi::NoteTime note = notes_[i];
      note.begin += begin;
      nt.add(note);
    }
}

//Returns the length of the motif in ticks
uint32_t ConcreteMotif::ticks() const
{
  uint32_t longest = 0;

  for (size_t i = 0; i < notes_.size(); i++)
    {
      if (notes_[i].begin + notes_[i].duration > longest)
        {
          longest = notes_[i].begin + notes_[i].duration;
        }
    }

  //Strictness 1: Just return the length
  /*if (set_.strictness == 1)
    {
      return longest;
    }
  else //Strictness 2-5: Pad to quarter note
    {
      uint8_t quarterNotes = ((longest)/(set_.ticksPerQuarter)) + 1;
      return quarterNotes*set_.ticksPerQuarter;
      }*/
  return longest;
}

#endif
