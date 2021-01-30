class Sprite_MapText < Sprite

  def initialize(viewport, text, x, y)

    super(viewport)
	@direction = 2
    @real_x = x * 4 * 32
    @real_y = y * 4 * 32

    self.bitmap = Bitmap.new(200, 24)
    if (Language::FONT_WESTERN == Font.default_name)
      self.zoom_x = 2
      self.zoom_y = 2
    else
      self.zoom_x = 1.5
      self.zoom_y = 1.5
    end

	#calculate text width
    spacewidth = self.bitmap.text_size(' ').width
	width = 0

	text.split(' ').each do |word|

      # Get width of this word
      width += self.bitmap.text_size(word.gsub(/(\000\[[0-9]+\]|\001|\002)/, '')).width
      width += spacewidth
    end
    width -= spacewidth

    if width <= 0
      width = 1
    end

	self.bitmap.dispose
	self.bitmap = Bitmap.new(width, 24)
	self.bitmap.font.color = Color.new(81, 33, 129, 255)
	self.bitmap.draw_text(0,0, width, 24, text)

    self.src_rect.set(0, 0, width, 24)
    self.oy = 24
    self.ox = width/2


    update
  end

  def update
    return if disposed?

    self.x = (@real_x - $game_map.display_x + 3) / 4 + 16
    self.y = (@real_y - $game_map.display_y + 3) / 4 + 32

  end

  def correctX(xDelta)
    #do nothing
  end
  def correctY(yDelta)
    #do nothing
  end

end
