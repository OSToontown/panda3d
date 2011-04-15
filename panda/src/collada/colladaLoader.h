// Filename: colladaLoader.h
// Created by: Xidram (21Dec10)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#ifndef COLLADALOADER_H
#define COLLADALOADER_H

#include "pandabase.h"
#include "config_collada.h"
#include "typedReferenceCount.h"
#include "pandaNode.h"
#include "modelRoot.h"
#include "pvector.h"
#include "pta_LVecBase4f.h"

class BamCacheRecord;
class LightNode;

class domCOLLADA;
class domNode;
class domVisual_scene;
class domExtra;
class domGeometry;
class domLight;
class domCamera;
class domSource;
class DAE;

////////////////////////////////////////////////////////////////////
//       Class : ColladaLoader
// Description : Object that interfaces with the COLLADA DOM library
//               and loads the COLLADA structures into Panda nodes.
////////////////////////////////////////////////////////////////////
class ColladaLoader {
public:
  ColladaLoader();
  virtual ~ColladaLoader();

  bool _error;
  PT(ModelRoot) _root;
  BamCacheRecord *_record;
  CoordinateSystem _cs;
  Filename _filename;

  bool read(const Filename &filename);
  void build_graph();

private:
  const domCOLLADA* _collada;
  DAE* _dae;
  pvector<LightNode*> _lights;

  void load_visual_scene(domVisual_scene &scene, PandaNode *parent);
  void load_node(domNode &node, PandaNode *parent);
  void load_tags(domExtra &extra, PandaNode *node);
  void load_camera(domCamera &cam, PandaNode *parent);
  void load_geometry(domGeometry &geom, PandaNode *parent);
  CPT(InternalName) load_input(GeomVertexArrayFormat *fmt, PTA_LVecBase4f &values, const string &semantic, domSource &src, unsigned int set=0);
  void load_light(domLight &light, PandaNode *parent);
};

#include "colladaLoader.I"

#endif