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
    equinox = Time.new(2017, 03, 20)
	diff = equinox - Time.now
	if(diff <= 0)
	  return true
	end
	return false
  end

  def self.countdown_update
    equinox = Time.new(2017, 03, 20)
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

  def self.copy_journal
    Dir.mkdir(Oneshot::DOCS_PATH + "\\My Games") unless File.exists?(Oneshot::DOCS_PATH + "\\My Games")
    Dir.mkdir(Oneshot::DOCS_PATH + "\\My Games\\Oneshot") unless File.exists?(Oneshot::DOCS_PATH + "\\My Games\\Oneshot")
    File.open("_______.exe", "rb") do |input|
      File.open(Oneshot::DOCS_PATH + "\\My Games\\Oneshot\\_______.exe","wb") do |output|
        while buff = input.read(4096)
          output.write(buff)
        end
      end
    end
	if File.exists?("README.txt")
      File.open("README.txt", "rb") do |input|
        File.open(Oneshot::DOCS_PATH + "\\My Games\\Oneshot\\README.txt","wb") do |output|
          while buff = input.read(4096)
            output.write(buff)
          end
        end
      end
	end
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
  Script.tmp_v1 = ((Time.now - $game_oneshot.plight_timer) / 60).to_i
end

def activate_balcony?(ypos)
  !$scene.menu_open? &&
    $game_player.y == ypos &&
    $game_player.direction == 8 &&
    Input.trigger?(Input::ACTION) && !$game_system.map_interpreter.running? && !$game_temp.menus_visible
end
