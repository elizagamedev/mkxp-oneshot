#!/usr/bin/ruby
#encoding: utf-8

require 'get_pomo'
require 'zlib'

if ARGV.length != 2
  STDERR.puts "usage: mklang.rb src_file out_file"
  exit 1
end

src_file = ARGV[0]
dst_file = ARGV[1]

po = GetPomo::PoFile.parse(File.read(src_file, :encoding => 'utf-8'))

File.open(dst_file, 'wb') do |file|
  file.write(Marshal.dump(po.map do |t|
  	id = t.msgid
  	msg = t.msgstr
  	id = id.gsub("\\\\","\\")
  	id = id.gsub("\\\"", "\"")
  	msg = msg.gsub("\\\\","\\")
  	msg = msg.gsub("\\\"", "\"")
    [Zlib.crc32(id), msg]
  end.to_h))
end
