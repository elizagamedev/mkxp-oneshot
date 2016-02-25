class Game_FastTravel
  attr_reader :zone
  attr_accessor :enabled
  alias :enabled? :enabled

  class Map
    attr_reader :id
    attr_reader :x
    attr_reader :y
    attr_reader :dir

    def initialize(id, x, y, dir)
      @id = id
      @x = x
      @y = y
      @dir = dir
    end
  end

  def initialize
    @unlocked = {}
    @zone = nil
    @enabled = false
  end

  def unlock(map, id, x, y, dir)
    @unlocked[@zone][map] = Map.new(id, x, y, dir)
  end

  def zone=(zone)
    @zone = zone
    unless @unlocked.include? zone
      @unlocked[zone] = {}
    end
  end

  def unlocked_maps
    @unlocked[@zone]
  end
end
