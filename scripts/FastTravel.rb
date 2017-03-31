class FastTravel
  MARGIN = 30
  TITLE_TOP_MARGIN = 32
  TITLE_MARGIN = 100
  ITEM_SPACING = 28
  ACTIVE_MARGIN = MARGIN * 2 + 20

  def initialize
    @viewport = Viewport.new(0, 0, 640, 480)
    @bg = Sprite.new(@viewport)
    @bg.bitmap = Bitmap.new(640, 480)
    @bg.bitmap.fill_rect(0, 0, 640, 480, Color.new(0, 0, 0, 128))
    @title = Sprite.new(@viewport)
    Language.register_text_sprite(self.class.name + "_title", @title)
    @title.bitmap = Bitmap.new(320, TITLE_MARGIN)
    @title.bitmap.font.size = 40
    @title.y = TITLE_TOP_MARGIN
    @title.x = MARGIN
    @data_sprites = []
    @viewport.z = 9998

    self.visible = false
    @index = 0
    @fade_in = false
    @fade_out = false

    @transfer_player = nil
  end

  def dispose
    @bg.dispose
    @title.dispose
    @data_sprites.each do |spr|
      spr.dispose
    end
    @viewport.dispose
  end

  def open
    self.visible = true
    self.opacity = 0
    @fade_in = true
    @data = $game_fasttravel.unlocked_maps.keys.sort

    # Set cursor to current map
    @data.each_with_index do |map, i|
      if $game_fasttravel.unlocked_maps[map].id == $game_map.map_id
        @index = i
        break
      end
    end
    zone = ZONES[$game_fasttravel.zone]

    # Create title
    @title.bitmap.clear
    @title.bitmap.draw_text(0, 0, @title.bitmap.width, @title.bitmap.height, zone.name)

    # Create menu options
    @data.each_with_index do |item, i|
      spr = Sprite.new(@viewport)
      spr.bitmap = Bitmap.new(320, ITEM_SPACING)
      spr.x = MARGIN
      spr.y = TITLE_MARGIN + TITLE_TOP_MARGIN + ITEM_SPACING * i
      spr.opacity = 0
      spr.bitmap.draw_text(0, 0, spr.bitmap.width, spr.bitmap.height, zone.maps[item])
      @data_sprites << spr
    end
  end

  def update
    if @fade_in
      self.opacity += 20
      active_spr = nil
      @data_sprites.each do |spr|
        spr.opacity += 10
        spr.opacity = 128 if spr.opacity > 128
        active_spr = spr if !active_spr && spr.x < MARGIN * 2
      end
      if active_spr
        active_spr.x += 6
        active_spr.x = MARGIN * 2 if active_spr.x > MARGIN * 2
      elsif self.opacity == 255
        @fade_in = false
      end
      return
    end

    if @fade_out
      self.opacity -= 20
      @data_sprites.each do |spr|
        spr.opacity -= 10
      end
      if self.opacity == 0
        @fade_out = false
        self.visible = false
        @data_sprites.each do |spr|
          spr.dispose
        end
        @data_sprites = []

        if @transfer_player
          $game_temp.player_transferring = true
          $game_temp.player_new_map_id = @transfer_player.id
          $game_temp.player_new_x = @transfer_player.x
          $game_temp.player_new_y = @transfer_player.y
          $game_temp.player_new_direction = @transfer_player.dir
          Graphics.freeze
          $game_temp.transition_processing = true
          $game_temp.transition_name = "black"
          @transfer_player = nil
        end
      end
      return
    end

    return if !self.visible

    # Adjust position and visibility
    @data_sprites.each_with_index do |spr, i|
      if i == @index
        if spr.x < ACTIVE_MARGIN
          spr.x += 6
          spr.x = ACTIVE_MARGIN if spr.x > ACTIVE_MARGIN
        end
        spr.opacity += 10 if spr.opacity < 255
      else
        if spr.x > MARGIN * 2
          spr.x -= 6
          spr.x = MARGIN * 2 if spr.x < MARGIN * 2
        end
        spr.opacity -= 10 if spr.opacity > 128
        spr.opacity = 128 if spr.opacity < 128
      end
    end

    if Input.trigger?(Input::UP)
      @index = (@index - 1) % @data.size
      $game_system.se_play($data_system.cursor_se)
    end
    if Input.trigger?(Input::DOWN)
      @index = (@index + 1) % @data.size
      $game_system.se_play($data_system.cursor_se)
    end

    if Input.trigger?(Input::ACTION)
      $game_system.se_play($data_system.decision_se)
      choice = $game_fasttravel.unlocked_maps[@data[@index]]
      if choice.id != $game_map.map_id
        @transfer_player = choice
      end
      @fade_out = true
      return
    end

    if Input.trigger?(Input::CANCEL)
      $game_system.se_play($data_system.cancel_se)
      @fade_out = true
    end
  end

  # Attributes
  def visible
    @viewport.visible
  end
  def visible=(val)
    @viewport.visible = val
  end
  def opacity=(val)
    @bg.opacity = val
    @title.opacity = val
  end
  def opacity
    @bg.opacity
  end
end
