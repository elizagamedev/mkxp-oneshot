#!/usr/bin/ruby

require 'get_pomo'
require 'zlib'

if ARGV.length != 2
  STDERR.puts "usage: mklang.rb src_file out_file"
  exit 1
end

src_file = ARGV[0]
dst_file = ARGV[1]

po = GetPomo::PoFile.parse(File.read(src_file))

File.open(dst_file, 'wb') do |file|
  file.write(Marshal.dump(po.map do |t|
    [Zlib.crc32(t.msgid), t.msgstr]
  end.to_h))
end
