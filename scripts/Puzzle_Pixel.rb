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
    end
    if puzzle_array[y*11+x] != $game_self_switches[[$game_map.map_id, event.id, 'A']]
      result = false
      break
    end
  end
  Script.tmp_s1 = result
end

def pixel_puzzle_reset
  $game_map.events.each do |key, event|
    next unless event.name =~ /^pixel /
    $game_self_switches[[$game_map.map_id, event.id, 'A']] = false
  end
  $game_map.need_refresh = true
end
