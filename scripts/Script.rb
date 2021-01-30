PROTO_TEXT = "put me in the big portal"
CEDRIC_TEXT = "put me in the big portal"
RUE_TEXT = "put me in the big portal"

# Gnome 3 does not show desktop entries in Nautilus.
SUPPORTED_DE = ["cinnamon", "mate", "kde", "xfce"]

module Script
  def self.px
    logpos($game_player.x, $game_player.real_x, $game_player.direction == 6)
  end

  def self.py
    logpos($game_player.y, $game_player.real_y, $game_player.direction == 2)
  end

  def self.is_name_swear
    test_string = $game_oneshot.player_name.downcase
    if ["fuck", "shit", "ass", "cock", "damn", "penis", "tiddy", "titty", "cunt", "fag", "whore"].any? { |swear| test_string.include? swear }
      return true
    end
    return false
  end

  def self.is_name_niko
    test_string = $game_oneshot.player_name.downcase
    if test_string == "niko"
      return true
    end
    return false
  end

  def self.is_name_like_niko
    test_string = $game_oneshot.player_name.downcase
    if ["nico", "nikko", "nicco", "nikoh", "nicoh"].any? { |name| test_string == name }
      return true
    end
    return false
  end

  def self.is_name_like_mom_dad
    test_string = $game_oneshot.player_name.downcase
    if ["mom", "dad", "mommy", "daddy", "mama", "papa"].any? { |name| test_string == name }
      return true
    end
    return false
  end

  def self.is_name_gross
    test_string = $game_oneshot.player_name.downcase
    if ["poo", "pee", "fart", "stinky", "piss", "shit", "slimy"].any? { |name| test_string == name }
      return true
    end
    return false
  end

  def self.start_bruteforce
    $game_oneshot.bruteforce_start = Graphics.frame_count
  end

  def self.check_bruteforce
    if($game_oneshot.bruteforce_start == nil)
      $game_oneshot.bruteforce_start = Graphics.frame_count
    end
    time_passed = Graphics.frame_count - $game_oneshot.bruteforce_start
    if(time_passed > (2 * 60 * 63014))
      return true
    else
      return false
    end
  end

  def self.skip_bruteforce
    $game_oneshot.bruteforce_start -= 62800*2*60
  end

  def self.lose_all_items
    for i in 1..99
      $game_party.lose_item(i, 99)
    end
  end

  def self.bruteforce_vars
    if($game_oneshot.bruteforce_start == nil)
      $game_oneshot.bruteforce_start = Graphics.frame_count
    end
    time_passed = Graphics.frame_count - $game_oneshot.bruteforce_start
    counts_tried = time_passed / (2 * 60)
    digit1 = counts_tried % 10;
    digit2 = ((counts_tried % 100) - digit1) / 10
    digit3 = ((counts_tried % 1000) - (digit2 * 10) - digit1) / 100
    digit4 = ((counts_tried % 10000) - (digit3 * 100) - (digit2 * 10) - digit1) / 1000
    digit5 = ((counts_tried % 100000) - (digit4 * 1000) - (digit3 * 100) - (digit2 * 10) - digit1) / 10000
    $game_variables[26] = digit5
    $game_variables[27] = digit4
    $game_variables[28] = digit3
    $game_variables[29] = digit2
    $game_variables[30] = digit1
  end

  def self.fix_footsplashes(xDelta, yDelta)
    $scene.fix_footsplashes(xDelta, yDelta)
  end

  def self.move_player_relative(x, y)
    $game_player.x += x
    $game_player.y += y
    $game_player.real_x += x * 4 * 32
    $game_player.real_y += y * 4 * 32
    $game_map.display_x += x * 4 * 32
    $game_map.display_y += y * 4 * 32
  end

  def self.move_player(x, y)
    xDelta = x - $game_player.x
	yDelta = y - $game_player.y
    Script.move_player_relative(xDelta, yDelta)
  end

  def self.eve_x(name)
    for event in $game_map.events.values
      if event.name == name
        return logpos(event.x, event.real_x, event.direction == 6)
      end
    end
    return 0
  end

  def self.eve_y(name)
    for event in $game_map.events.values
      if event.name == name
        return logpos(event.y, event.real_y, event.direction == 2)
      end
    end
    return 0
  end

  def self.countdown_over
    equinox = Time.new(2017, 03, 27)
	diff = equinox - Time.now
	if(diff <= 0)
	  return true
	end
	return false
  end

  def self.countdown_extend_over
    equinox = Time.new(2017, 03, 27)
	diff = equinox - Time.now
	if(diff <= 0)
	  return true
	end
	return false
  end

  def self.cdown_update(equinox)
    diff = equinox - Time.now
	if(diff < 0)
	  diff = 0
	end
	seconds = diff % 60
	minutes = ((diff - seconds)/ 60) % 60
	hours = ((((diff - seconds)/ 60) - minutes) / 60) % 24
	days = (((((diff - seconds)/ 60) - minutes) / 60) - hours) / 24
	secs1 = seconds % 10
	secs2 = (seconds - secs1) / 10
	mins1 = minutes % 10
	mins2 = (minutes - mins1) / 10
	hrs1 = hours % 10
	hrs2 = (hours - hrs1) / 10
	dys1 = days % 10
	dys2 = ((days - dys1) / 10) % 10
	dys3 = ((days - dys1 - (dys2*10)) / 100) % 10
	dys4 = ((days - dys1 - (dys2*10) - (dys3*100)) / 1000) % 10
	secs1 = secs1.floor
	change = false
	if secs1 != $game_variables[101]
	  $game_variables[101] = secs1
	  change = true
	end
	if secs2 != $game_variables[102]
	  $game_variables[102] = secs2
	  change = true
	end
	if mins1 != $game_variables[103]
	  $game_variables[103] = mins1
	  change = true
	end
	if mins2 != $game_variables[104]
	  $game_variables[104] = mins2
	  change = true
	end
	if hrs1 != $game_variables[105]
	  $game_variables[105] = hrs1
	  change = true
	end
	if hrs2 != $game_variables[106]
	  $game_variables[106] = hrs2
	  change = true
	end
	if dys1 != $game_variables[107]
	  $game_variables[107] = dys1
	  change = true
	end
	if dys2 != $game_variables[108]
	  $game_variables[108] = dys2
	  change = true
	end
	if dys3 != $game_variables[109]
	  $game_variables[109] = dys3
	  change = true
	end
	if dys4 != $game_variables[110]
	  $game_variables[110] = dys4
	  change = true
	end
	return change
  end

  def self.countdown_update
    return cdown_update(Time.new(2017, 03, 27))
  end

  def self.countdown_extend_update
    return cdown_update(Time.new(2017, 03, 27))
  end

  def self.countdown_update_rue
    if @rue_equinox == nil
	  @rue_equinox = Time.now + 6
	end
    return cdown_update(@rue_equinox)
  end

  def self.niko_reflection_update
    for event in $game_map.events.values
      if event.name == "niko reflection"
        event.real_y = 27*128/2 - ($game_player.real_y - 27*128/2)
        event.real_x = $game_player.real_x
        event.y = 14 - (($game_player.y - 13))
        event.x = $game_player.x
        if event.y > 14
          event.y = 14
        end
        if event.real_y > 14*128
          event.real_y = 14*128
        end
        event.direction = $game_player.direction
        event.pattern = $game_player.pattern
        case event.direction
          when 2
            event.direction = 8
          when 8
            event.direction = 2
        end
        return
      end
    end
  end

  def self.niko_reflection_enc_update
    for event in $game_map.events.values
      if event.name == "niko reflection"
        event.real_y = 20*128 - ($game_player.real_y - 20*128)
        event.real_x = $game_player.real_x
        event.y = 20 - (($game_player.y - 20))
        event.x = $game_player.x
        if event.y > 19
          event.y = 19
        end
        if event.real_y > 19*128
          event.real_y = 19*128
        end

        if event.x > 14
          event.x = 14
        elsif event.x < 6
		  event.x = 6
		end
        if event.real_x > (14*128) - 32
          event.real_x = (14*128) - 32
        elsif event.real_x < (6*128) + 32
          event.real_x = (6*128) + 32
        end

        event.direction = $game_player.direction
        event.pattern = $game_player.pattern
        case event.direction
          when 2
            event.direction = 8
          when 8
            event.direction = 2
        end
        return
      end
    end
  end


  def self.niko_reflection_peng_update
    for event in $game_map.events.values
      if event.name == "niko reflection"
        event.real_y = 20*128 - ($game_player.real_y - 20*128)
        event.real_x = $game_player.real_x
        event.y = 20 - (($game_player.y - 20))
        event.x = $game_player.x
        if event.y > 19
          event.y = 19
        end
        if event.real_y > 19*128
          event.real_y = 19*128
        end

        if event.x > 14
          event.x = 14
        elsif event.x < 6
		  event.x = 6
		end
        if event.real_x > (14*128) - 32
          event.real_x = (14*128) - 32
        elsif event.real_x < (6*128) + 32
          event.real_x = (6*128) + 32
        end

        return
      end
    end
  end

  # Temporary switch assignment
  def self.tmp_s1=(val)
    $game_switches[TMP_INDEX+0] = val
  end

  def self.tmp_s2=(val)
    $game_switches[TMP_INDEX+1] = val
  end

  def self.tmp_s3=(val)
    $game_switches[TMP_INDEX+2] = val
  end

  def self.tmp_v1=(val)
    $game_variables[TMP_INDEX+0] = val
  end

  def self.tmp_v2=(val)
    $game_variables[TMP_INDEX+1] = val
  end

  def self.tmp_v3=(val)
    $game_variables[TMP_INDEX+2] = val
  end

  private
  TMP_INDEX = 22

  def self.logpos(pos, realpos, positive)
    bigpos = pos * 128
    if realpos < bigpos
      return pos - 1 if positive
    elsif realpos > bigpos
      return pos + 1 if positive
    end
    return pos
  end

  def self.set_cam(x, y)
    $game_map.display_x = x
    $game_map.display_y = y
  end

  def self.fadein_bgm(target, speed)
    $game_temp.target_bgm_vol_level = target
    $game_temp.bgm_fadein_speed = speed
  end

  def self.copy_file_chmod(src, dest, chmod)
    File.open(dest, 'wb') do |output|
      File.open(src, 'rb') do |input|
        while buff = input.read(4096)
          output.write(buff)
        end
      end
    end
    File.chmod(chmod, dest)
  end

  def self.copy_journal
    # If the required directories don't exist, create them
    Dir.mkdir(Oneshot::GAME_PATH) unless File.exists?(Oneshot::GAME_PATH)
    Dir.mkdir(Oneshot::GAME_PATH + "/Oneshot") unless File.exists?(Oneshot::GAME_PATH + "/Oneshot")
    begin
      # If we're on a supported Linux DE, make a .desktop file
      # so the clover icon can be properly shown
      if Oneshot::OS == "linux" and SUPPORTED_DE.include? Oneshot::DE
        copy_file_chmod("#{Dir.pwd}/#{Oneshot::JOURNAL}", "#{Oneshot::SAVE_PATH}/#{Oneshot::JOURNAL}", 0755)
        copy_file_chmod("#{Dir.pwd}/#{Oneshot::JOURNAL}.png", "#{Oneshot::SAVE_PATH}/#{Oneshot::JOURNAL}.png", 0644)
        path = "#{Oneshot::GAME_PATH}/Oneshot/#{Oneshot::JOURNAL}.desktop"
        File.open(path, "wb") do |output|
          output.write("[Desktop Entry]\n")
          output.write("Comment=...\n")
          output.write("Terminal=false\n")
          output.write("Name=_______\n")
          output.write("Exec=#{Oneshot::SAVE_PATH}/#{Oneshot::JOURNAL}\n")
          output.write("Type=Application\n")
          output.write("Icon=#{Oneshot::SAVE_PATH}/#{Oneshot::JOURNAL}.png\n")
        end
        File.chmod(0755, path)
      # If the journal is a file, copy it to Documents
      elsif File.file?(Oneshot::JOURNAL)
        copy_file_chmod(Oneshot::JOURNAL, "#{Oneshot::GAME_PATH}/Oneshot/#{Oneshot::JOURNAL}", 0755)
      # If the journal isn't a file, symlink it
      else
        File.symlink "#{Dir.pwd}/#{Oneshot::JOURNAL}", "#{Oneshot::GAME_PATH}/Oneshot/#{Oneshot::JOURNAL}"
      end
    rescue Errno::EACCES => e
      # this probably means the clover.exe already exists and is running, so no need to create it again
    rescue Errno::EEXIST => e
      # this means that the journal file already exists, so no need to create it again
    end
    if File.exists?("README.txt")
      File.open("README.txt", "rb") do |input|
        File.open(Oneshot::GAME_PATH + "/Oneshot/README.txt","wb") do |output|
          while buff = input.read(4096)
            output.write(buff)
          end
        end
      end
    end
  end

  def self.create_boxes
    Dir.mkdir(Oneshot::GAME_PATH) unless File.exists?(Oneshot::GAME_PATH)
    Dir.mkdir(Oneshot::GAME_PATH + "/Oneshot") unless File.exists?(Oneshot::GAME_PATH + "/Oneshot")
    Dir.mkdir(Oneshot::GAME_PATH + "/Oneshot/Portal1") unless File.exists?(Oneshot::GAME_PATH + "/Oneshot/Portal1")
    Dir.mkdir(Oneshot::GAME_PATH + "/Oneshot/Portal2") unless File.exists?(Oneshot::GAME_PATH + "/Oneshot/Portal2")
    Dir.mkdir(Oneshot::GAME_PATH + "/Oneshot/Portal3") unless File.exists?(Oneshot::GAME_PATH + "/Oneshot/Portal3")
    Dir.mkdir(Oneshot::GAME_PATH + "/Oneshot/BigPortal") unless File.exists?(Oneshot::GAME_PATH + "/Oneshot/BigPortal")
  end

  def self.delete_if_exists(f_name)
    File.delete(f_name) unless !File.exists?(f_name)
  end

  def self.clear_boxes
    for i in 1..3
	  portal_path = Oneshot::GAME_PATH + "/Oneshot/Portal" + i.to_s
	  case i
	  when 1
	    delete_if_exists(portal_path + "/blue_npc_prototype.png")
	    delete_if_exists(portal_path + "/proto1.png")
	    delete_if_exists(portal_path + "/keyB.txt")
	  when 2
	    delete_if_exists(portal_path + "/green_npc_cedric.png")
	    delete_if_exists(portal_path + "/cedric.png")
	    delete_if_exists(portal_path + "/keyG.txt")
	  when 3
	    delete_if_exists(portal_path + "/red_rue.png")
	    delete_if_exists(portal_path + "/rue.png")
	    delete_if_exists(portal_path + "/keyR.txt")
	  end
	end
	delete_if_exists(Oneshot::GAME_PATH + "/Oneshot/BigPortal/keyB.txt")
	delete_if_exists(Oneshot::GAME_PATH + "/Oneshot/BigPortal/keyG.txt")
	delete_if_exists(Oneshot::GAME_PATH + "/Oneshot/BigPortal/keyR.txt")
  end

  def self.copy_file(src, dst)
    begin
      File.open(src, "rb") do |input|
        File.open(dst,"wb") do |output|
          while buff = input.read(4096)
            output.write(buff)
          end
        end
      end
	rescue Errno::EACCES => e
	  #this probably means the file already exists and is open, so no need to create it again
  rescue Errno::EEXIST => e
    #this probably means the file already exists, so no need to create it again
	end
  end

  def self.write_key(dst, str)
    File.open(dst, 'w') do |file|
      file.puts(str)
    end
  end

  def self.put_key_in_box(numb)
    portal_path = Oneshot::GAME_PATH + "/Oneshot/Portal" + numb.to_s
	case numb
	when 1
	  copy_file("Graphics/Characters/blue_npc_prototype.png", portal_path + "/blue_npc_prototype.png")
	  copy_file("Graphics/Faces/proto1.png", portal_path + "/proto1.png")
	  write_key(portal_path + "/keyB.txt", tr(PROTO_TEXT))
	when 2
	  copy_file("Graphics/Characters/green_npc_cedric.png", portal_path + "/green_npc_cedric.png")
	  copy_file("Graphics/Faces/cedric.png", portal_path + "/cedric.png")
	  write_key(portal_path + "/keyG.txt", tr(CEDRIC_TEXT))
	when 3
	  copy_file("Graphics/Characters/red_rue.png", portal_path + "/red_rue.png")
	  copy_file("Graphics/Faces/rue.png", portal_path + "/rue.png")
	  write_key(portal_path + "/keyR.txt", tr(RUE_TEXT))
	end

  end

  def self.password1
    locale = $persistent.langcode
	if !Language::LANGUAGES.include? locale
	  locale = 'en'
	end
    source_dir = "Graphics/Fogs/_/scenario1/" + locale.downcase
    Dir.mkdir(Oneshot::DOCS_PATH) unless File.exists?(Oneshot::DOCS_PATH)
    copy_file(source_dir + "/pw1.png", Oneshot::DOCS_PATH + "/ONESHOT_password1.png")
    copy_file(source_dir + "/pw2.png", Oneshot::DOCS_PATH + "/ONESHOT_password2.png")
    copy_file(source_dir + "/pw3.png", Oneshot::DOCS_PATH + "/ONESHOT_password3.png")
    copy_file(source_dir + "/pw4.png", Oneshot::DOCS_PATH + "/ONESHOT_password4.png")
  end

  def self.password2
    locale = $persistent.langcode
	if !Language::LANGUAGES.include? locale
	  locale = 'en'
	end
    source_dir = "Graphics/Fogs/_/scenario2/#{locale.downcase}"
    Dir.mkdir(Oneshot::DOCS_PATH) unless File.exists?(Oneshot::DOCS_PATH)
    copy_file("#{source_dir}/pw1.png", "#{Oneshot::DOCS_PATH}/ONESHOT_password1.png")
    copy_file("#{source_dir}/pw2.png", "#{Oneshot::DOCS_PATH}/ONESHOT_password2.png")
    copy_file("#{source_dir}/pw3.png", "#{Oneshot::DOCS_PATH}/ONESHOT_password3.png")
    copy_file("#{source_dir}/pw4.png", "#{Oneshot::DOCS_PATH}/ONESHOT_password4.png")
  end

=begin
  def self.take_key_out_of_box(numb)
	if File.exists?(Oneshot::GAME_PATH + "/Oneshot/Box" + numb.to_s + "/key" + numb.to_s + ".png")
      File.delete(Oneshot::GAME_PATH + "/Oneshot/Box" + numb.to_s + "/key" + numb.to_s + ".png")
	end
	if File.exists?(Oneshot::GAME_PATH + "/Oneshot/BigBox/key" + numb.to_s + ".png")
      File.delete(Oneshot::GAME_PATH + "/Oneshot/BigBox/key" + numb.to_s + ".png")
	end
  end
=end

  def self.is_key_in_box(numb)
    portal_path = Oneshot::GAME_PATH + "/Oneshot/Portal" + numb.to_s
	case numb
	when 1
	  return File.exists?(portal_path + "/keyB.txt")
	when 2
	  return File.exists?(portal_path + "/keyG.txt")
	when 3
	  return File.exists?(portal_path + "/keyR.txt")
	end
    return false
  end

  def self.is_key_in_bigbox(numb)
    portal_path = Oneshot::GAME_PATH + "/Oneshot/BigPortal"
	case numb
	when 1
	  return File.exists?(portal_path + "/keyB.txt")
	when 2
	  return File.exists?(portal_path + "/keyG.txt")
	when 3
	  return File.exists?(portal_path + "/keyR.txt")
	end
    return false
  end

end

def has_lightbulb?
  $game_party.item_number(1) > 0
end

def button_pressed?
  (1..18).each do |i|
    return true if Input.trigger?(i)
  end
  return false
end

def enter_name
  $game_temp.name_calling = true
end

def check_exit(min, max, x: -1, y: -1)
  result = false
  if x >= 0
    if $game_player.x == x
      if $game_player.y >= min && $game_player.y <= max
        result = true
      end
    end
  elsif y >= 0
    if $game_player.y == y
      if $game_player.x >= min && $game_player.x <= max
        result = true
      end
    end
  end
  Script.tmp_s1 = $game_switches[11] ? false : result
end

def loadQASave(fname)
  filename = "testing_saves/" + fname + ".rxdata"
    # If file doesn't exist
    unless FileTest.exist?(filename)
      filename = "testing_saves_postgame/" + fname + ".rxdata"
	  unless FileTest.exist?(filename)
        # Play buzzer SE
        $game_system.se_play($data_system.buzzer_se)
        return
      end
    end
    # Play load SE
    $game_system.se_play($data_system.load_se)

	# Read save data
    file = File.open(filename, "rb")
    # Read character data for drawing save file
    characters = Marshal.load(file)
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
    $game_troop         = Marshal.load(file)
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
      $game_player.center($game_player.x, $game_player.y)
    end
    # Refresh party members
    $game_party.refresh

	f_prev = $game_player
    for f in $game_followers
      f.leader = f_prev
      f.moveto($game_player.x, $game_player.y)
	  f_prev = f
    end
    file.close
    # Restore BGM and BGS
    $game_system.bgm_play($game_system.playing_bgm)
    $game_system.bgs_play($game_system.playing_bgs)
    # Update map (run parallel process event)
    $game_map.update
    # Switch to map screen
    $scene = Scene_Map.new
end

def kill_perma_flags
  for i in 151..175
    $game_switches[i] = false
  end
  for i in 76..100
    $game_variables[i] = 0
  end
end

def bg(name)
  $game_map.bg_name = name
end

def particles(type)
  $game_map.particles_type = type
end

def ambient(r, g, b, gray = 0)
  $game_map.ambient.set(r, g, b, gray)
end

def clear_ambient
  $game_map.ambient.set(0, 0, 0, 0)
end

def add_light(id, file, intensity, x, y)
  #$scene.add_light(id, file, intensity, x, y)
end

def del_light(id)
  #$scene.del_light(id)
end

def clear_lights
  #$scene.clear_lights
end

def wrap_map
  $game_map.wrapping = true
end

def pan_offset_y(val)
  $game_map.pan_offset_y = val
end

# Map specific settings
def green_ambient
  ambient(-30, -20, -30)
end

def blue_ambient
  ambient(-40, -40, -40)
end

def house_ambient
  ambient(-15, -15, -15)
end

# Travel
def enable_travel
  $game_fasttravel.enabled = true
end

def disable_travel
  $game_fasttravel.enabled = false
end

def unlock_map(zone, map, dir)
  # Find the coordinate of the calling event
  e = $game_map.events.values.find { |e| e.name == 'FAST TRAVEL' }
  raise 'could not find event named FAST TRAVEL' if !e
  case dir
  when :down
    newdir = 2
  when :left
    newdir = 4
  when :right
    newdir = 6
  when :up
    newdir = 8
  else
    raise "invalid direction: #{dir}"
  end
  $game_fasttravel.enabled = true
  $game_fasttravel.zone = zone
  $game_fasttravel.unlock(map, $game_map.map_id, e.x, e.y, newdir)
end

# Misc
def watcher_tell_time
  hour = Time.now.hour
  if hour >= 6 && hour < 12
    Script.tmp_v1 = 0
  elsif hour >= 12 && hour < 17
    Script.tmp_v1 = 1
  elsif hour >= 17
    Script.tmp_v1 = 2
  else
    Script.tmp_v1 = 3
  end
end

def plight_start_timer
  $game_oneshot.plight_timer = Time.now
end

def plight_update_timer
  if $game_oneshot.plight_timer == nil
    plight_start_timer
  end
  Script.tmp_v1 = ((Time.now - $game_oneshot.plight_timer) / 60).to_i
end

def activate_balcony?(ypos)
  !$scene.menu_open? &&
    $game_player.y == ypos &&
    $game_player.direction == 8 &&
    Input.trigger?(Input::ACTION) && !$game_system.map_interpreter.running? && !$game_temp.menus_visible
end
