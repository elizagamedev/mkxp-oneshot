# Put any overrides/extensions to RGSS modules here

module RPG
  module Cache
    def self.face(filename)
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

module Graphics
  class << self
    alias_method :update_native, :update
    def update
      #if $game_map.map_id == 97
      #  Oneshot.allow_exit false
      #elsif $game_system.map_interpreter
      # Oneshot.allow_exit ($scene == nil)
      #end
      update_native
    end
  end
end
