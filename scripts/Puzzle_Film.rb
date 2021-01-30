def film_puzzle_begin
  Oneshot.reset_obscured
  $game_temp.filmsprite.dispose if $game_temp.filmsprite
  filmsprite = Sprite.new
  filmsprite.bitmap = RPG::Cache.picture('numbersheet')
  filmsprite.z = 9999
  filmsprite.obscured = true
  $game_temp.filmsprite = filmsprite
end

def film_puzzle_end
  $game_temp.filmsprite.dispose if $game_temp.filmsprite
  $game_temp.filmsprite = nil
end
