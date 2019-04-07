# Put any overrides/extensions to RGSS modules here

module RPG
  module Cache
    def self.character(filename, hue)
	  filename = filename.downcase
	  if $game_switches[160] && filename.start_with?("niko")
	    filename.gsub!(/niko/, "en")
	  end
      self.load_bitmap("Graphics/Characters/", filename, hue)
    end
    def self.face(filename)
	  #lowercasing facepic name to be more case insensitive
	  #to catch the few instances where we use "Niko" instead of "niko"
	  filename = filename.downcase
	  
	  #april fools!
	  t = Time.now
	  if t.month == 4 && t.day == 1 && filename.start_with?("niko")
	    filename = "af"
	  end
	  
	  if $game_switches[160] && filename.start_with?("niko")
	    filename.gsub!(/niko/, "en")
	  end
      self.load_bitmap("Graphics/Faces/", filename)
    end
    def self.menu(filename)
      self.load_bitmap("Graphics/Menus/", filename)
    end
    def self.lightmap(filename)
      self.load_bitmap("Graphics/Lightmaps/", filename)
    end
    def self.light(filename)
      self.load_bitmap("Graphics/Lights/", filename)
    end
    def self.misc(filename)
      self.load_bitmap("Graphics/Misc/", filename)
    end
  end
end

class Tone
  def +(o)
    Tone.new(self.red + o.red, self.green + o.green, self.blue + o.blue, self.gray + o.gray)
  end

  def *(s)
    Tone.new(self.red * s, self.green * s, self.blue * s, self.gray * s)
  end

  def blank?
    self.red == 0 && self.green == 0 && self.blue == 0 && self.gray == 0
  end
end
