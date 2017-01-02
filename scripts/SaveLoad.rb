SAVE_FILE_NAME = Oneshot::SAVE_PATH + '/save.dat'
PERMA_FLAGS_NAME = Oneshot::SAVE_PATH + '/p-settings.dat'
FAKE_SAVE_NAME = Oneshot::DOCS_PATH + '\\My Games\\Oneshot\\save_progress.oneshot'


def erase_game
  File.delete(SAVE_FILE_NAME)
end

def fake_save
  Dir.mkdir(Oneshot::DOCS_PATH + "\\My Games") unless File.exists?(Oneshot::DOCS_PATH + "\\My Games")
  Dir.mkdir(Oneshot::DOCS_PATH + "\\My Games\\Oneshot") unless File.exists?(Oneshot::DOCS_PATH + "\\My Games\\Oneshot")
  File.open(FAKE_SAVE_NAME, 'wb') do |file|
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
  save_perma_flags
end

def save
  if ($game_variables[3] == 0) #don't save if intro variable isn't set
    return
  end
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
  save_perma_flags
end

def save_perma_flags
  perma_flags = Array.new(25, false)
  for i in 151..175
    perma_flags[i-151] = $game_switches[i]
  end
  perma_vars = Array.new(25, 0)
  for i in 76..100
    perma_vars[i-76] = $game_variables[i]
  end
  p_name = $game_oneshot.player_name

  File.open(PERMA_FLAGS_NAME, 'wb') do |file|
    Marshal.dump(perma_flags, file)
    Marshal.dump(perma_vars, file)
    Marshal.dump(p_name,file)
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
  load_perma_flags
  return true
end

def load_perma_flags
  return false if !FileTest.exist?(PERMA_FLAGS_NAME)
  perma_flags = nil
  perma_vars = nil
  p_name = nil
  File.open(PERMA_FLAGS_NAME, 'rb') do |file|
    perma_flags      = Marshal.load(file)
    perma_vars       = Marshal.load(file)
    p_name           = Marshal.load(file)
  end
  for i in 151..175
    $game_switches[i] = perma_flags[i-151]
  end
  for i in 76..100
    $game_variables[i] = perma_vars[i-76]
  end
  $game_oneshot.player_name = p_name
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
	if !$game_switches[198]  # if this was a "checkpoint" save
      $game_temp.common_event_id = 42
	end
    # Set wallpaper if necessary
    if $game_oneshot.wallpaper
      Wallpaper.set $game_oneshot.wallpaper, $game_oneshot.wallpaper_color
    end
end

def save_exists
  return FileTest.exist?(SAVE_FILE_NAME)
end

def fake_save_exists
  return FileTest.exist?(FAKE_SAVE_NAME)
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
