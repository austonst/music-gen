/*
  Copyright (c) 2014 Auston Sterling
  See LICENSE for copying permissions.

  -----Theme Test Program-----
  Auston Sterling
  austonst@gmail.com

  A program to test the generation of a theme: analagous to a sentence in speech.
  A theme should have some self-repetition and a bit of structure.
*/

#include "theme.hpp"

#include <chrono>

int main()
{
  std::mt19937 gen;
  gen.seed(std::chrono::system_clock::now().time_since_epoch().count());

  //Settings
  MotifGenSettings set1(1.5, &gen, 3);

  //Create some global abstract motifs
  std::vector<AbstractMotif> am;
  am.push_back(AbstractMotif(set1));
  am.push_back(AbstractMotif(set1));
  am.push_back(AbstractMotif(set1));
  set1.length = 1;
  am.push_back(AbstractMotif(set1));
  am.push_back(AbstractMotif(set1));
  am.push_back(AbstractMotif(set1));

  //Abstract themes
  ThemeGenSettings set2(3, am, .25, &gen, 3);
  AbstractTheme at1(set2);
  set2.concreteness = .5;
  AbstractTheme at2(set2);
  set2.concreteness = .75;
  AbstractTheme at3(set2);
  set2.concreteness = 1;
  AbstractTheme at4(set2);
  AbstractTheme at5(set2);

  //Concrete themes
  ThemeConcreteSettings set3("C4", 0, 80, midi::INST_ACOUSTIC_GRAND_PIANO,
                             1500, &gen, 3);

  midi::NoteTrack nt;
  uint32_t ticks = 0;

  ConcreteTheme ct(at1, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  ct.generate(at1, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  ct.generate(at1, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  ct.generate(at4, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  ct.generate(at4, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  ct.generate(at4, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  ct.generate(at4, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //1 1 2 2 3 3 1 1 4 4 5 5 1 1
  //1
  /*
  ConcreteTheme ct(at1, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //1
  ct.generate(at1, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //2
  ct.generate(at2, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //2
  ct.generate(at2, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //3
  ct.generate(at3, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //3
  ct.generate(at3, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //1
  ct.generate(at1, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //1
  ct.generate(at1, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //4
  ct.generate(at4, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //4
  ct.generate(at4, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //5
  ct.generate(at5, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //5
  ct.generate(at5, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //1
  ct.generate(at1, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;

  //1
  ct.generate(at1, set3);
  ct.addToTrack(nt, ticks);
  ticks += ct.ticks() + 5000;*/

  //Save
  midi::MIDI_Type0 mid(nt, midi::TimeDivision(1548));
  mid.write("testtheme.mid");
}
