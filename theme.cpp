/*
  Copyright (c) 2014 Auston Sterling
  See LICENSE for copying permissions.
  
  -----Theme Class Implementation-----
  Auston Sterling
  austonst@gmail.com

  The implementation of the Theme class, representing a short series of motifs.
  This would be analagous to a sentence.
*/

#include "theme.hpp"

//Standard constructor, generates a new AbstractTheme
AbstractTheme::AbstractTheme(const ThemeGenSettings& set)
{
  generate(set);
}

//Generates an AbstractTheme from the passed settings
void AbstractTheme::generate(const ThemeGenSettings& set)
{
  //Create some more motifs to be used here only
  MotifGenSettings mgs1;
  MotifGenSettings mgs15;
  MotifGenSettings mgs2;

  std::uniform_int_distribution<uint8_t> distTimeSig(0,2);
  uint8_t timesig = distTimeSig(*(set.gen));
  if (timesig == 0) //3 beats per measure
    {
      mgs1 =  {set.strictness, .75,   set.gen};
      mgs15 = {set.strictness, 1.125, set.gen};
      mgs2 =  {set.strictness, 1.5,   set.gen};
    }
  else if (timesig == 1) //4 beats per measure
    {
      mgs1 =  {set.strictness, 1,   set.gen};
      mgs15 = {set.strictness, 1.5, set.gen};
      mgs2 =  {set.strictness, 2,   set.gen};
    }
  else //5 beats per measure
    {
      mgs1 =  {set.strictness, 1.25,   set.gen};
      mgs15 = {set.strictness, 1.825, set.gen};
      mgs2 =  {set.strictness, 2.5,   set.gen};
    }

  //Even amounts of local and global motifs
  std::vector<AbstractMotif> localMotifs;
  std::uniform_int_distribution<uint8_t> distLen(0,2);
  for (size_t i = 0; i < set.motifs.size(); i++)
    {
      uint8_t rand = distLen(*(set.gen));
      if (rand == 0)
        {
          localMotifs.push_back(AbstractMotif(mgs1));
        }
      //Strictness 1-2: Allow for motifs of non-measure length
      else if (rand == 1 && set.strictness <= 2)
        {
          localMotifs.push_back(AbstractMotif(mgs15));
        }
      else
        {
          localMotifs.push_back(AbstractMotif(mgs2));
        }
    }

  //Fill the theme with motifs
  float length = 0;
  motifs_.clear();
  std::uniform_int_distribution<uint16_t> distMotif(0,2*localMotifs.size()-1);
  AbstractMotif prevMotif;
  uint8_t repeatCount = 0;
  while (length < set.length)
    {
      //Select motif
      AbstractMotif select;
      
      //Strictness 1-2: Choose motif at random
      if (set.strictness <= 2 || length == 0)
        {
          uint16_t rand = distMotif(*(set.gen));
          if (rand > localMotifs.size()-1)
            {
              select = localMotifs[rand-localMotifs.size()];
            }
          else
            {
              select = set.motifs[rand];
            }
        }
      //Strictness 3: Have an increased chance of repeating the previous motif
      else if (set.strictness == 3) 
        {
          std::uniform_real_distribution<float> prob(0,1);
          if (prob(*(set.gen)) < 0.3)
            {
              select = prevMotif;
            }
          else
            {
              uint16_t rand = distMotif(*(set.gen));
              if (rand > localMotifs.size()-1)
                {
                  select = localMotifs[rand-localMotifs.size()];
                }
              else
                {
                  select = set.motifs[rand];
                }
            }
        }
      //Strictness 4-5: Further increased chance of repetition, but falls off
      else
        {
          std::uniform_real_distribution<float> prob(0,1);
          if (prob(*(set.gen)) < 0.6 - float(repeatCount)*.2)
            {
              select = prevMotif;
              repeatCount++;
            }
          else
            {
              uint16_t rand = distMotif(*(set.gen));
              if (rand > localMotifs.size()-1)
                {
                  select = localMotifs[rand-localMotifs.size()];
                }
              else
                {
                  select = set.motifs[rand];
                }
              repeatCount = 0;
            }
        }

      //Add it
      motifs_.push_back(select);
      length += select.length();
      prevMotif = select;
    }

  //Copy over concreteness
  concrete_ = set.concreteness;
}

//Generate a new concrete theme as part of the constructor
ConcreteTheme::ConcreteTheme(const AbstractTheme abstr, const ThemeConcreteSettings& set)
{
  generate(abstr, set);
}

//Create an instantiation of an AbstractTheme using the passed settings
void ConcreteTheme::generate(const AbstractTheme abstr, ThemeConcreteSettings set)
{
  //Pass down most of the settings directly
  MotifConcreteSettings motifSet;
  motifSet.strictness = set.strictness;
  motifSet.key = set.key;
  motifSet.keyType = set.keyType;
  motifSet.instrument = set.instrument;
  motifSet.gen = set.gen;
  motifSet.ticksPerQuarter = set.ticksPerQuarter;
  motifSet.forceStartNote = false;

  //Mutations are dependent on concreteness of AbstractTheme
  set.maxMutations *= abstr.concrete();

  //Have some variability in mutation amounts
  std::normal_distribution<float> distMut(set.maxMutations, 10);

  //Concretize each AbstractMotif
  motifs_.clear();
  for (size_t i = 0; i < abstr.numMotifs(); i++)
    {
      float rand;
      do {rand = distMut(*(set.gen));} while (rand < 0);
      motifSet.mutations = rand + 0.5;
      if (i > 0)
        {
          motifSet.forceStartNote = true;
          motifSet.startNote = abstr.motif(i-1).note(abstr.motif(i-1).numNotes()-1).note;
        }

      motifs_.push_back(ConcreteMotif(abstr.motif(i), motifSet));
    }
}

//Adds this theme to a NoteTrack
void ConcreteTheme::addToTrack(midi::NoteTrack& nt, uint32_t begin)
{
  uint32_t offset = 0;
  for (size_t i = 0; i < motifs_.size(); i++)
    {
      motifs_[i].addToTrack(nt, begin+offset);
      offset += motifs_[i].ticks();
    }
}

//Return the total number of ticks in this theme
uint32_t ConcreteTheme::ticks() const
{
  uint32_t count = 0;
  for (size_t i = 0; i < motifs_.size(); i++)
    {
      count += motifs_[i].ticks();
    }
  return count;
}
