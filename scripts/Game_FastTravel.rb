class Game_FastTravel
  attr_reader :zone
  attr_accessor :enabled
  alias :enabled? :enabled

  def initialize
    @unlocked = {}
    @zone = nil
    @enabled = false
  end

  def unlock(map)
    @unlocked[@zone] << map
  end

  def zone=(zone)
    @zone = zone
    unless @unlocked.include? zone
      @unlocked[zone] = []
    end
  end

  def unlocked_maps
    @unlocked[@zone]
  end
end
