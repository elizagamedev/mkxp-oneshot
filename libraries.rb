#!/usr/bin/ruby
#encoding: utf-8
require 'fileutils'

line = gets
files = []
while line
  if line =~ / => (\/.*) \(/
    files << $1
  end
  line = gets
end
FileUtils.cp(files, 'libs')
