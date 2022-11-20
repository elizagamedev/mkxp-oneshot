# Classes for translating text similar to GNU gettext
# Translator class: translate text to another language

# for debug REMOVE BEFORE COMMITTING / BUILDING
# require "zlib"

class Language
  FONT_WESTERN = 'Terminus (TTF)'
  FONT_J = 'HigashiOme Gothic regular'
  LANGUAGES = []
  class << self
    def set(lc)
      dbg_print(lc.lang)
      @data = nil
      @tr = nil
      script = nil
      [lc.full.to_s, lc.lang.to_s].each do |name|
        path = "Languages/#{name}.po"
        dbg_print(path)
        if FileTest.exist?(path)
          load_pot(path)
          loadFontMap
          Font.default_name = @languageFontMap[name]
          Journal.setLang(name)
          dbg_print(Font.default_name)
          break
        end
      end
      reset_fonts(@text_sprites)
      Oneshot.set_yes_no(tr('Yes'), tr('No'))
    end

    def load_pot(path)
      msgid = nil
      msgstr = nil
      @data = Hash.new
      lastLineWasMsgId = false
      lastLineWasMsgStr = false
      if FileTest.exist?(path)
        File.readlines(path).each do |line|
          if line.start_with?("msgid ")
            line = line[6..-1]
            #unescape the string
			#note that I tried using undump here instead before, but it doesn't play nicely with non-ascii characters
            eval("msgid = " + line)
            lastLineWasMsgId = true
            lastLineWasMsgStr = false
      
          elsif line.start_with?("msgstr ")
            line = line[7..-1]
            #unescape the string
            eval("msgstr = " + line)
            lastLineWasMsgId = false
            lastLineWasMsgStr = true
      
          elsif line.start_with?("\"")
            if lastLineWasMsgId
              eval("msgid += " + line)
              lastLineWasMsgId = true
              lastLineWasMsgStr = false
        
            elsif lastLineWasMsgStr
              eval("msgstr += " + line)
              lastLineWasMsgId = false
              lastLineWasMsgStr = true
        
            else #ignore
              lastLineWasMsgId = false
              lastLineWasMsgStr = false
        
            end
      
          else
            lastLineWasMsgId = false
            lastLineWasMsgStr = false
      
            if !(msgid.nil? || msgid.empty?)
              @data[Oneshot::crc32(msgid)] = msgstr
              msgid = nil
              msgstr = nil
            end
          end
        end
      end
    
      # make sure we cleared out the last one stored
      if !(msgid.nil? || msgid.empty?)
        @data[Oneshot::crc32(msgid)] = msgstr
        msgid = nil
        msgstr = nil
      end
    end

    # Translate some text
    def tr(string)
      #dbg_print(caller_locations(1, 1).first.tap{|loc| puts "#{loc.path}:#{loc.lineno}"})
      if @data
        rv = @data[Oneshot::crc32(string)] || string
      else
        rv = string
      end
	  
      if rv.nil?
        rv = "NULL"
	  end
      #dbg_print(string + " -> " + rv)
      return String.new(rv)
    end

    def loadFontMap
      if !@fontMapLoaded
        @languageFontMap = Hash.new
        path = "Languages/language_fonts.ini"
        if FileTest.exist?(path)
          File.readlines(path).each do |line|
            parts = line.split("=", 2)
            if parts.length == 2
              LANGUAGES.push(parts[0])
              @languageFontMap[parts[0]] = parts[1].strip
            end
          end
        end
        @fontMapLoaded = true
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
