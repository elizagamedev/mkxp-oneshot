X = false
O = true
CORRECT_PIXEL_PUZZLE = [X,O,O,O,X,
                        O,X,X,X,O,
                        O,X,O,X,O,
                        O,X,X,X,O,
                        X,O,O,O,X,
                        X,X,O,X,X]


BLANK_PXL_PZZLE = [X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X]


S1_PXL_PZZLE =    [X,X,X,X,X,O,X,X,X,X,X,
                   X,X,X,X,X,O,X,X,X,X,X,
                   X,X,X,X,X,O,X,X,X,X,X,
                   X,X,X,X,X,O,X,X,X,X,X,
                   X,X,X,X,X,O,X,X,X,X,X,
                   X,X,X,X,X,O,X,X,X,X,X,
                   X,X,X,X,X,O,X,X,X,X,X,
                   X,X,X,X,X,O,X,X,X,X,X,
                   X,X,X,X,X,O,X,X,X,X,X,
                   X,X,X,X,X,O,X,X,X,X,X,
                   X,X,X,X,X,O,X,X,X,X,X]

S2_PXL_PZZLE =    [X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,O,O,O,O,O,X,X,X,
                   X,X,O,O,X,X,X,O,O,X,X,
                   X,O,O,X,X,X,X,X,O,O,X,
                   X,O,X,X,X,X,X,X,X,O,X,
                   X,O,X,X,X,X,X,X,X,O,X,
                   X,O,X,X,X,X,X,X,X,O,X,
                   X,O,O,X,X,X,X,X,O,O,X,
                   X,X,O,O,X,X,X,O,O,X,X,
                   X,X,X,O,O,O,O,O,X,X,X,
                   X,X,X,X,O,O,O,X,X,X,X]


S3_PXL_PZZLE =    [O,O,O,O,O,O,O,O,O,O,X,
                   O,O,O,O,O,O,O,O,O,O,X,
                   O,O,O,O,O,O,X,X,X,X,X,
                   O,O,O,O,O,O,X,X,X,X,X,
                   O,O,O,O,O,O,X,X,X,X,X,
                   O,O,O,O,O,O,X,X,X,X,X,
                   O,O,O,O,O,O,X,X,X,X,X,
                   O,O,O,O,O,O,X,X,X,X,X,
                   O,O,O,O,O,O,X,X,X,X,X,
                   O,O,O,O,O,O,X,X,X,X,X,
                   O,O,O,O,O,O,X,X,X,X,X]


S4_PXL_PZZLE =    [X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,O,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X,
                   X,X,X,X,X,X,X,X,X,X,X]

S5_PXL_PZZLE =    [X,X,O,O,O,O,O,O,O,X,X,
                   X,O,O,X,X,X,X,X,O,O,X,
                   O,O,X,O,O,O,O,O,X,O,O,
                   O,X,O,O,X,X,X,O,O,X,O,
                   O,X,O,X,X,X,X,X,O,X,O,
                   O,X,O,X,X,X,X,X,O,X,O,
                   O,X,O,X,X,X,X,X,O,X,O,
                   O,X,O,O,X,X,X,O,O,X,O,
                   O,O,X,O,O,O,O,O,X,O,O,
                   X,O,O,X,X,X,X,X,O,O,X,
                   X,X,O,O,O,O,O,O,O,X,X,
                   X,X,X,X,O,O,O,X,X,X,X]

def pixel_puzzle_check
  result = true
  $game_map.events.each do |key, event|
    next unless event.name =~ /^pixel /
    x = event.x - 31
    y = event.y - 34
    if CORRECT_PIXEL_PUZZLE[y*5+x] != $game_self_switches[[$game_map.map_id, event.id, 'A']]
      result = false
      break
    end
  end
  Script.tmp_s1 = result
end

def puzzle_check(puzzle)
  result = true
  $game_map.events.each do |key, event|
    next unless event.name =~ /^pixel /
    x = event.x - 5
    y = event.y - 2
    puzzle_array = S1_PXL_PZZLE
    case puzzle
      when "s1"
        puzzle_array = S1_PXL_PZZLE
      when "s2"
        puzzle_array = S2_PXL_PZZLE
      when "s3"
        puzzle_array = S3_PXL_PZZLE
      when "s4"
        puzzle_array = S4_PXL_PZZLE
      when "s5"
        puzzle_array = S5_PXL_PZZLE
    end
    if puzzle_array[y*11+x] != $game_self_switches[[$game_map.map_id, event.id, 'A']]
      result = false
      break
    end
  end
  Script.tmp_s1 = result
end

def lightbulb_room_fix
  if (($game_player.x < 5) || ($game_player.x > 15) || ($game_player.y < 2) || ($game_player.y > 13))
    $game_player.x = 10
	$game_player.y = 13
	$game_player.real_x = 10 * 4 * 32
    $game_player.real_y = 13 * 4 * 32
  end
end

def pixel_puzzle_niko_correct(puzzle)
  niko_x = $game_variables[7]
  niko_y = $game_variables[8]
  test_x = niko_x - 5
  test_y = niko_y - 2
  test_left_x = niko_x - 5
  test_up_y = niko_y - 2
  test_right_x = niko_x - 5
  test_down_y = niko_y - 2
  x_search = 0
  y_search = 0
  puzzle_array = S1_PXL_PZZLE
  case puzzle
    when "s1"
      puzzle_array = S1_PXL_PZZLE
    when "s2"
      puzzle_array = S2_PXL_PZZLE
    when "s3"
      puzzle_array = S3_PXL_PZZLE
    when "s4"
      puzzle_array = S4_PXL_PZZLE
    when "s5"
      puzzle_array = S5_PXL_PZZLE
  end
  while true
    if puzzle_array[test_y*11+test_left_x]
      $game_variables[7] = niko_x - x_search
      return
    elsif puzzle_array[test_y*11+test_right_x]
      $game_variables[7] = niko_x + x_search
      return
    elsif puzzle_array[test_up_y*11+test_x]
      $game_variables[8] = niko_y - y_search
      return
    elsif puzzle_array[test_down_y*11+test_x]
      $game_variables[8] = niko_y + y_search
      return
    end

    x_search += 1
    y_search += 1

    test_left_x -= 1
    if test_left_x < 0
      test_left_x = 0
    end
    test_right_x += 1
    if test_right_x > 10
      test_right_x = 10
    end
    test_up_y -= 1
    if test_up_y < 0
      test_up_y = 0
    end
    test_down_y += 1
    if test_down_y > 11
      test_down_y = 11
    end

    if x_search > 200 || y_search > 200
    # something has gone horribly wrong if we've been searching this long, just leave the function
      return
    end
  end

end

def pixel_puzzle_reset
  $game_map.events.each do |key, event|
    next unless event.name =~ /^pixel /
    $game_self_switches[[$game_map.map_id, event.id, 'A']] = false
  end
  $game_map.need_refresh = true
end
