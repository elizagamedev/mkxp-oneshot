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
  ambient(-40, -40, -40)
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
    Input.trigger?(Input::ACTION)
end
