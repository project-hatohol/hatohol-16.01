#!/usr/bin/env python

import sys

class HatoholActionCreator:
  def __init__(self, url):
    self.url = url

  def add(self, options):
    type = self.get_one_arg(options, "--type")
    if type != "command" and type != "resident":
      print "--type's value must be 'command' or 'resident'."
      sys.exit(-1)

    command = self.get_one_arg(options, "--command")
    working_dir = self.get_one_arg(options, "--workind-dir", True);
    timeout = self.get_one_arg(options, "--timeout", True);
    server_id = self.get_one_arg(options, "--server-id", True);
    host_id = self.get_one_arg(options, "--host-id", True);
    host_group_id = self.get_one_arg(options, "--host-group-id", True);
    trigger_id = self.get_one_arg(options, "--trigger-id", True);
    status = self.get_one_arg(options, "--status", True);
    severity = self.get_one_arg(options, "--severity", True);
    severity_cmp = 1

    print "Type       : " + type
    print "Commad     : " + command
    if timeout:
      print "Timeout    : " + timeout
    if server_id:
      print "Server ID  : " + server_id
    if host_id:
      print "Host ID    : " + host_id
    if host_group_id:
      print "Host Grp ID: " + host_group_id
    if trigger_id:
      print "Trigger ID : " + tigger_id
    if status:
      print "Trig. Stat.: " + status
    if severity:
      print "Seveirty   : " + severity

  def get_one_arg(self, options, key, allow_none = False):
    if key not in options:
      if allow_none:
        return None
      print key + " is not defined."
      sys.exit(-1)
    idx = options.index(key)
    if idx == len(options) - 1:
      print key + ": must have the argument."
      sys.exit(-1)
    arg = options[idx+1]
    del options[idx:idx+2]
    return arg

