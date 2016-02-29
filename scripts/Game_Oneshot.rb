class Game_Oneshot
  #--------------------------------------------------------------------------
  # * Public Instance Variables
  #--------------------------------------------------------------------------
  attr_accessor :player_name              # map music (for battle memory)
  attr_accessor :plight_timer             # start of plight's plight

  def initialize
    user_name = (Steam.enabled? ? Steam::USER_NAME : Oneshot::USER_NAME).split(/\s+/)
    if user_name[0].casecmp('the') == 0 || user_name[0].casecmp('a') == 0
      @player_name = user_name.join(' ')
    else
      @player_name = user_name[0]
    end
    @lights = {}
    @plight_timer = nil
  end
end
