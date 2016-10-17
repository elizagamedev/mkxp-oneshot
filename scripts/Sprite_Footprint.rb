class Sprite_Footprint < Sprite
  def initialize(viewport, direction, x, y)
    super(viewport)
    @direction = direction
    @real_x = x * 4 * 32
    @real_y = y * 4 * 32

    self.zoom_x = 2
    self.zoom_y = 2
    self.ox = 8
    self.oy = 16
    self.bitmap = RPG::Cache.misc('footprints')
    self.src_rect.set(0, 16 * (direction / 2 - 1), 16, 16)
    update
  end

  def update
    return if disposed?

    self.x = (@real_x - $game_map.display_x + 3) / 4 + 16
    self.y = (@real_y - $game_map.display_y + 3) / 4 + 32

    self.opacity -= 4
    if self.opacity == 0
      dispose
    end
  end

  def correctX(xDelta)
    @real_x += xDelta*4*32
  end
  def correctY(yDelta)
    @real_y += yDelta*4*32
  end
end
