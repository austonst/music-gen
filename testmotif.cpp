/*
  Copyright (c) 2014 Auston Sterling
  See LICENSE for copying permissions.

  -----Motif Test Program-----
  Auston Sterling
  austonst@gmail.com

  A program to test the random generation of a motif, and the concrete
  instantiations of that motif with increasing levels of variance.
*/

#include "motif.hpp"

#include <chrono>

int main()
{
  std::mt19937 gen;
  gen.seed(std::chrono::system_clock::now().time_since_epoch().count());
  
  //Settings
  MotifGenSettings set1;
  set1.strictness = 2;
  set1.length = 2;
  set1.gen = &gen;
  MotifConcreteSettings set2;
  set2.strictness = 2;
  set2.key = "C4";
  set2.keyType = 0;
  set2.mutations = 0;
  set2.instrument = 0;
  set2.ticksPerQuarter = 1000;
  set2.gen = &gen;

  //Abstract
  AbstractMotif am(set1);

  //Concrete 1
  ConcreteMotif cm1(am, set2);

  //Concrete 2
  set2.mutations = 20;
  ConcreteMotif cm2(am, set2);

  //Concrete 3
  set2.mutations = 40;
  ConcreteMotif cm3(am, set2);

  //Concrete 4
  set2.mutations = 60;
  ConcreteMotif cm4(am, set2);

  //Concrete 5
  set2.mutations = 80;
  ConcreteMotif cm5(am, set2);

  //Combine
  uint32_t totalTicks = 0;
  midi::NoteTrack nt;
  cm1.addToTrack(nt, totalTicks);
  totalTicks += cm1.ticks() + 2000;
  
  cm2.addToTrack(nt, totalTicks);
  totalTicks += cm2.ticks() + 2000;

  cm3.addToTrack(nt, totalTicks);
  totalTicks += cm3.ticks() + 2000;

  cm4.addToTrack(nt, totalTicks);
  totalTicks += cm4.ticks() + 2000;

  cm5.addToTrack(nt, totalTicks);
  totalTicks += cm5.ticks() + 2000;

  //Save
  midi::MIDI_Type0 mid(nt, midi::TimeDivision(1548));
  mid.write("testmotif.mid");
}
