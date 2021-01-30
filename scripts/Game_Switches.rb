#==============================================================================
# ** Game_Switches
#------------------------------------------------------------------------------
#  This class handles switches. It's a wrapper for the built-in class "Array."
#  Refer to "$game_switches" for the instance of this class.
#==============================================================================

class Game_Switches
  #--------------------------------------------------------------------------
  # * Object Initialization
  #--------------------------------------------------------------------------
  def initialize
    @data = []
  end
  #--------------------------------------------------------------------------
  # * Get Switch
  #     switch_id : switch ID
  #--------------------------------------------------------------------------
  def [](switch_id)
    assert_not_deprecated(switch_id)
    return true if switch_id == 25
    if switch_id <= 5000 and @data[switch_id] != nil
      return @data[switch_id]
    else
      return false
    end
  end
  #--------------------------------------------------------------------------
  # * Set Switch
  #     switch_id : switch ID
  #     value     : ON (true) / OFF (false)
  #--------------------------------------------------------------------------
  def []=(switch_id, value)
    assert_not_deprecated(switch_id)
    if switch_id <= 5000
      @data[switch_id] = value
    end
  end

  def assert_not_deprecated(switch_id)
    if switch_id == 10 || switch_id == 21
      STDERR.puts "deprecated switch referenced in map #{$game_map.map_id}"
    end
  end
end
