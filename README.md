xoreos-tools
============

A small repository containing tools, docs, odds and ends to help with [xoreos](https://github.com/DrMcCoy/xoreos) development.

Currently included are:

oldlibeos
---------

This is an old library for reading/writing NWN files that I worked on between 2004 and 2005. The plan was to expand it into a full-fledged portable toolset, but it never got that far.

Readers beware: The code is quite ugly. I only release it because the tools provided by the test application, especially the GFF to XML converter, still are useful. They should probably be replaced by proper clean tools in the future.

docs
----

At the moment, this directory contains file format specifications, both official and as figured out by the modding community.

* bioware: The official BioWare NWN docs, from the now defunct
  nwn.bioware.com site
* torlack: Tim Smith (Torlack)'s notes of several formats, from his
  now defunct website
* gmax_nwn_mdl_0.3b.ms: GMax import script for NWN models, by
  Wayland Reid
* kotor_mdl.html: Partial KotOR model specs
* nds_sdat.html: Partial Nintendo DS Nitro Composer File (*.sdat)
  specs, by kiwi.ds
