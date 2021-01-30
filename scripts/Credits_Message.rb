# Displays doc message boxes
class Credits_Message

  #--------------------------------------------------------------------------
  # * Object Initialization
  #--------------------------------------------------------------------------
  def initialize
    @timer = 0

    @viewport = Viewport.new(0, 0, 640, 480)
    @sprite_bg = Sprite.new(@viewport)
    @sprite_bg.bitmap = Bitmap.new(640, 480)
    if $game_switches[104]
      @sprite_bg.bitmap.fill_rect(0, 0, 640, 480, Color.new(0, 0, 0, 255))
    else
      @sprite_bg.bitmap.fill_rect(0, 0, 640, 480, Color.new(255, 255, 255, 255))
    end
    @sprite_text = Sprite.new(@viewport)
    @contents = Bitmap.new(320, 240)
    Language.register_text_sprite(self.class.name + "_contents", @contents)
    @contents.font.size = 16
    @sprite_text.bitmap = @contents
    @sprite_bg.z = 0
    @sprite_text.z = 1
    @sprite_text.zoom_x = @sprite_text.zoom_y = 2
    @viewport.z = 9999
    @viewport.visible = false

    @sprite_bg.opacity = 0
    @sprite_text.opacity = 0

    # Animation flags
    @fade_in = false
    @fade_out = false
  end
  #--------------------------------------------------------------------------
  # * Dispose
  #--------------------------------------------------------------------------
  def dispose
    terminate_message
    $game_temp.message_window_showing = false
    @contents.dispose
    #@sprite_bg.dispose
    @sprite_text.dispose
    @viewport.dispose
  end
  #--------------------------------------------------------------------------
  # * Terminate Message
  #--------------------------------------------------------------------------
  def terminate_message
    # Call message callback
    if !@skip_message_proc && $game_temp.message_proc != nil
      $game_temp.message_proc.call
      $game_temp.message_proc = nil
    end
    $game_temp.message_credits_text = nil
  end
  #--------------------------------------------------------------------------
  # * Refresh: Load new message text and pre-process it
  #--------------------------------------------------------------------------
  def refresh

    if $game_switches[104]
      @sprite_bg.bitmap.fill_rect(0, 0, 640, 480, Color.new(0, 0, 0, 255))
    else
      @sprite_bg.bitmap.fill_rect(0, 0, 640, 480, Color.new(255, 255, 255, 255))
    end

    # Initialize
    @timer = 0
    text = ''
    y = -1
    widths = []
	right_widths = []

    # Pre-process text
    text_raw = $game_temp.message_credits_text.to_str

    # Substitute variables, actors, player name, newlines, etc
    text_raw.gsub!(/\\v\[([0-9]+)\]/) do
      $game_variables[$1.to_i]
    end
    text_raw.gsub!(/\\n\[([0-9]+)\]/) do
      $game_actors[$1.to_i] != nil ? $game_actors[$1.to_i].name : ""
    end
    text_raw.gsub!("\\p", $game_oneshot.player_name)
    text_raw.gsub!("\\n", "\n")
    text_raw.gsub!("\\l", "\005")
    text_raw.gsub!("\\r", "\006")
    # Handle text-rendering escape sequences
    text_raw.gsub!(/\\c\[([0-9]+)\]/, "\000[\\1]")
    # Finally convert the backslash back
    text_raw.gsub!("\\\\", "\\")

    @contents.font.size = 16

    # Now split text into lines by measuring text metrics
    x = y = 0
	right_x = 0
    maxwidth = @contents.width - 8
    spacesize = @contents.text_size(' ')
    for i in text_raw.split(/ /)
      # Split each word around newlines
      newline = false
      for j in i.split("\n")
        # Handle newline
        if newline
          text << "\n"
          widths << x
		  right_widths << right_x
          x = 0
		  right_x = 0
          y += 1
          @contents.font.size = 24
        else
          newline = true
        end

		if j.start_with?("\006")
		  right_x = 0
		  x = 0
		end

        # Get width of this word and see if it goes out of bounds
        width = @contents.text_size(j.gsub(/\000\[[0-9]+\]/, '').gsub( "\005","").gsub( "\006","")).width
        if x + width > maxwidth
          text << "\n"
          widths << x
		  right_widths << right_x
          x = 0
		  right_x = 0
          y += 1
          @contents.font.size = 24
        end

        # Append word to list
        if x == 0
          text << j
        else
          text << ' ' << j
        end
        x += width + spacesize.width
		right_x += width
      end
    end
    widths << x
	right_widths << right_x

    # Prepare renderer
    @contents.clear
    if $game_switches[104]
      @contents.font.color = Color.new(255, 255, 255, 255)
    else
      @contents.font.color = Color.new(0, 0, 0, 255)
    end

    x = (320 - widths[0]) / 2
    y = 0
    y_top = (240 - widths.length * 24) / 2

    @contents.font.size = 16


    # Get 1 text character in c (loop until unable to get text)
    while ((c = text.slice!(/./m)) != nil)
      # \n
      if c == "\n"
        y += 1
        @contents.font.size = 24
        x = (320 - widths[y]) / 2
        next
      end
      # \c[n]
      if c == "\000"
        # Change text color
        text.sub!(/\[([0-9]+)\]/, "")
        color = $1.to_i
        if color >= 0 and color <= 7
          @contents.font.color = Window_Base.text_color(color)
        end
        # go to next text
        next
      end
	  if c == "\005"
	    x = 16
        next
	  end
	  if c == "\006"
	    x = 320 - (right_widths[y] + 16)
        next
	  end
      # Draw text
      @contents.draw_text(x, y_top + (y * 24), 40, 18, c)
      # Add x to drawn text width
      x += @contents.text_size(c).width
    end
    Graphics.frame_reset
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    # Handle fade-out effect
    if @fade_out
      @sprite_text.opacity -= 5
      @sprite_bg.opacity -= 5
      if @sprite_text.opacity <= 0
        @sprite_text.opacity = 0
        @sprite_bg.opacity = 0
        @fade_out = false
        @viewport.visible = false
        $game_temp.message_window_showing = false
      end
      return
    end

    # Handle fade-in effect
    if @fade_in
      @sprite_text.opacity += 5
      @sprite_bg.opacity += 5
      if @sprite_text.opacity >= 255
        @sprite_text.opacity = 255
        @sprite_bg.opacity = 255
        @fade_in = false
        $game_temp.message_window_showing = true
      end
      return
    end

    # Message is over and should be hidden or advanced to next
    if $game_temp.message_credits_text == nil
      @fade_out = true if @viewport.visible
    else
      if !@viewport.visible
        # Fade in
        refresh
        @viewport.visible = true
        @fade_in = true
      end
    end

    if visible
      @timer += 1
      if @timer >= 150
        terminate_message
        @fade_out = true
      end
    end
  end
  #--------------------------------------------------------------------------
  # * Variables
  #--------------------------------------------------------------------------
  def visible
    @viewport.visible
  end
  def visible=(val)
    @viewport.visible = val
  end
end
