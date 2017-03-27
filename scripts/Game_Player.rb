#==============================================================================
# ** Game_Player
#------------------------------------------------------------------------------
#  This class handles the player. Its functions include event starting
#  determinants and map scrolling. Refer to "$game_player" for the one
#  instance of this class.
#==============================================================================

class Game_Player < Game_Character
  attr_reader :move_speed

  #--------------------------------------------------------------------------
  # * Invariables
  #--------------------------------------------------------------------------
  CENTER_X = (320 - 16) * 4   # Center screen x-coordinate * 4
  CENTER_Y = (240 - 16) * 4   # Center screen y-coordinate * 4
  #--------------------------------------------------------------------------
  # * Passable Determinants
  #     x : x-coordinate
  #     y : y-coordinate
  #     d : direction (0,2,4,6,8)
  #         * 0 = Determines if all directions are impassable (for jumping)
  #--------------------------------------------------------------------------
  def passable?(x, y, d)
    # Get new coordinates
    new_x = x + (d == 6 ? 1 : d == 4 ? -1 : 0)
    new_y = y + (d == 2 ? 1 : d == 8 ? -1 : 0)
    # If coordinates are outside of map
    unless $game_map.valid?(new_x, new_y)
      # Impassable
      return false
    end
    # If debug mode is ON and ctrl key was pressed
    #if $debug and Input.press?(Input::CTRL)
    #  # Passable
    #  return true
    #end
    super
  end
  #--------------------------------------------------------------------------
  # * Set Map Display Position to Center of Screen
  #--------------------------------------------------------------------------
  def center(x, y)
	if $game_switches[98] == true
      max_x = ($game_map.width - 20) * 128
      max_y = ($game_map.height - 15) * 128
      $game_map.display_x = [0, [x * 128 - CENTER_X, max_x].min].max
      $game_map.display_y = [0, [y * 128 - CENTER_Y, max_y].min].max
	else
      $game_map.display_x = x * 128 - CENTER_X
      $game_map.display_y = y * 128 - CENTER_Y
	end
  end
  #--------------------------------------------------------------------------
  # * Move to Designated Position
  #     x : x-coordinate
  #     y : y-coordinate
  #--------------------------------------------------------------------------
  def moveto(x, y)
    super
    # Centering
    if !$game_switches[100]
      center(x, y)
    end
  end
  #--------------------------------------------------------------------------
  # * Increaase Steps
  #--------------------------------------------------------------------------
  def increase_steps
    super
    # If move route is not forcing
    unless @move_route_forcing
      # Increase steps
      $game_party.increase_steps
      # Number of steps are an even number
      if $game_party.steps % 2 == 0
        # Slip damage check
        $game_party.check_map_slip_damage
      end
    end
  end
  #--------------------------------------------------------------------------
  # * Refresh
  #--------------------------------------------------------------------------
  def refresh
    # If party members = 0
    if $game_party.actors.size == 0
      # Clear character file name and hue
      @character_name = ""
      @character_hue = 0
      # End method
      return
    end
    # Get lead actor
    actor = $game_party.actors[0]
    # Set character file name and hue
    @character_name = actor.character_name
    @character_hue = actor.character_hue
    # Initialize opacity level and blending method
    @opacity = 255
    @blend_type = 0
  end
  #--------------------------------------------------------------------------
  # * Same Position Starting Determinant
  #--------------------------------------------------------------------------
  def check_event_trigger_here(triggers)
    result = false
    # If event is running
    if $game_system.map_interpreter.running?
      return result
    end
    # All event loops
    for event in $game_map.events.values
      # If event coordinates and triggers are consistent
      if event.x == @x and event.y == @y and triggers.include?(event.trigger)
        # If starting determinant is same position event (other than jumping)
        if not event.jumping? and event.over_trigger?
          event.start
          result = true
        end
      end
    end
    return result
  end
  #--------------------------------------------------------------------------
  # * Front Envent Starting Determinant
  #--------------------------------------------------------------------------
  def check_event_trigger_there(triggers)
    result = false
    # If event is running
    if $game_system.map_interpreter.running?
      return result
    end
    # Calculate front event coordinates
    new_x = @x + (@direction == 6 ? 1 : @direction == 4 ? -1 : 0)
    new_y = @y + (@direction == 2 ? 1 : @direction == 8 ? -1 : 0)
    # All event loops
    for event in $game_map.events.values
      # If event coordinates and triggers are consistent
      if event.intersects?(new_x, new_y) && triggers.include?(event.trigger)
        # If starting determinant is front event (other than jumping)
        if not event.jumping? and not event.over_trigger?
          event.start
          result = true
        end
      end
    end
    return result
  end
  #--------------------------------------------------------------------------
  # * Touch Event Starting Determinant
  #--------------------------------------------------------------------------
  def check_event_trigger_touch(x, y)
    result = false
    # If event is running
    if $game_system.map_interpreter.running?
      return result
    end
    # All event loops
    for event in $game_map.events.values
      # If event coordinates and triggers are consistent
      if event.x == x and event.y == y and [1,2].include?(event.trigger)
        # If starting determinant is front event (other than jumping)
        if not event.jumping? and not event.over_trigger?
          event.start
          result = true
        end
      end
    end
    return result
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    # Remember whether or not moving in local variables
    last_moving = moving?
    # If moving, event running, move route forcing, and message window
    # display are all not occurring
    unless $game_system.map_interpreter.running? or @move_route_forcing or
           $game_temp.message_window_showing or $game_temp.menus_visible
      # Adjust move speed
	  if $game_switches[251] == false
        @move_speed = Input.press?(Input::RUN) ? 4 : 3
	  else
        @move_speed = Input.press?(Input::RUN) ? 3 : 4
	  end
      unless moving?
        # Move player in the direction the directional button is being pressed
        case Input.dir4
        when 2
		  if $game_switches[112] == false
            move_down
		  else
		    turn_down
		  end
        when 4
          move_left
        when 6
          move_right
        when 8
		  if $game_switches[112] == false
            move_up
		  else
		    turn_up
		  end
        end
      end
    else
      @move_speed = 2
    end
    # Remember coordinates in local variables
    last_real_x = @real_x
    last_real_y = @real_y
    super
    # Emit footsteps
    if moving? || last_real_x != @real_x || last_real_y != @real_y
      @footstep_timer += 1
      if (@move_speed <= 3 && @footstep_timer >= 16) || (@move_speed >= 4 && @footstep_timer >= 12)
        emit_footstep
        @footstep_timer = 0
      end
    else
      @footstep_timer = 8
    end
    if !$game_switches[100]
      # If character moves down and is positioned lower than the center
      # of the screen
      if @real_y > last_real_y and @real_y - $game_map.display_y > CENTER_Y
        # Scroll map down
        $game_map.scroll_down(@real_y - last_real_y)
      end
      # If character moves left and is positioned more let on-screen than
      # center
      if @real_x < last_real_x and @real_x - $game_map.display_x < CENTER_X
        # Scroll map left
        $game_map.scroll_left(last_real_x - @real_x)
      end
      # If character moves right and is positioned more right on-screen than
      # center
      if @real_x > last_real_x and @real_x - $game_map.display_x > CENTER_X
        # Scroll map right
        $game_map.scroll_right(@real_x - last_real_x)
      end
      # If character moves up and is positioned higher than the center
      # of the screen
      if @real_y < last_real_y and @real_y - $game_map.display_y < CENTER_Y
        # Scroll map up
        $game_map.scroll_up(last_real_y - @real_y)
      end
    end
    # If not moving
    unless moving? or $game_temp.menus_visible
      # If player was moving last time
      if last_moving
        # Event determinant is via touch of same position event
        check_event_trigger_here([1,2])
      end
      # If C button was pressed
      if Input.trigger?(Input::ACTION)
        # Same position and front event determinant
        check_event_trigger_here([0])
        check_event_trigger_there([0,1,2])
      end
    end
  end

  # Footsteps
  def emit_footstep
    return unless $game_temp.footstep_sfx
    tag = $game_map.terrain_tag(@x, @y) - 1
    if tag >= 0 && tag < $game_temp.footstep_sfx.size 
      name = $game_temp.footstep_sfx[tag]
      if name.kind_of?(Array)
        name, volume = name
      else
        volume = 1.0
      end
      if FOOTSTEP_AMT.include? name
        name += '%02d' % [rand(FOOTSTEP_AMT[name]) + 1]
      end
      pitch = 85 + rand(30)
      vol = 70 + rand(20)
	  if $game_switches[112] == true
	    name = "wheel_squeak1"
		pitch = 120 + rand(10)
		if @wheel_squeak != true
		  pitch += 10
		  @wheel_squeak = true
		else
		  @wheel_squeak = false
		end
	  end
      Audio.se_play("Audio/SE/#{name}.wav", (vol * volume).to_i, pitch.to_i)
    end
    emit_footsplash(@direction)
  end
end
