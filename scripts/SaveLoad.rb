SAVE_FILE_NAME = Oneshot::SAVE_PATH + '/save.dat'
PERMA_FLAGS_NAME = Oneshot::SAVE_PATH + '/p-settings.dat'
FAKE_SAVE_NAME = Oneshot::GAME_PATH + '/Oneshot/save_progress.oneshot'


def erase_game
  File.delete(SAVE_FILE_NAME) unless !File.exists?(SAVE_FILE_NAME)
end

def fake_save
  Dir.mkdir(Oneshot::GAME_PATH) unless File.exists?(Oneshot::GAME_PATH)
  Dir.mkdir(Oneshot::GAME_PATH + "/Oneshot") unless File.exists?(Oneshot::GAME_PATH + "/Oneshot")
  File.open(FAKE_SAVE_NAME, 'wb+') do |file|
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
	file.puts('HeyNoxidHeresTheLanguage[' + $persistent.langcode + ']')
  end
  write_perma_flags(PERMA_FLAGS_NAME)
end

def save
  if ($game_variables[3] == 0) #don't save if intro variable isn't set
    return
  end
  write_save(SAVE_FILE_NAME)
  write_perma_flags(PERMA_FLAGS_NAME)
  
  Dir.mkdir(Oneshot::SAVE_PATH + "/save_backups") unless File.exists?(Oneshot::SAVE_PATH + "/save_backups")
  i = 5
  while i > 0
    #delete save5.bk if it exists
    if File.exists?(Oneshot::SAVE_PATH + "/save_backups/save" + i.to_s + ".bk")
	  File.delete(Oneshot::SAVE_PATH + "/save_backups/save" + i.to_s + ".bk")
	end
    #delete p-settings5.bk if it exists
    if File.exists?(Oneshot::SAVE_PATH + "/save_backups/p-settings" + i.to_s + ".bk")
	  File.delete(Oneshot::SAVE_PATH + "/save_backups/p-settings" + i.to_s + ".bk")
	end

	#rename save4.bk to save5.bk if save4.bk exists
    if File.exists?(Oneshot::SAVE_PATH + "/save_backups/save" + (i-1).to_s + ".bk")
	  File.rename(Oneshot::SAVE_PATH + "/save_backups/save" + (i-1).to_s + ".bk", Oneshot::SAVE_PATH + "/save_backups/save" + i.to_s + ".bk" )
	end
	#rename p-settings4.bk to p-settings5.bk if save4.bk exists
    if File.exists?(Oneshot::SAVE_PATH + "/save_backups/p-settings" + (i-1).to_s + ".bk")
	  File.rename(Oneshot::SAVE_PATH + "/save_backups/p-settings" + (i-1).to_s + ".bk", Oneshot::SAVE_PATH + "/save_backups/p-settings" + i.to_s + ".bk" )
	end
	
    i -= 1
  end
  write_save(Oneshot::SAVE_PATH + "/save_backups/save1.bk")
  write_perma_flags(Oneshot::SAVE_PATH + "/save_backups/p-settings1.bk")
  
end

def write_save(filename)
  if ($game_variables[3] == 0) #don't save if intro variable isn't set
    return
  end
  File.open(filename, 'wb') do |file|
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

def write_perma_flags(filename)
  perma_flags = Array.new(25, false)
  for i in 151..175
    perma_flags[i-151] = $game_switches[i]
  end
  perma_vars = Array.new(25, 0)
  for i in 76..100
    perma_vars[i-76] = $game_variables[i]
  end
  p_name = $game_oneshot.player_name

  File.open(filename, 'wb') do |file|
    Marshal.dump(perma_flags, file)
    Marshal.dump(perma_vars, file)
    Marshal.dump(p_name,file)
  end
end

def load(filename)
  return false if !FileTest.exist?(filename) || $debug
  File.open(filename, 'rb') do |file|
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

	f_prev = $game_player
    for f in $game_followers
      f.leader = f_prev
      f.moveto($game_player.x, $game_player.y)
	  f_prev = f
    end
  end
  return true
end

def load_perma_flags
  
	begin
      read_perma_flags(PERMA_FLAGS_NAME)
	rescue TypeError, ArgumentError => e
	  puts "oops: #{e.message}"
	  EdText.err("p-settings.dat corrupt. Attempting to load backup.")
	  for i in 1..6
	    if !File.exists?(Oneshot::SAVE_PATH + "/save_backups/p-settings" + (i).to_s + ".bk")
	      EdText.err("All p-settings backups corrupt!  Deleting corrupt p-settings file and shutting down.")
		  File.delete(PERMA_FLAGS_NAME)
          Oneshot.allow_exit true
          Kernel.abort("All p-settings backups corrupt!  Deleting corrupt p-settings file and shutting down.")
		  break
		end
	    begin
	      read_perma_flags(Oneshot::SAVE_PATH + "/save_backups/p-settings" + (i).to_s + ".bk")
		  break
	    rescue TypeError, ArgumentError => e2
	      puts "oops: #{e2.message}"
	      EdText.err("p-settings" + (i).to_s + ".bk corrupt. Attempting to load backup.")
		end
	  end
	end
	
end

def read_perma_flags(filename)
  return false if !FileTest.exist?(filename)
  perma_flags = nil
  perma_vars = nil
  p_name = nil
  File.open(filename, 'rb') do |file|
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

#load save data
	begin
      load(SAVE_FILE_NAME)
	rescue TypeError, ArgumentError => e
	  puts "oops: #{e.message}"
	  EdText.err("save.dat corrupt. Attempting to load backup.")
	  for i in 1..6
	    if !File.exists?(Oneshot::SAVE_PATH + "/save_backups/save" + (i).to_s + ".bk")
	      EdText.err("All save backups corrupt!  Deleting corrupt save and shutting down.")
		  File.delete(SAVE_FILE_NAME)
          Oneshot.allow_exit true
          Kernel.abort("All save backups corrupt!  Deleting corrupt save file and shutting down.")
		  break
		end
	    begin
	      load(Oneshot::SAVE_PATH + "/save_backups/save" + (i).to_s + ".bk")
		  break
	    rescue TypeError, ArgumentError => e2
	      puts "oops: #{e2.message}"
	      EdText.err("save" + (i).to_s + ".bk corrupt. Attempting to load backup.")
		end
	  end
	end
	
	load_perma_flags
	
	
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
