#!/usr/bin/ruby
require 'zlib'

# Dummy RGSS template for unmarshalling
module RPG
  # Useful
  class Actor
    attr_reader :name
  end
  class Map
    attr_reader :events
  end
  class MapInfo
    attr_reader :name
    attr_reader :parent_id
  end
  class Event
    attr_reader :name
    attr_reader :pages
    class Page
      attr_reader :list
      class Condition
      end
      class Graphic
      end
    end
  end
  class EventCommand
    attr_reader :code
    attr_reader :parameters
  end
  class CommonEvent
    attr_reader :name
    attr_reader :list
  end
  class Item
    attr_reader :name
    attr_reader :description
  end

  # Useless
  class AudioFile
  end
  class MoveRoute
  end
  class MoveCommand
  end
end
class Table
  def self._load(foo)
  end
end
class Color
  def self._load(foo)
  end
end
class Tone
  def self._load(foo)
  end
end

# Various escape functions
# https://stackoverflow.com/questions/8639642/best-way-to-escape-and-unescape-strings-in-ruby
module Escape
  # Ruby
  UNESCAPES = {
    'a' => "\x07", 'b' => "\x08", 't' => "\x09",
    'n' => "\x0a", 'v' => "\x0b", 'f' => "\x0c",
    'r' => "\x0d", 'e' => "\x1b", "\\" => "\x5c",
    '"' => "\x22", "'" => "\x27"
  }
  ESCAPES = {
    "\x07" => 'a', "\x08" => 'b', "\x09" => 't',
    "\x0a" => 'n', "\x0b" => 'v', "\x0c" => 'f',
    "\x0d" => 'r', "\x1b" => 'e', "\x5c" => "\\",
    "\x22" => '"'
  }

  def self.unescape(str)
    str.gsub(/\\(?:([#{Regexp.escape(UNESCAPES.keys.join)}])|u([\da-fA-F]{4}))|\\0?x([\da-fA-F]{2})/) do
      if $1
        UNESCAPES[$1]
      elsif $2 # escape \u0000 unicode
        ["#$2".hex].pack('U*')
      elsif $3 # escape \0xff or \xff
        [$3].pack('H2')
      end
    end
  end

  # gettext
  def self.escape(str)
    str.gsub(/[#{Regexp.escape(ESCAPES.keys.join)}]/) do |m|
      "\\" + ESCAPES[m]
    end
  end
end

class StringList
  class Entry
    attr_accessor :string
    attr_accessor :contexts
    attr_accessor :comments

    def initialize(string)
      @string = string
      @contexts = []
      @comments = []
    end

    def add(context, comment)
      @contexts << context unless @contexts.include?(context) || context == nil
      case comment
      when String
        @comments << comment unless @comments.include?(comment)
      when Array
        comment.each do |com|
          @comments << com unless @comments.include?(com)
        end
      end
    end
  end

  def initialize
    @strings = Hash.new { |k, v| k[v] = Entry.new(v) }
    @order = []
  end

  def add(string, context, comment)
    @order << string unless @strings.include? string
    @strings[string].add(context, comment)
  end

  def dump(filename)
    File.open(filename, 'w') do |file|
      # Write header
      file.puts '# Translation template for OneShot'
      file.puts
      file.puts 'msgid ""'
      file.puts 'msgstr ""'
      file.puts
      @order.each do |str|
        entry = @strings[str]
        entry.comments.each do |comment|
          file.puts "#. #{comment}"
        end
        unless entry.contexts.empty?
          file.puts "#: #{entry.contexts.join(' ')}"
        end
        file.puts "msgid \"#{Escape.escape(entry.string)}\""
        file.puts "msgstr \"\""
        file.puts
      end
    end
  end
end

def load_data(filename)
  Marshal.load(File.binread(filename))
end

# Script
if ARGV.size < 2
  STDERR.puts "usage: mklangsrc.rb game_dir out_file"
  exit 1
end
game_dir = ARGV[0]
out_file = ARGV[1]

strlist = StringList.new

# Actors
load_data(File.join(game_dir, 'Data/Actors.rxdata')).each_with_index do |actor, i|
  next if !actor || actor.name.empty?
  strlist.add(actor.name, "Actors:#{i}", 'actor name')
end

# Items
load_data(File.join(game_dir, 'Data/Items.rxdata')).each_with_index do |item, i|
  next if !item || item.name.empty?
  strlist.add(item.name, "Items:#{i}", 'item name')
  next if item.description.empty?
  strlist.add(item.description, "Items:#{i}", 'item description')
end

# Scripts
load_data(File.join(game_dir, 'Data/xScripts.rxdata')).each do |script|
  script_name = script[1]
  comment = nil
  Zlib::Inflate.inflate(script[2]).each_line.with_index do |line, line_num|
    # Scan for tr() calls
    line.scan(/(?:^|[^_A-Za-z0-9])tr\s*\(\s*(?:'((?:\\.|[^'])*)'|"((?:\\.|[^"])*)")\s*\)/) do |single, double|
      string = single || double
      string = Escape.unescape(string)
      next if string =~ /^\s*$/
      strlist.add(string, "Scripts/#{script_name}:#{line_num+1}", comment)
    end
    # Scan for any comments on this line
    m = line.match(/^(?:[^"'#]|"(?:\\"|[^"])*"|'(?:\\'|[^'])*')*(?:#\s*(.*))?$/)
    if !m || m.captures.empty?
      comment = nil
    else
      comment = m.captures.first
    end
  end
end

# Yields all translatable EdText strings
def tr_edtext(script)
  # Scan for tr() calls
  script.scan(/(?:^|[^_A-Za-z0-9])EdText\.\w+\s*\(\s*(?:'((?:\\.|[^'])*)'|"((?:\\.|[^"])*)")\s*\)/m) do |single, double|
    string = single || double
    string = Escape.unescape(string).gsub(/\s+/, ' ').gsub(/^\s+/, '').gsub(/\s+$/, '')
    next if string =~ /^\s*$/
    yield string
  end
end

# Events
def parse_event_list(strlist, name, page, list)
  # Parse commands
  if page == nil
    context = name
  else
    context = "#{name}:#{page}"
  end
  comment = nil
  i = 0
  while i < list.size
    case list[i].code
    when 101
      # Message box
      string = list[i].parameters[0].rstrip
      loop do
        i += 1
        break unless list[i].code == 401
        string << " " << list[i].parameters[0].rstrip
      end
      string.gsub!(/\s*\\n\s*/, '\\n')
      string.strip!
      unless string.empty?
        strlist.add(string, "#{context}:#{i}:text", comment)
      end
      comment = nil
    when 102
      # Choices
      list[i].parameters[0].each_with_index do |choice, j|
        unless choice.empty?
          strlist.add(choice, "#{context}:#{i}:choice:#{j}", comment)
        end
      end
      i += 1
      comment = nil
    when 108
      # Comment
      comment = []
      loop do
        comment << list[i].parameters[0].rstrip
        i += 1
        break unless list[i].code == 408
      end
    when 111
      # Conditional branch (may contain script text)
      if list[i].parameters[0] == 12
        tr_edtext(list[i].parameters[1]) do |string|
          strlist.add(string, "#{context}:#{i}:condition", comment)
        end
      end
      i += 1
      comment = nil
    when 355
      # Script
      script = list[i].parameters[0]
      loop do
        i += 1
        break unless list[i].code == 655
        script += "\n" + list[i].parameters[0]
      end
      tr_edtext(script) do |string|
        strlist.add(string, "#{context}:#{i}:script", comment)
      end
      comment = nil
    else
      i += 1
      comment = nil
    end
  end
end

# Do common events
load_data(File.join(game_dir, 'Data/CommonEvents.rxdata')).each do |event|
  parse_event_list(strlist, 'CommonEvents/' + event.name, nil, event.list) if event
end

# Do map events
map_info = load_data(File.join(game_dir, 'Data/MapInfos.rxdata'))
map_info.each do |map_id, foo|
  # Construct full name of map
  map_name = ''
  i = map_id
  begin
    map_name.insert(0, "/" + map_info[i].name)
    i = map_info[i].parent_id
  end while i > 0

  # Load map
  map = Marshal.load(File.binread(File.join(game_dir, sprintf('Data/Map%03d.rxdata', map_id))))

  # Parse each event
  map.events.sort.each do |_, event|
    # Construct full name of event
    name = "Maps#{map_name}/#{event.name}"
    # Iterate through pages
    event.pages.each_with_index do |page, i|
      parse_event_list(strlist, name, i + 1, page.list)
    end
  end
end

strlist.dump(out_file)
