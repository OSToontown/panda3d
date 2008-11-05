// Filename: virtualFileMountHTTP.cxx
// Created by:  drose (30Oct08)
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

#include "virtualFileMountHTTP.h"
#include "virtualFileHTTP.h"
#include "virtualFileSystem.h"

#ifdef HAVE_OPENSSL

TypeHandle VirtualFileMountHTTP::_type_handle;


////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::Constructor
//       Access: Published
//  Description: 
////////////////////////////////////////////////////////////////////
VirtualFileMountHTTP::
VirtualFileMountHTTP(const URLSpec &root, HTTPClient *http) :
  _http(http),
  _root(root)
{
  // Make sure the root ends on a slash.  The implicit trailing slash
  // is a semi-standard internet convention.
  string path = _root.get_path();
  if (!path.empty() && path[path.length() - 1] != '/') {
    path += '/';
    _root.set_path(path);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::Destructor
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
VirtualFileMountHTTP::
~VirtualFileMountHTTP() {
}


////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::has_file
//       Access: Public, Virtual
//  Description: Returns true if the indicated file exists within the
//               mount system.
////////////////////////////////////////////////////////////////////
bool VirtualFileMountHTTP::
has_file(const Filename &) const {
  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::is_directory
//       Access: Public, Virtual
//  Description: Returns true if the indicated file exists within the
//               mount system and is a directory.
////////////////////////////////////////////////////////////////////
bool VirtualFileMountHTTP::
is_directory(const Filename &) const {
  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::is_regular_file
//       Access: Public, Virtual
//  Description: Returns true if the indicated file exists within the
//               mount system and is a regular file.
////////////////////////////////////////////////////////////////////
bool VirtualFileMountHTTP::
is_regular_file(const Filename &) const {
  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::make_virtual_file
//       Access: Public, Virtual
//  Description: Constructs and returns a new VirtualFile instance
//               that corresponds to the indicated filename within
//               this mount point.  The returned VirtualFile object
//               does not imply that the given file actually exists;
//               but if the file does exist, then the handle can be
//               used to read it.
////////////////////////////////////////////////////////////////////
PT(VirtualFile) VirtualFileMountHTTP::
make_virtual_file(const string &local_filename,
                  const Filename &original_filename, bool implicit_pz_file,
                  bool status_only) {
  PT(VirtualFileHTTP) vfile = 
    new VirtualFileHTTP(this, local_filename, implicit_pz_file, status_only);
  vfile->set_original_filename(original_filename);

  return vfile.p();
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::open_read_file
//       Access: Public, Virtual
//  Description: Opens the file for reading, if it exists.  Returns a
//               newly allocated istream on success (which you should
//               eventually delete when you are done reading).
//               Returns NULL on failure.
////////////////////////////////////////////////////////////////////
istream *VirtualFileMountHTTP::
open_read_file(const Filename &) const {
  return NULL;
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::get_file_size
//       Access: Published, Virtual
//  Description: Returns the current size on disk (or wherever it is)
//               of the already-open file.  Pass in the stream that
//               was returned by open_read_file(); some
//               implementations may require this stream to determine
//               the size.
////////////////////////////////////////////////////////////////////
off_t VirtualFileMountHTTP::
get_file_size(const Filename &, istream *) const {
  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::get_file_size
//       Access: Published, Virtual
//  Description: Returns the current size on disk (or wherever it is)
//               of the file before it has been opened.
////////////////////////////////////////////////////////////////////
off_t VirtualFileMountHTTP::
get_file_size(const Filename &) const {
  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::get_timestamp
//       Access: Published, Virtual
//  Description: Returns a time_t value that represents the time the
//               file was last modified, to within whatever precision
//               the operating system records this information (on a
//               Windows95 system, for instance, this may only be
//               accurate to within 2 seconds).
//
//               If the timestamp cannot be determined, either because
//               it is not supported by the operating system or
//               because there is some error (such as file not found),
//               returns 0.
////////////////////////////////////////////////////////////////////
time_t VirtualFileMountHTTP::
get_timestamp(const Filename &) const {
  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::scan_directory
//       Access: Public, Virtual
//  Description: Fills the given vector up with the list of filenames
//               that are local to this directory, if the filename is
//               a directory.  Returns true if successful, or false if
//               the file is not a directory or cannot be read.
////////////////////////////////////////////////////////////////////
bool VirtualFileMountHTTP::
scan_directory(vector_string &, const Filename &) const {
  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::output
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void VirtualFileMountHTTP::
output(ostream &out) const {
  out << _root;
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::get_channel
//       Access: Public
//  Description: Returns an HTTPChannel object suitable for use for
//               extracting a document from the current URL root.
////////////////////////////////////////////////////////////////////
PT(HTTPChannel) VirtualFileMountHTTP::
get_channel() {
  PT(HTTPChannel) channel;
  _channels_lock.acquire();

  if (!_channels.empty()) {
    // If we have some channels sitting around, grab one.  Grab the
    // one on the end; it was most recently pushed, and therefore most
    // likely to be still alive.
    channel = _channels.back();
    _channels.pop_back();
  } else {
    // If we don't have any channels standing by, make a new one.
    channel = _http->make_channel(true);
  }

  _channels_lock.release();
  return channel;
}

////////////////////////////////////////////////////////////////////
//     Function: VirtualFileMountHTTP::recycle_channel
//       Access: Public
//  Description: Accepts an HTTPChannel that is no longer being used,
//               and restores it to standby duty, so that it will be
//               returned by a future call to get_channel().
////////////////////////////////////////////////////////////////////
void VirtualFileMountHTTP::
recycle_channel(HTTPChannel *channel) {
  _channels_lock.acquire();
  _channels.push_back(channel);
  _channels_lock.release();
}

#endif  // HAVE_OPENSSL