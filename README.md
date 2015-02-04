#Procedural Music Generation

This set of C++ classes, and the programs that use them, are components of an attempt at procedural music generation. The goal of this project is to attempt to determine the span of what we consider to be "music". Most people can easily identify a series of random notes as "not music", but on the other hand, repeating musical foundations such as scales and chords doesn't really sound like "music" either. As an example of one aspect to consider: we expect that our music will have some amount of repetition. We usually expect repetition in a repeated chorus as well as in shorter collections of notes which are often modified greatly, but retain their central structure to cause some recognition.

##Components
This project's approach to the problem of creating aesthetically pleasing music from scratch is to view a musical piece as almost a hierarchical structure of repetition.

###Motif
At the most basic level, a motif is an abstract concept of a few notes which are played together throughout the piece. A single motif can show up all over the place, played at different speeds in different keys. Take, for instance, the classic example of the first four notes of Beethoven's Fifth Symphony. Three notes followed by a lower pitched, longer held note. Even in the first 15 seconds of the symphony, the motif is played at a higher tempo in different keys, and the variant where the third note is slightly lowered to be between the pitches of the second and fourth is introduced. It's so common in Beethoven's work to see a simple motif introduced, then gradually mutated until it is barely recognizable, and later mixed in with other motifs.

###Theme
If motifs are words, said over the course of a measure or two, a theme is a sentence built out these motifs. In Beethoven's Fifth, the three quickly played instances of the previously mentioned motif in increasing pitch, followed by a short pause, makes up the first repeated and recognizable theme. You can even sing along to it comfortably in one breath. Like motifs, themes are varied over the course of a piece (Beethoven plays the first theme with pitch decreasing between motifs very early on), but tend to be a bit more restrained in general.

###Piece
We can then say a piece (or maybe, song) is a combination of themes. The themes which barely change make up the chorus. The abstract, mutable themes make up the rest of the piece, but are still based off of the same motifs as everything else. This is the structure this project uses, and while it is far from great at producing music, it is great at asking questions about why music is music.

##Building
This depends on my midi library, which can be found at https://github.com/austonst/midi . I've only tested it using g++ 4.7-4.8 under 32 bit Linux, though I think it should be entirely cross-platform.

The build system for this library is still very primitive (hand-written Linux Makefile). Also, this requires that:

* The headers to my midi library be either in a "midi" subdirectory, or be in a midi folder in some system location such as /usr/include/midi.
* The static library produced by the midi library (libmidi.a) needs to be placed alongside these source files.

Sorry about the complexity there; I'll change it as soon as I update this to CMake. Currently, assuming you have *make*, *g++*, and the midi library properly set up, you should be able to build with a simple call to *make*.

This will produce three executables.

* testmotif will generate a random motif and play it back repeatedly with increasing amounts of variance. Ideally, it should start to sound less and less like the first motif played, but still be somewhat recognizable.
* testtheme will generate multiple themes which share some global motifs, then play back multiple variations on each theme.
* testpiece demonstrates full piece generation. Sometimes it gets lucky and turns out okay. Most of the time, it does not.

##To-do
There's really a lot of directions this could be taken. Here's a few ideas:

* Motifs are currently created by choosing random notes near the last played note for random durations. Would motifs sound better if they started off as one note for the whole duration, then went through a series of splits and perturbations? Maybe generate a random Fourier series and sample it to get a motif's notes?
* There is currently no tempo variance. How should that be accomplished without making listeners lose track of the beat?
* Chord progressions are like motifs of their own. They shouldn't be completely random, but what sort of structure makes sense, and how do we avoid hard-coding in a limited amount of progressions which fails to capture the full range of music?
* This only generates melodies. What about everything else? Harmonies? Accompanying chords? Introductions with some music before the real melody starts?
* I could go on and on, but that's enough for now.

This is all under the MIT license, so feel free to play around with any components, or contribute to the project yourself.