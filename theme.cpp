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

//Default constructor, sets to minimum strictness
ThemeGenSettings::ThemeGenSettings() :
  length(0),
  concreteness(1),
  gen(nullptr)
{
  setStrictness(1);
}

//Constructor to set up required fields and optionally strictness
//If no strictness specified, sets to minimum
ThemeGenSettings::ThemeGenSettings(float inLength, std::vector<AbstractMotif>& inMotifs,
                                   float inConc, std::mt19937* inGen, uint8_t strict) :
  length(inLength),
  motifs(inMotifs),
  concreteness(inConc),
  gen(inGen)
{
  setStrictness(strict);
}

void ThemeGenSettings::setStrictness(uint8_t strict)
{
  strictness = strict;
  
  if (strictness <= 1)
    {
      nonIntMotifs = true;
      extraRepeatWeight = false;
      decayRepeatWeight = false;
    }
  else if (strictness == 2)
    {
      nonIntMotifs = true;
      extraRepeatWeight = false;
      decayRepeatWeight = false;
    }
  else if (strictness == 3)
    {
      nonIntMotifs = false;
      extraRepeatWeight = true;
      decayRepeatWeight = false;
    }
  else if (strictness == 4)
    {
      nonIntMotifs = false;
      extraRepeatWeight = true;
      decayRepeatWeight = true;
    }
  else // >= 5
    {
      nonIntMotifs = false;
      extraRepeatWeight = true;
      decayRepeatWeight = true;
    }
}

//Default constructor, sets to minimum strictness
ThemeConcreteSettings::ThemeConcreteSettings() :
  key(0),
  keyType(0),
  maxMutations(0),
  instrument(midi::INST_ACOUSTIC_GRAND_PIANO),
  ticksPerQuarter(1500), //No justification for this
  gen(nullptr)
{
  setStrictness(1);
}

//Constructor to set up required fields and optionally strictness
//If no strictness specified, sets to minimum
ThemeConcreteSettings::ThemeConcreteSettings(midi::Note inKey, uint8_t inType,
                                             uint32_t inMut, midi::Instrument inInst,
                                             uint32_t inTPQ, std::mt19937* inGen,
                                             uint8_t strict) :
  key(inKey),
  keyType(inType),
  maxMutations(inMut),
  instrument(inInst),
  ticksPerQuarter(inTPQ),
  gen(inGen)
{
  setStrictness(strict);
}

void ThemeConcreteSettings::setStrictness(uint8_t strict)
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

//Standard constructor, generates a new AbstractTheme
AbstractTheme::AbstractTheme(const ThemeGenSettings& set)
{
  generate(set);
}

//Generates an AbstractTheme from the passed settings
void AbstractTheme::generate(const ThemeGenSettings& set)
{
  //Create some more motifs to be used here only
  MotifGenSettings mgs1(1, set.gen, set.strictness);
  MotifGenSettings mgs15(1.5, set.gen, set.strictness);
  MotifGenSettings mgs2(2, set.gen, set.strictness);

  std::uniform_int_distribution<uint8_t> distTimeSig(0,2);
  uint8_t timesig = distTimeSig(*(set.gen));
  if (timesig == 0) //3 beats per measure
    {
      mgs1.length *= 3./4.;
      mgs15.length *= 3./4.;
      mgs2.length *= 3./4.;
    }
  else if (timesig == 1) //4 beats per measure
    {
      //Already set to this length!
    }
  else //5 beats per measure
    {
      mgs1.length *= 5./4.;
      mgs15.length *= 5./4.;
      mgs2.length *= 5./4.;
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
      //If nonIntMotifs set, allow for motifs of non-measure length
      else if (rand == 1 && set.nonIntMotifs)
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
      
      //If extraRepeatWeight false, choose motif at random
      if (!set.extraRepeatWeight || length == 0)
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
      //if decayRepeatWeight false, have a flatly increased chance of repeating
      //the previous motif
      else if (!set.decayRepeatWeight) 
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
      //If decayRepeatweight true, further increased chance of repetition,
      //but falls off
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
ConcreteTheme::ConcreteTheme(const AbstractTheme abstr,
                             const ThemeConcreteSettings& set)
{
  generate(abstr, set);
}

//Create an instantiation of an AbstractTheme using the passed settings
void ConcreteTheme::generate(const AbstractTheme abstr, ThemeConcreteSettings set)
{
  //Pass down most of the settings directly
  MotifConcreteSettings motifSet(set.key, set.keyType, 0, set.instrument,
                                 set.ticksPerQuarter, false, 0, set.gen,
                                 set.strictness);

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
