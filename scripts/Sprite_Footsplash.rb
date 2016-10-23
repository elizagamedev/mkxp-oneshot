class Sprite_Footsplash < Sprite
  def initialize(viewport, direction, x, y)

    super(viewport)
    y = y + 2
    @direction = direction
    @real_x = x * 4 * 32
    @real_y = y * 4 * 32

    case direction
    when 2
      @real_y -= 32
    when 4
      @real_x += 32
    when 6
      @real_x -= 32
    when 8
      @real_y += 32
    end

    @real_y += 48

    self.zoom_x = 2
    self.zoom_y = 2
    self.ox = 40
    self.oy = 80
    self.bitmap = RPG::Cache.misc('foot_splash')
    self.src_rect.set(0, 0, 80, 80)
    update
  end

  def update
    return if disposed?

    self.x = (@real_x - $game_map.display_x + 3) / 4 + 16
    self.y = (@real_y - $game_map.display_y + 3) / 4 + 32

    self.opacity -= 6
    frameIndex = 20 - (self.opacity / 13)
    frameX = frameIndex % 4
    frameY = frameIndex / 4
    self.src_rect.set(80*frameX, 80*frameY, 80, 80)
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
