# Classes for translating text similar to GNU gettext

# Translator class: translate text to another language

# for debug REMOVE BEFORE COMMITTING / BUILDING
# require "zlib"

class Language
  FONT_WESTERN = 'Terminus (TTF)'
  FONT_CK = 'WenQuanYi Micro Hei'
  FONT_J = 'HigashiOme Gothic regular'
  LANG_WESTERN = [
    'en', 'fr', 'pt_BR', 'es'
  ]
  LANG_J = [
    'ja'
  ]
  LANG_CK = [
     'ko', 'zh_CN'
  ]
  LANGUAGES = LANG_WESTERN + LANG_CK + LANG_J

  class << self
    def set(lc)
      dbg_print(lc.lang)
      @data = nil
      @tr = nil
      script = nil
      [lc.full.to_s, lc.lang.to_s].each do |name|
        path = "Languages/#{name}.loc"
        dbg_print(path)
        if FileTest.exist?(path)
          File.open(path, "rb") do |file|
            @data = Marshal.load(file)
            if LANG_CK.include? name
                Font.default_name = FONT_CK
            elsif LANG_J.include? name
                Font.default_name = FONT_J
            else
                Font.default_name = FONT_WESTERN
            end
            Journal.setLang(name)
            dbg_print(Font.default_name)
          end
        end
      end
      reset_fonts(@text_sprites)
      Oneshot.set_yes_no(tr('Yes'), tr('No'))
    end

    # Translate some text
    def tr(string)
      #dbg_print(caller_locations(1, 1).first.tap{|loc| puts "#{loc.path}:#{loc.lineno}"})
      if @data
        rv = @data[Oneshot::crc32(string)] || string
      else
        rv = string
      end
      dbg_print(string + " -> " + rv)
      return String.new(rv)
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

    def dbg_print(str)
      # dbg = IO.new(STDERR.fileno)
      # if str.nil?
      #   dbg.write("null\n")
      # else
      #   dbg.write(str.to_s + "\n")
      # end
      # dbg.close()
    end

    def register_text_sprite(key, spr)
      dbg_print(key)
      if @text_sprites.nil?
        @text_sprites = Hash.new()
      end
      @text_sprites[key] = spr
    end

    def reset_fonts(sprites)
      if sprites.kind_of?(Array)
        sprites.each do |spr|
          if spr.kind_of?(Sprite) and !spr.disposed?
             spr.bitmap.font.name = Font.default_name
          end
        end
      elsif sprites.kind_of?(Hash)
        sprites.each_value do |spr|
          if spr.kind_of?(Sprite) and !spr.disposed?
            spr.bitmap.font.name = Font.default_name
          elsif spr.kind_of?(Bitmap) and !spr.disposed?
            spr.font.name = Font.default_name
          end
        end
      elsif sprites.kind_of?(Sprite) and not sprites.disposed?
        sprites.bitmap.font.name = Font.default_name
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
