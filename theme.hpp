/*
  -----Theme Class Header-----
  Auston Sterling
  austonst@gmail.com

  The header for the Theme class, representing a short series of motifs.
  This would be analagous to a sentence.
*/

#ifndef _theme_h_
#define _theme_h_

#include "motif.hpp"

//Helper struct for AbstractTheme generation
struct ThemeGenSettings
{
  //The strictness of the theme
  uint8_t strictness;
  
  //The approximate length of the theme in whole notes
  float length;

  //Abstract motifs which are reused throughout the piece and can be used here
  std::vector<AbstractMotif> motifs;

  //A pointer to a random number generator to be used in generation
  std::mt19937* gen;

  //The concreteness of the theme from 0 (no mutations) to 1 (full mutations)
  //Low concreteness makes themes good for choruses and stuff
  //High concreteness makes for random sounding music that still follows common motifs
  float concreteness;
};

//Helper struct for ConcreteTheme generation from an AbstractTheme
struct ThemeConcreteSettings
{
  //The strictness of the theme
  uint8_t strictness;
  
  //The base key of the concrete theme
  midi::Note key;

  //The main type of key (0=major, 1=harmonic 2=natural minor)
  uint8_t keyType;

  //The maximum number of average mutation points per motif
  uint32_t maxMutations;

  //The intrument that will play this theme
  midi::Instrument instrument;

  //The conversion between abstract and concrete time
  uint32_t ticksPerQuarter;

  //A pointer to a Mersenne Twister to be used in generation
  std::mt19937* gen;
};

//An abstract theme, which contains the main information about a theme but lacks
//some details
class AbstractTheme
{
 public:
  //Constructors
  AbstractTheme(const ThemeGenSettings& set);

  //General use functions
  void generate(const ThemeGenSettings& set);

  //Accessors
  size_t numMotifs() const {return motifs_.size();}
  AbstractMotif motif(int i) const {return motifs_[i];}
  float concrete() const {return concrete_;}

 private:
  //Ordered motifs
  std::vector<AbstractMotif> motifs_;

  //The concreteness of the theme
  float concrete_;
};

//A concrete theme, which contains complete information about a theme
class ConcreteTheme
{
 public:
  //Constructors
  ConcreteTheme(const AbstractTheme abstr, const ThemeConcreteSettings& set);

  //General use functions
  void generate(const AbstractTheme abstr, ThemeConcreteSettings set);
  void addToTrack(midi::NoteTrack& nt, uint32_t begin);
  uint32_t ticks() const;

 private:
  //The concrete motifs, ready to be played!
  std::vector<ConcreteMotif> motifs_;
};

#endif
