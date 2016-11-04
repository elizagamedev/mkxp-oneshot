SAVE_FILE_NAME = Oneshot::SAVE_PATH + '/save.dat'

def save
  File.open(SAVE_FILE_NAME, 'wb') do |file|
    # Wrire frame count for measuring play time
    Marshal.dump(Graphics.frame_count, file)
    # Increase save count by 1
    $game_system.save_count += 1
    # Save magic number
    # (A random value will be written each time saving with editor)
    $game_system.magic_number = $data_system.magic_number
    # Write each type of game object
    Marshal.dump($game_system, file)
    Marshal.dump($game_switches, file)
    Marshal.dump($game_variables, file)
    Marshal.dump($game_self_switches, file)
    Marshal.dump($game_screen, file)
    Marshal.dump($game_actors, file)
    Marshal.dump($game_party, file)
    Marshal.dump($game_map, file)
    Marshal.dump($game_player, file)
    Marshal.dump($game_followers, file)
    Marshal.dump($game_oneshot, file)
    Marshal.dump($game_fasttravel, file)
    Marshal.dump($game_temp.footstep_sfx , file)
  end
end

def load
  return false if !FileTest.exist?(SAVE_FILE_NAME) || $debug
  File.open(SAVE_FILE_NAME, 'rb') do |file|
    # Read frame count for measuring play time
    Graphics.frame_count = Marshal.load(file)
    # Read each type of game object
    $game_system        = Marshal.load(file)
    $game_switches      = Marshal.load(file)
    $game_variables     = Marshal.load(file)
    $game_self_switches = Marshal.load(file)
    $game_screen        = Marshal.load(file)
    $game_actors        = Marshal.load(file)
    $game_party         = Marshal.load(file)
    $game_map           = Marshal.load(file)
    $game_player        = Marshal.load(file)
    $game_followers     = Marshal.load(file)
    $game_oneshot       = Marshal.load(file)
    $game_fasttravel    = Marshal.load(file)
	  $game_temp.footstep_sfx = Marshal.load(file)
    # If magic number is different from when saving
    # (if editing was added with editor)
    if $game_system.magic_number != $data_system.magic_number
      # Load map
      $game_map.setup($game_map.map_id)
      if !$game_switches[100]
        $game_player.center($game_player.x, $game_player.y)
      end
    end
    # Refresh party members
    $game_party.refresh

    for f in $game_followers
      f.leader = $game_player
      f.moveto($game_player.x, $game_player.y)
    end
  end
  return true
end

def real_load
    load
    # Restore BGM and BGS
    # switch 181 or 183 or 186 means its time for a dream, so no BGM

    if !($game_switches[181] || $game_switches[183] || $game_switches[186] || $game_switches[188] || $game_switches[190])
      $game_system.bgm_play($game_system.playing_bgm)
      $game_system.bgs_play($game_system.playing_bgs)
    end
    # Update map (run parallel process event)
    $game_map.update
    # Switch to map screen
    $scene = Scene_Map.new
    $game_temp.common_event_id = 42
    # Set wallpaper if necessary
    if $game_oneshot.wallpaper
      Wallpaper.set $game_oneshot.wallpaper
    end
end

def save_exists
  return FileTest.exist?(SAVE_FILE_NAME)
end

def quit_game_bed
  $game_system.map_interpreter.index += 1
  save
  $game_system.map_interpreter.index -= 1
  $scene = nil
end

def quit_game_no_save
  $scene = nil
end
