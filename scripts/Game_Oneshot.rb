class Game_Oneshot
  #--------------------------------------------------------------------------
  # * Public Instance Variables
  #--------------------------------------------------------------------------
  attr_accessor :player_name              # map music (for battle memory)
  attr_accessor :plight_timer             # start of plight's plight
  attr_accessor :wallpaper                # the wallpaper that we should use

  def initialize
    if $GDC
      @player_name = ''
    else
      user_name = (Steam.enabled? ? Steam::USER_NAME : Oneshot::USER_NAME).split(/\s+/)
      if user_name[0].casecmp('the') == 0 || user_name[0].casecmp('a') == 0
        @player_name = user_name.join(' ')
      else
        @player_name = user_name[0]
      end
    end
    @lights = {}
    @plight_timer = nil
    @wallpaper = nil
  end
end

module Wallpaper
  def set_persistent(name)
    $game_oneshot.wallpaper = name
    Wallpaper.set(name)
  end

  def reset_persistent
    $game_oneshot.wallpaper = nil
    Wallpaper.reset
  end
end
