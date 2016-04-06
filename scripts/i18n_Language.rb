# Classes for translating text similar to GNU gettext

# Translator class: translate text to another language
class Language
  FONT_WESTERN = 'Terminus (TTF)'
  FONT_CJK = 'WenQuanYi Micro Hei'

  class << self
    def set(lc)
      @tr = nil
      script = nil
      [lc.full.to_s, lc.lang.to_s].each do |name|
        begin
          @tr, script = load_data("Languages/#{name}.rxdata")
          break
        rescue
        end
      end
      exec(script) if script
      Oneshot.set_yes_no(tr('Yes'), tr('No'))
    end

    # Translate some text
    def tr(string)
      if @data
        @data[Zlib::crc32(string)] || string
      else
        string
      end
    end

    # Turn all database items into translatable strings
    def initialize_database
      $data_actors.each do |i|
        i.name = TrString.new(i.name) if i && !i.name.empty?
      end
      $data_items.each do |i|
        if i && !i.name.empty?
          i.name = TrString.new(i.name)
          i.description = TrString.new(i.description)
        end
      end
    end
  end
end

# Translatable string
class TrString
  def initialize(str)
    @str = str
  end

  def to_str
    Language.tr(@str)
  end
  alias :to_s :to_str
end

def tr(text)
  TrString.new(text)
end
