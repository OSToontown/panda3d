// Filename: ppInstance.h
// Created by:  drose (19Jun09)
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

#ifndef PPINSTANCE_H
#define PPINSTANCE_H

#include "nppanda3d_common.h"
#include "fileSpec.h"
#include "get_tinyxml.h"

#include <vector>

class PPObject;

////////////////////////////////////////////////////////////////////
//       Class : PPInstance
// Description : This represents a single instance of the Panda3D
//               plugin, via the NPAPI interface.  This instance
//               brokers the communication with the P3D Core API, as
//               defined in the plugin directory.
////////////////////////////////////////////////////////////////////
class PPInstance {
public:
  PPInstance(NPMIMEType pluginType, NPP instance, uint16 mode, 
             int16 argc, char *argn[], char *argv[], NPSavedData *saved);
  ~PPInstance();

  inline NPP get_npp_instance() const;

  void set_window(NPWindow *window);
  NPError new_stream(NPMIMEType type, NPStream *stream, 
                     bool seekable, uint16 *stype);
  int write_stream(NPStream *stream, int offset, int len, void *buffer);
  NPError destroy_stream(NPStream *stream, NPReason reason);
  void url_notify(const char *url, NPReason reason, void *notifyData);
  void stream_as_file(NPStream *stream, const char *fname);

  void handle_request(P3D_request *request);

  NPObject *get_script_object();

private:
  bool read_contents_file(const string &filename);
  void get_core_api(TiXmlElement *xpackage);
  void downloaded_plugin(const string &filename);
  void do_load_plugin();

  void create_instance();
  void send_window();

  void show_np_variant(const NPVariant &result);
  P3D_object *np_variant_to_object(const NPVariant &result);

private:
  NPP _npp_instance;
  unsigned int _npp_mode;
  typedef vector<P3D_token> Tokens;
  Tokens _tokens;

  string _root_dir;
  FileSpec _core_api_dll;

  bool _started_instance_data;
  bool _got_instance_data;
  string _p3d_filename;

  bool _got_window;
  NPWindow _window;

  PPObject *_script_object;

  P3D_instance *_p3d_inst;
};

#include "ppInstance.I"

#endif