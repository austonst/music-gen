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
  //Default constructor, sets to minimum strictness
  ThemeGenSettings();

  //Constructor to set up required fields and optionally strictness
  //If no strictness specified, sets to minimum
  ThemeGenSettings(float inLength, std::vector<AbstractMotif>& inMotifs,
                   float inConc, std::mt19937* inGen, std::uint8_t strict = 1);
  
  //Sets up values corresponding to a certain strictness
  //Does not properly set length, motifs, concreteness or gen!
  void setStrictness(std::uint8_t strict);

  //--- Strictness Independent Variables ---
  //The approximate length of the theme in whole notes
  float length;

  //Abstract motifs which are reused throughout the piece and can be used here
  std::vector<AbstractMotif> motifs;

  //The concreteness of the theme from 0 (no mutations) to 1 (full mutations)
  //Low concreteness makes themes good for choruses and stuff
  //High concreteness makes for random sounding music that still follows common motifs
  float concreteness;

  //A pointer to a random number generator to be used in generation
  std::mt19937* gen;

  //--- Strictness Dependent Variables ---
  //The strictness of the theme
  std::uint8_t strictness;

  //If true, motif length can be a non-integer measure length
  bool nonIntMotifs;

  //If true, have an increased chance of repeating the previous motif
  bool extraRepeatWeight;
  
  //If true and weightMotifRepeat true, have extra weight decay with repetition
  bool decayRepeatWeight;
};

//Helper struct for ConcreteTheme generation from an AbstractTheme
struct ThemeConcreteSettings
{
  //Default constructor, sets to minimum strictness
  ThemeConcreteSettings();

  //Constructor to set up required fields and optionally strictness
  //If no strictness specified, sets to minimum
  ThemeConcreteSettings(midi::Note inKey, std::uint8_t inType, std::uint32_t inMut,
                        midi::Instrument inInst, std::uint32_t inTPQ,
                        std::mt19937* inGen, std::uint8_t strict = 1);
  
  //Sets up values corresponding to a certain strictness
  //Currently has no effect
  void setStrictness(std::uint8_t strict);

  //--- Strictness Independent Variables ---
  //The base key of the concrete theme
  midi::Note key;

  //The main type of key (0=major, 1=harmonic 2=natural minor)
  std::uint8_t keyType;

  //The maximum number of average mutation points per motif
  std::uint32_t maxMutations;

  //The instrument that will play this theme
  midi::Instrument instrument;

  //The conversion between abstract and concrete time
  std::uint32_t ticksPerQuarter;

  //A pointer to a Mersenne Twister to be used in generation
  std::mt19937* gen;

  //--- Strictness Dependent Variables ---
  //The strictness of the theme
  std::uint8_t strictness;
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
  std::size_t numMotifs() const {return motifs_.size();}
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
  void addToTrack(midi::NoteTrack& nt, std::uint32_t begin);
  std::uint32_t ticks() const;

 private:
  //The concrete motifs, ready to be played!
  std::vector<ConcreteMotif> motifs_;
};

#endif
