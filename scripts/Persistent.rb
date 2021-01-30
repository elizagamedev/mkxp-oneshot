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
    if File.exists?("zh_CN.ver")
      self.lang = LanguageCode.new("zh_CN")
    end
    if not Steam.enabled? then
      self.lang = LanguageCode.new("en")
    end
  end

  # Members
  def lang=(val)
    case val
    when String
      @lang = LanguageCode.new(val)
    when LanguageCode
      @lang = val
    else
      raise 'value passed to Persistent.lang neither String nor LanguageCode'
    end
    Language.set(@lang)
  end
  
  def langcode
  	self.lang.full.to_s
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
  def save
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
