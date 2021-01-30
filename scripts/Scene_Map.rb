#==============================================================================
# ** Scene_Map
#------------------------------------------------------------------------------
#  This class performs map screen processing.
#==============================================================================

class Scene_Map
  #--------------------------------------------------------------------------
  # * Main Processing
  #--------------------------------------------------------------------------
  def main
    # Make sprite set
    @spriteset = Spriteset_Map.new
    # Make message window
    @message_window = Window_Message.new
    @ed_message = Ed_Message.new
    @doc_message = Doc_Message.new
    @desktop_message = Desktop_Message.new
    @credits_message = Credits_Message.new
    # Make menus
    @menu = Window_MainMenu.new
    @item_menu = Window_Item.new
    @item_menu_refresh = false
    # Make item icon
    @item_icon = Sprite.new
    @item_icon.x = 640 - 64
    @item_icon.y = 480 - 64
    @item_icon.z = 9000
    @item_icon.zoom_x = 2.0
    @item_icon.zoom_y = 2.0
    @item_id = 0
    # Make item flash (for clover)
    @item_icon_flash = Sprite.new
    @item_icon_flash.x = 640 - 64
    @item_icon_flash.y = 480 - 64
    @item_icon_flash.z = 10000
    @item_icon_flash.zoom_x = 2.0
    @item_icon_flash.zoom_y = 2.0
    @item_icon_flash.opacity = 0
    @item_icon_flash_fadein = true
    @item_icon_flash_wait = 100
    # Make fast travel menu
    @fast_travel = FastTravel.new
    @window_settings = Window_Settings.new
    # Fade to black transition
    @blackfade = Sprite.new
    @blackfade.bitmap = Bitmap.new(640, 480)
    @blackfade.bitmap.fill_rect(0, 0, 640, 480, Color.new(0, 0, 0))
    @blackfade.visible = false
    @blackfade.z = 9999
    # Transition run
    Graphics.transition
    # Main loop
    loop do
      # Update game screen
      Graphics.update
      # Update input information
      Input.update
      # Frame update
      update
      # Abort loop if screen is changed
      if $scene != self
        break
      end
    end
    # Prepare for transition
    Graphics.freeze
    # Dispose of sprite set
    @spriteset.dispose
    # Dispose of message window
    @message_window.dispose
    @ed_message.dispose
    @doc_message.dispose
    @desktop_message.dispose
    @credits_message.dispose
    # Dispose of menu
    @menu.dispose
    @item_menu.dispose
    @fast_travel.dispose
	@window_settings.dispose
    # Dispose of item icon
    @item_icon.dispose
    @item_icon_flash.dispose
    @blackfade.dispose
    # If switching to title screen
    if $scene.is_a?(Scene_Title)
      # Fade out screen
      Graphics.transition
      Graphics.freeze
    end
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    if Input.quit?
      # put in dialogue boxes here for player
      # either telling them they can't quit during a cutscene
      # or telling them they're saving and quitting
      if $game_system.map_interpreter.running?
        EdText.info(tr("You cannot perform this action during cutscenes."))
        return
      else
        $game_temp.common_event_id = 35
      end
    end
    # Loop
    loop do
      if $game_temp.prompt_wait > 0
        $game_temp.prompt_wait -= 1
        return
      end
      $game_temp.bgm_fadein($game_system)
      # Update map, interpreter, and player order
      # (this update order is important for when conditions are fulfilled
      # to run any event, and the player isn't provided the opportunity to
      # move in an instant)
      $game_map.update
      if $scene == nil
        return
      end
      $game_system.map_interpreter.update
	  $game_temp.menus_visible = @menu.visible || @item_menu.visible || @fast_travel.visible || @window_settings.visible
      $game_player.update
      $game_followers.each{|f| f.update}
      # Update system (timer), screen
      $game_system.update
      $game_screen.update
      # Abort loop if player isn't place moving
      unless $game_temp.player_transferring
        break
      end
      # Run place move
      transfer_player
      # Abort loop if transition processing
      if $game_temp.transition_processing
        break
      end
    end
    # Update sprite set
    @spriteset.update
    # Update message window
    @message_window.update
    @ed_message.update
    @doc_message.update
    @desktop_message.update
    @credits_message.update
    # Deactivate item
    if Input.trigger?(Input::DEACTIVATE) && $game_variables[1] > 0
      $game_system.se_play($data_system.cancel_se)
      $game_variables[1] = 0
    end

    # Update the menu
    if @message_window.visible || @ed_message.visible || @doc_message.visible || @desktop_message.visible || @credits_message.visible
      @item_menu_refresh = true
    else
      if @item_menu_refresh
        @item_menu_refresh = false
        @item_menu.refresh
      end
      @menu.update
      @item_menu.update
    end
    # Update the item icon
    if @item_id != $game_variables[1]
      @item_id = $game_variables[1]
      if @item_id == 0
        @item_icon.bitmap = nil
        @item_icon_flash.bitmap = nil
      else
	    name = $data_items[@item_id].icon_name
	    translation_name = "#{$persistent.langcode}/#{name}"
        if File.exists?("Graphics/Icons/#{translation_name}.png")
          @item_icon.bitmap = RPG::Cache.icon(translation_name)
          @item_icon.zoom_x = 1.0
          @item_icon.zoom_y = 1.0
	    else
          @item_icon.bitmap = RPG::Cache.icon(name)
          @item_icon.zoom_x = 2.0
          @item_icon.zoom_y = 2.0
	    end
	  
        if @item_id == 58 #clover
          @item_icon_flash.bitmap = RPG::Cache.icon($data_items[@item_id].icon_name + "2")
          @item_icon_flash.opacity = 0
          @item_icon_flash_fadein = true
          @item_icon_flash_wait = 80
        else
          @item_icon_flash.bitmap = nil
        end
      end
    end
    # Hide icon when item menu is visible
    @item_icon.visible = !@item_menu.visible
    @item_icon_flash.visible = @item_icon.visible

    if @item_icon_flash_wait > 0
      @item_icon_flash_wait -= 1
    else
      if @item_icon_flash_fadein
        @item_icon_flash.opacity += 6
        if @item_icon_flash.opacity >= 255
          @item_icon_flash.opacity = 255
          @item_icon_flash_fadein = false
          @item_icon_flash_wait = 40
        end
      else
        @item_icon_flash.opacity -= 6
        if @item_icon_flash.opacity <= 0
          @item_icon_flash.opacity = 0
          @item_icon_flash_fadein = true
          @item_icon_flash_wait = 160
        end
      end
    end
    # If game over
    if $game_temp.gameover
      # Switch to game over screen
      $scene = Scene_Gameover.new
      return
    end
    # If returning to title screen
    if $game_temp.to_title
      # Change to title screen
      $scene = Scene_Title.new
      return
    end
    # If transition processing
    if $game_temp.transition_processing
      # Clear transition processing flag
      $game_temp.transition_processing = false
      # Execute transition
      if $game_temp.transition_name == ""
        Graphics.transition(20)
      elsif $game_temp.transition_name == "black"
        @blackfade.visible = true
        Graphics.transition(30)
        Graphics.freeze
        @blackfade.visible = false
        Graphics.transition(30)
      else
        Graphics.transition(40, "Graphics/Transitions/" +
          $game_temp.transition_name)
      end
    end
    # Update fast travel
    @fast_travel.update
	@window_settings.update

    if Input.trigger?(Input::F8) && !$game_switches[123]
      if Graphics.fullscreen == true
	    Graphics.fullscreen = false
		$console = false
	  else
	    Graphics.fullscreen = true
		$console = true
	  end
	  sleep(0.500)
	  if @window_settings.visible
	    @window_settings.redraw_setting_index(2)
	    sleep(0.500)
	  end
    end
    # If showing message window
    if $game_temp.message_window_showing || @ed_message.visible || @doc_message.visible || @desktop_message.visible || @credits_message.visible
      return
    end
    # Process menu opening
    unless $game_system.map_interpreter.running? ||
        $game_system.menu_disabled ||
        @fast_travel.visible || @window_settings.visible || $game_temp.menu_calling == true  || $game_temp.item_menu_calling == true
      if !@menu.visible && Input.trigger?(Input::MENU)
        $game_temp.menu_calling = true
        $game_temp.menu_beep = true
      elsif !@item_menu.visible && Input.trigger?(Input::ITEMS) && ($game_switches[174] == false)
        $game_temp.item_menu_calling = true
        $game_temp.menu_beep = true
      end
    end
    # If debug mode is ON and F5 key was pressed
    if $debug and Input.press?(Input::F5)
      # Set transferring player flag
      $game_temp.player_transferring = true
      # Set player move destination
      $game_temp.player_new_map_id = $data_system.start_map_id
      $game_temp.player_new_x = $data_system.start_x
      $game_temp.player_new_y = $data_system.start_y
    end
    # debug && F6
    if $debug and Input.press?(Input::F6) and $lastpress != 6
      $lastpress = 6
      Chroma.playAnim("chroma/blank_keyboard.chroma", false);
    end
    if $debug and Input.press?(Input::F7) and $lastpress != 7
      $lastpress = 7
      Chroma.playAnim("chroma/Fire_Keyboard.chroma", true);
    end
    if $debug and Input.press?(Input::F9) and $lastpress != 9
      $lastpress = 9
      Chroma.playAnim("chroma/Random_Keyboard.chroma", false);
    end
    # If debug mode is ON and F9 key was pressed
    # if $debug and Input.press?(Input::F9)
    #   # Set debug calling flag
    #   $game_temp.debug_calling = true
    # end
    # If player is not moving
    unless $game_player.moving?
      # Run calling of each screen
      if $game_temp.battle_calling
        call_battle
      elsif $game_temp.shop_calling
        call_shop
      elsif $game_temp.name_calling
        call_name
      elsif $game_temp.menu_calling
        call_menu
      elsif $game_temp.item_menu_calling
        call_item_menu
      elsif $game_temp.travel_menu_calling
        call_travel_menu
      elsif $game_temp.window_settings_calling
        call_window_settings
      elsif $game_temp.save_calling
        call_save
      elsif $game_temp.debug_calling
        call_debug
      end
    end
  end
  #--------------------------------------------------------------------------
  # * Battle Call
  #--------------------------------------------------------------------------
  def call_battle
    # Clear battle calling flag
    $game_temp.battle_calling = false
    # Clear menu calling flag
    $game_temp.menu_calling = false
    $game_temp.menu_beep = false
    # Memorize map BGM and stop BGM
    $game_temp.map_bgm = $game_system.playing_bgm
    $game_system.bgm_stop
    # Play battle start SE
    $game_system.se_play($data_system.battle_start_se)
    # Play battle BGM
    $game_system.bgm_play($game_system.battle_bgm)
    # Straighten player position
    $game_player.straighten
    # Switch to battle screen
    $scene = Scene_Battle.new
  end
  #--------------------------------------------------------------------------
  # * Shop Call
  #--------------------------------------------------------------------------
  def call_shop
    # Clear shop call flag
    $game_temp.shop_calling = false
    # Straighten player position
    $game_player.straighten
    # Switch to shop screen
    $scene = Scene_Shop.new
  end
  #--------------------------------------------------------------------------
  # * Name Input Call
  #--------------------------------------------------------------------------
  def call_name
    # Clear name input call flag
    $game_temp.name_calling = false
    # Straighten player position
    $game_player.straighten
    # Switch to name input screen
    $scene = Scene_Name.new
  end
  #--------------------------------------------------------------------------
  # * Menu Call
  #--------------------------------------------------------------------------
  def call_menu
    # Clear menu call flag
    $game_temp.menu_calling = false
    # If menu beep flag is set
    if $game_temp.menu_beep
      # Play decision SE
      $game_system.se_play($data_system.decision_se)
      # Clear menu beep flag
      $game_temp.menu_beep = false
    end
    # Straighten player position
    $game_player.straighten
    # Open the menu
    @menu.open
  end
  def call_item_menu
    # Clear menu call flag
    $game_temp.item_menu_calling = false
    # If menu beep flag is set
    if $game_temp.menu_beep
      # Play decision SE
      $game_system.se_play($data_system.decision_se)
      # Clear menu beep flag
      $game_temp.menu_beep = false
    end
    # Straighten player position
    $game_player.straighten
    # Open the menu
    @item_menu.open
  end
  def call_travel_menu
    # Clear menu call flag
    $game_temp.travel_menu_calling = false
    # Straighten player position
    $game_player.straighten
    # Open the menu
    @fast_travel.open
  end
  def call_window_settings
    # Clear menu call flag
    $game_temp.window_settings_calling = false
    # Straighten player position
    $game_player.straighten
    # Open the menu
    @window_settings.open
  end
  #--------------------------------------------------------------------------
  # * Save Call
  #--------------------------------------------------------------------------
  def call_save
    # Straighten player position
    $game_player.straighten
    # Switch to save screen
    $scene = Scene_Save.new
  end
  #--------------------------------------------------------------------------
  # * Debug Call
  #--------------------------------------------------------------------------
  def call_debug
    # Clear debug call flag
    $game_temp.debug_calling = false
    # Play decision SE
    $game_system.se_play($data_system.decision_se)
    # Straighten player position
    $game_player.straighten
    # Switch to debug screen
    $scene = Scene_Debug.new
  end
  #--------------------------------------------------------------------------
  # * Player Place Move
  #--------------------------------------------------------------------------
  def transfer_player
    # Clear player place move call flag
    $game_temp.player_transferring = false
    # If move destination is different than current map
    if $game_map.map_id != $game_temp.player_new_map_id
      # Set up a new map
      $game_map.setup($game_temp.player_new_map_id)
    end
    # Set up player/follower positions
    [$game_player].concat($game_followers).each do |character|
      character.moveto($game_temp.player_new_x, $game_temp.player_new_y)
      # Set player direction
      case $game_temp.player_new_direction
      when 2  # down
        character.turn_down
      when 4  # left
        character.turn_left
      when 6  # right
        character.turn_right
      when 8  # up
        character.turn_up
      end
      # Straighten player position
      character.straighten
    end
    # Remake sprite set
    @spriteset.dispose
    @spriteset = Spriteset_Map.new
    # Update map (run parallel process event)
    $game_map.update
    @spriteset.update
    # # If processing transition
    # if $game_temp.transition_processing
    #   # Clear transition processing flag
    #   $game_temp.transition_processing = false
    #   # Execute transition
    #   Graphics.transition(20)
    # end
    # Run automatic change for BGM and BGS set on the map
    $game_map.autoplay
    # Frame reset
    Graphics.frame_reset
  end
  #--------------------------------------------------------------------------
  # * Lighting operations
  #--------------------------------------------------------------------------
  def add_light(id, filename, intensity, x, y)
    @spriteset.add_light(id, filename, intensity, x, y)
  end
  def del_light(id)
    @spriteset.del_light(id)
  end
  def clear_lights
    @spriteset.clear_lights
  end
  #--------------------------------------------------------------------------
  # * Particle operations
  #--------------------------------------------------------------------------
  def particles=(val)
    @spriteset.particles = val
  end
  #--------------------------------------------------------------------------
  # * Follower operations
  #--------------------------------------------------------------------------
  def add_follower(follower)
    @spriteset.add_follower(follower)
  end
  def remove_follower(follower)
    @spriteset.remove_follower(follower)
  end
  #--------------------------------------------------------------------------
  # * BG operations
  #--------------------------------------------------------------------------
  def bg=(name)
    @spriteset.bg = name
  end
  #--------------------------------------------------------------------------
  # * Misc operations
  #--------------------------------------------------------------------------
  def new_footprint(direction, x, y)
    if @spriteset != nil
      @spriteset.new_footprint(direction, x, y)
	end
  end
  def new_footsplash(direction, x, y)
    if @spriteset != nil
      @spriteset.new_footsplash(direction, x, y)
	end
  end
  def new_maptext(text, x, y)
    if @spriteset != nil
      @spriteset.new_maptext(text, x, y)
	end
  end
  def fix_footsplashes(xDelt, yDelt)
    if @spriteset != nil
      @spriteset.fix_footsplashes(xDelt, yDelt)
	end
  end
  def menu_open?
    @menu.visible || @item_menu.visible
  end
end
