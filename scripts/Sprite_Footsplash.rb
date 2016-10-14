class Sprite_Footsplash < Sprite
  def initialize(viewport, direction, x, y)

    super(viewport)
    y = y + 1
    @direction = direction
    @real_x = x * 4 * 32
    @real_y = y * 4 * 32

    self.zoom_x = 1
    self.zoom_y = 1
    self.ox = 48
    self.oy = 96
    self.bitmap = RPG::Cache.misc('foot_splash')
    self.src_rect.set(0, 0, 96, 96)
    update
  end

  def update
    return if disposed?

    self.x = (@real_x - $game_map.display_x + 3) / 4 + 16
    self.y = (@real_y - $game_map.display_y + 3) / 4 + 32

    self.opacity -= 6
    frameIndex = 12 - (self.opacity / 21)
    frameX = frameIndex % 4
    frameY = frameIndex / 4
    self.src_rect.set(96*frameX, 96*frameY, 96, 96)
    if self.opacity == 0
      dispose
    end
  end
end
