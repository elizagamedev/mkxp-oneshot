#!/usr/bin/ruby
#encoding: utf-8
require 'fileutils'

# These libraries MUST NOT be packaged with OneShot
# or otherwise they will cause cross-distro
# incompatibilities.
BLACKLIST = [
  # These packaged on Manjaro cause issues on openSUSE.
  'libc.so.6',
  'libpthread.so.0',
  'libdl.so.2',
  'libm.so.6'
]

line = gets
files = []
while line
  if line =~ / => (\/.*) \(/
    if not BLACKLIST.any? {|library| $1.end_with? library }
      files << $1
    end
  end
  line = gets
end
FileUtils.cp(files, 'libs')
