# Persistent data independent from save file.
# Stores settings and the like.
class LanguageCode
  attr_accessor :full
  attr_accessor :lang
  attr_accessor :region

  def initialize(str)
    parts = str.split(/[_-]/)
    if parts.size == 1
      @lang, @region = parts.first.downcase.to_sym, nil
      @full = @lang
    elsif parts.size == 2
      @lang, @region = parts.first.downcase.to_sym, parts.last.upcase.to_sym
      @full = (parts.first.downcase + '_' + parts.last.upcase).to_sym
    else
      raise "malformed language code: #{str}"
    end
  end
end

class Persistent
  attr_reader :lang

  def initialize
    self.lang = LanguageCode.new(Steam::LANG || Oneshot::LANG)
  end

  # Members
  def lang=(val)
    @lang = val
    $tr = Translator.new(@lang)
    $lang = Language.get(@lang)
    Font.default_name = $lang.font
    Oneshot.set_yes_no(tr('Yes'), tr('No'))
  end

  # MARSHAL
  def marshal_dump
    [@lang]
  end
  def marshal_load(array)
    self.lang = array.first
  end

  # Save/Load global instance of persistent
  FILE_NAME = Oneshot::SAVE_PATH + '/persistent.dat'
  def self.save
    File.open(FILE_NAME, 'wb') do |file|
      Marshal.dump($persistent, file)
    end
  end

  def self.load
    if FileTest.exist?(FILE_NAME)
      File.open(FILE_NAME, 'rb') do |file|
        $persistent = Marshal.load(file)
      end
    else
      $persistent = Persistent.new
    end
  end
end
